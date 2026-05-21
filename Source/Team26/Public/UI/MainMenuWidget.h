#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;
class UUserWidget;

UCLASS()
class TEAM26_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void HandleStartGameClicked();

	UFUNCTION()
	void HandleReplayClicked();

	UFUNCTION()
	void HandleExitClicked();

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> StartGameButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ReplayButton = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ExitButton = nullptr;

	UPROPERTY(EditAnywhere, Category="Menu")
	FName GameLevelName = TEXT("L_Track");

	UPROPERTY(EditAnywhere, Category="Menu")
	TSubclassOf<UUserWidget> ReplaySelectWidgetClass;
};