#include "UI/MenuWidgetManager.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogMenuWidgetManager, Log, All);

namespace
{
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

AMenuWidgetManager::AMenuWidgetManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMenuWidgetManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		UE_LOG(LogMenuWidgetManager, Warning, TEXT("PlayerController not found."));
		return;
	}

	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogMenuWidgetManager, Warning, TEXT("MainMenuWidgetClass is not set."));
		return;
	}

	CurrentWidget = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
	if (!CurrentWidget)
	{
		UE_LOG(LogMenuWidgetManager, Warning, TEXT("Failed to create main menu widget."));
		return;
	}

	CurrentWidget->AddToViewport(100);
	SetMenuInputMode(PC, CurrentWidget);
}
