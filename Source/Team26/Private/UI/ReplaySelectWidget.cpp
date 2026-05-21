#include "UI/ReplaySelectWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Replay/ReplayFileLibrarys.h"
#include "Replay/ReplayGameInstanceSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogReplaySelectWidget, Log, All);

namespace
{
	void SetTextIfValid(UTextBlock* TextBlock, const FString& Text)
	{
		if (TextBlock)
		{
			TextBlock->SetText(FText::FromString(Text));
		}
	}

	bool IsSameFilePath(const FString& A, const FString& B)
	{
		FString NormalizedA = FPaths::ConvertRelativePathToFull(A);
		FString NormalizedB = FPaths::ConvertRelativePathToFull(B);
		FPaths::NormalizeFilename(NormalizedA);
		FPaths::NormalizeFilename(NormalizedB);
		return NormalizedA.Equals(NormalizedB, ESearchCase::IgnoreCase);
	}

	void SetGameInputMode(APlayerController* PlayerController)
	{
		if (!PlayerController)
		{
			return;
		}

		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

	void SetReplaySelectInputMode(APlayerController* PlayerController, UUserWidget* WidgetToFocus)
	{
		if (!PlayerController || !WidgetToFocus)
		{
			return;
		}

		PlayerController->bShowMouseCursor = true;

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
	}
}

void UReplaySelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ReplayListView)
	{
		ReplayListView->OnItemClicked().RemoveAll(this);
		ReplayListView->OnItemClicked().AddUObject(this, &UReplaySelectWidget::HandleReplayItemClicked);
	}

	if (PlayButton)
	{
		PlayButton->OnClicked.RemoveAll(this);
		PlayButton->OnClicked.AddDynamic(this, &UReplaySelectWidget::HandlePlayClicked);
	}

	if (DeleteButton)
	{
		DeleteButton->OnClicked.RemoveAll(this);
		DeleteButton->OnClicked.AddDynamic(this, &UReplaySelectWidget::HandleDeleteClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UReplaySelectWidget::HandleBackClicked);
	}

	SetReplaySelectInputMode(GetOwningPlayer(), this);
	RefreshReplayList();
	UpdatePreviewPanel();
}

void UReplaySelectWidget::RefreshReplayList()
{
	if (!ReplayListView)
	{
		UE_LOG(LogReplaySelectWidget, Warning, TEXT("ReplayListView is not bound."));
		UpdatePreviewPanel();
		return;
	}

	ReplayListView->ClearListItems();

	SelectedReplay = FReplayFileInfo();
	bHasSelection = false;
	bHasReplayFiles = false;

	UReplayGameInstanceSubsystem* ReplaySubsystem =
		GetGameInstance() ? GetGameInstance()->GetSubsystem<UReplayGameInstanceSubsystem>() : nullptr;

	auto AddReplayEntry = [this](const FReplayFileInfo& ReplayInfo)
	{
		UReplayEntryObject* EntryObject = NewObject<UReplayEntryObject>(this);
		if (!EntryObject)
		{
			return static_cast<UReplayEntryObject*>(nullptr);
		}

		EntryObject->Info = ReplayInfo;
		ReplayListView->AddItem(EntryObject);
		return EntryObject;
	};

	TArray<FReplayFileInfo> Replays;
	UReplayFileLibrary::FindTrajectoryReplays(Replays);
	bHasReplayFiles = Replays.Num() > 0;

	const FString LastRecordedPath = ReplaySubsystem ? ReplaySubsystem->GetLastRecordedReplayPath() : FString();
	const FReplayFileInfo* ReplayToSelect = nullptr;

	if (!LastRecordedPath.IsEmpty())
	{
		ReplayToSelect = Replays.FindByPredicate([&LastRecordedPath](const FReplayFileInfo& Info)
		{
			return IsSameFilePath(Info.FilePath, LastRecordedPath) && Info.SampleCount > 0;
		});

		if (!ReplayToSelect && ReplaySubsystem)
		{
			ReplaySubsystem->ClearLastRecordedReplayPath();
		}
	}

	if (!ReplayToSelect)
	{
		ReplayToSelect = Replays.FindByPredicate([](const FReplayFileInfo& Info)
		{
			return Info.SampleCount > 0 && FPaths::FileExists(Info.FilePath);
		});
	}

	if (!ReplayToSelect && Replays.Num() > 0)
	{
		ReplayToSelect = &Replays[0];
	}

	for (const FReplayFileInfo& ReplayInfo : Replays)
	{
		UReplayEntryObject* EntryObject = AddReplayEntry(ReplayInfo);
		if (!EntryObject)
		{
			continue;
		}

		if (ReplayToSelect && IsSameFilePath(ReplayInfo.FilePath, ReplayToSelect->FilePath))
		{
			ReplayListView->SetSelectedItem(EntryObject);

			SelectedReplay = ReplayInfo;
			bHasSelection = true;
		}
	}

	if (ReplaySubsystem && bHasSelection && SelectedReplay.SampleCount > 0)
	{
		ReplaySubsystem->SetLastRecordedReplayPath(SelectedReplay.FilePath);
	}

	UpdatePreviewPanel();
}

