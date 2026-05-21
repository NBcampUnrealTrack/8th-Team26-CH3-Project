#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Replay/ReplayType.h"
#include "ReplaySelectWidget.generated.h"

class UButton;
class UListView;
class UTextBlock;
class UUserWidget;

UCLASS()
class TEAM26_API UReplaySelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void RefreshReplayList();
	void UpdatePreviewPanel();
	void HandleReplayItemClicked(UObject* ClickedItem);
	FString BuildReplayOpenLevelOptions() const;

	UFUNCTION()
	void HandlePlayClicked();

	UFUNCTION()
	void HandleDeleteClicked();

	UFUNCTION()
	void HandleBackClicked();

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> ReplayListView = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> PlayButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> DeleteButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BackButton = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PreviewTitleText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PreviewDurationText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PreviewDateText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> PreviewPathText = nullptr;

	UPROPERTY(EditAnywhere, Category="Replay|Open Level")
	FName ReplayPlaybackLevelName = TEXT("VehicleAdvExampleMap");

	UPROPERTY(EditAnywhere, Category="Replay|Open Level")
	bool bOpenGameplayMapAsReplayMode = true;

	UPROPERTY(EditAnywhere, Category="Replay|Open Level")
	FString ReplayOpenLevelOptions = TEXT("ReplayMode=1?game=/Game/VehicleTemplate/Blueprints/Replay/BP_GM_Replay.BP_GM_Replay_C");

	UPROPERTY(EditAnywhere, Category="Replay")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	FReplayFileInfo SelectedReplay;
	bool bHasSelection = false;
	bool bHasReplayFiles = false;
};
