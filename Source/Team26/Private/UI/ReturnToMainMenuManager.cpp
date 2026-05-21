#include "UI/ReturnToMainMenuManager.h"

#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogReturnToMainMenuManager, Log, All);

AReturnToMainMenuManager::AReturnToMainMenuManager()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoReceiveInput = EAutoReceiveInput::Player0;
}

void AReturnToMainMenuManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (!PC)
	{
		UE_LOG(LogReturnToMainMenuManager, Warning, TEXT("PlayerController not found."));
		return;
	}

	EnableInput(PC);

	if (!InputComponent)
	{
		UE_LOG(LogReturnToMainMenuManager, Warning, TEXT("InputComponent not found."));
		return;
	}

	InputComponent->Priority = 100;

	FInputKeyBinding& ReturnKeyBinding = InputComponent->BindKey(
		EKeys::R,
		IE_Pressed,
		this,
		&AReturnToMainMenuManager::HandleReturnKeyPressed
	);

	ReturnKeyBinding.bConsumeInput = true;
	ReturnKeyBinding.bExecuteWhenPaused = true;

	FInputModeGameOnly InputMode;
	PC->SetInputMode(InputMode);

	if (bHideMouseCursorWhenActive)
	{
		PC->bShowMouseCursor = false;
	}

	if (bShowDebugMessage)
	{
		UE_LOG(LogReturnToMainMenuManager, Log, TEXT("Press R to return to main menu."));
	}
}

void AReturnToMainMenuManager::HandleReturnKeyPressed()
{
	if (bIsReturningToMenu)
	{
		return;
	}

	bIsReturningToMenu = true;

	if (bShowDebugMessage)
	{
		UE_LOG(LogReturnToMainMenuManager, Log, TEXT("Returning to main menu."));
	}

	UGameplayStatics::OpenLevel(this, MainMenuLevelName);
}