void UReplaySelectWidget::HandleReplayItemClicked(UObject* ClickedItem)
{
	const UReplayEntryObject* EntryObject = Cast<UReplayEntryObject>(ClickedItem);
	if (!EntryObject)
	{
		return;
	}

	SelectedReplay = EntryObject->Info;
	bHasSelection = true;

	UpdatePreviewPanel();
}

void UReplaySelectWidget::UpdatePreviewPanel()
{
	const bool bHasExistingSelection = bHasSelection && FPaths::FileExists(SelectedReplay.FilePath);
	const bool bCanUseReplay = bHasExistingSelection && SelectedReplay.SampleCount > 0;

	if (PlayButton)
	{
		PlayButton->SetIsEnabled(bCanUseReplay);
	}

	if (DeleteButton)
	{
		DeleteButton->SetIsEnabled(bCanUseReplay);
	}

	if (!bHasExistingSelection)
	{
		SetTextIfValid(
			PreviewTitleText,
			bHasReplayFiles ? TEXT("NO REPLAY SELECTED") : TEXT("저장된 CSV가 없습니다")
		);
		SetTextIfValid(PreviewDurationText, TEXT("-"));
		SetTextIfValid(PreviewDateText, TEXT("-"));
		SetTextIfValid(PreviewPathText, TEXT(""));
		return;
	}

	SetTextIfValid(PreviewTitleText, SelectedReplay.DisplayName);
	SetTextIfValid(
		PreviewDurationText,
		SelectedReplay.SampleCount > 0 ? SelectedReplay.DurationText : TEXT("No samples")
	);
	SetTextIfValid(PreviewDateText, FString::Printf(TEXT("%s %s"), *SelectedReplay.DateText, *SelectedReplay.TimeText));
	SetTextIfValid(PreviewPathText, SelectedReplay.FilePath);
}

void UReplaySelectWidget::HandlePlayClicked()
{
	if (!bHasSelection || SelectedReplay.SampleCount <= 0 || !FPaths::FileExists(SelectedReplay.FilePath))
	{
		UpdatePreviewPanel();
		return;
	}

	UReplayGameInstanceSubsystem* ReplaySubsystem =
		GetGameInstance() ? GetGameInstance()->GetSubsystem<UReplayGameInstanceSubsystem>() : nullptr;

	if (!ReplaySubsystem)
	{
		UE_LOG(LogReplaySelectWidget, Warning, TEXT("ReplayGameInstanceSubsystem not found."));
		return;
	}

	if (ReplayPlaybackLevelName.IsNone())
	{
		UE_LOG(LogReplaySelectWidget, Warning, TEXT("ReplayPlaybackLevelName is not set."));
		return;
	}

	ReplaySubsystem->SetSelectedReplayPath(SelectedReplay.FilePath);

	SetGameInputMode(GetOwningPlayer());
	UGameplayStatics::OpenLevel(this, ReplayPlaybackLevelName);
}

void UReplaySelectWidget::HandleDeleteClicked()
{
	if (!bHasSelection || !FPaths::FileExists(SelectedReplay.FilePath))
	{
		UpdatePreviewPanel();
		return;
	}

	if (UReplayFileLibrary::DeleteTrajectoryReplay(SelectedReplay.FilePath))
	{
		if (UReplayGameInstanceSubsystem* ReplaySubsystem =
			GetGameInstance() ? GetGameInstance()->GetSubsystem<UReplayGameInstanceSubsystem>() : nullptr)
		{
			ReplaySubsystem->ClearSelectedReplayPath();

			if (IsSameFilePath(ReplaySubsystem->GetLastRecordedReplayPath(), SelectedReplay.FilePath))
			{
				ReplaySubsystem->ClearLastRecordedReplayPath();
			}
		}

		RefreshReplayList();
	}
}

void UReplaySelectWidget::HandleBackClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogReplaySelectWidget, Warning, TEXT("Owning player not found."));
		return;
	}

	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogReplaySelectWidget, Warning, TEXT("MainMenuWidgetClass is not set."));
		return;
	}

	UUserWidget* MainWidget = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
	if (!MainWidget)
	{
		UE_LOG(LogReplaySelectWidget, Warning, TEXT("Failed to create main menu widget."));
		return;
	}

	RemoveFromParent();
	MainWidget->AddToViewport(100);
	SetReplaySelectInputMode(PC, MainWidget);
}
