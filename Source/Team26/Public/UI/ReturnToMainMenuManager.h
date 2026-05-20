#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReturnToMainMenuManager.generated.h"

UCLASS()
class TEAM26_API AReturnToMainMenuManager : public AActor
{
	GENERATED_BODY()

public:
	AReturnToMainMenuManager();

protected:
	virtual void BeginPlay() override;

private:
	void HandleReturnKeyPressed();

private:
	UPROPERTY(EditAnywhere, Category="Return To Menu")
	FName MainMenuLevelName = TEXT("L_Menu");

	UPROPERTY(EditAnywhere, Category="Return To Menu")
	bool bHideMouseCursorWhenActive = true;

	UPROPERTY(EditAnywhere, Category="Return To Menu")
	bool bShowDebugMessage = false;

	bool bIsReturningToMenu = false;
};
