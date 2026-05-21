#include "UI/MainMenuWidget.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogMainMenuWidget, Log, All);

namespace
{
	void SetGameInputMode(APlayerController* PlayerController)
	{
		if (!PlayerController)
		{
			return;
		}

		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

	void SetMenuInputMode(APlayerController* PlayerController, UUserWidget* WidgetToFocus)
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

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartGameButton)
	{
		StartGameButton->OnClicked.RemoveAll(this);
		StartGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleStartGameClicked);
	}

	if (ReplayButton)
	{
		ReplayButton->OnClicked.RemoveAll(this);
		ReplayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleReplayClicked);
	}

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveAll(this);
		ExitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleExitClicked);
	}
}

void UMainMenuWidget::HandleStartGameClicked()
{
	if (GameLevelName.IsNone())
	{
		UE_LOG(LogMainMenuWidget, Warning, TEXT("GameLevelName is not set."));
		return;
	}

	SetGameInputMode(GetOwningPlayer());
	UGameplayStatics::OpenLevel(this, GameLevelName);
}

void UMainMenuWidget::HandleReplayClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		UE_LOG(LogMainMenuWidget, Warning, TEXT("Owning player not found."));
		return;
	}

	if (!ReplaySelectWidgetClass)
	{
		UE_LOG(LogMainMenuWidget, Warning, TEXT("ReplaySelectWidgetClass is not set."));
		return;
	}

	UUserWidget* ReplayWidget = CreateWidget<UUserWidget>(PC, ReplaySelectWidgetClass);
	if (!ReplayWidget)
	{
		UE_LOG(LogMainMenuWidget, Warning, TEXT("Failed to create replay select widget."));
		return;
	}

	RemoveFromParent();
	ReplayWidget->AddToViewport(100);
	SetMenuInputMode(PC, ReplayWidget);
}

void UMainMenuWidget::HandleExitClicked()
{
	UKismetSystemLibrary::QuitGame(
		this,
		GetOwningPlayer(),
		EQuitPreference::Quit,
		true
	);
}
