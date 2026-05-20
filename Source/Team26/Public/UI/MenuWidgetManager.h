#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MenuWidgetManager.generated.h"

class UUserWidget;

UCLASS()
class TEAM26_API AMenuWidgetManager : public AActor
{
	GENERATED_BODY()

public:
	AMenuWidgetManager();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Menu")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentWidget = nullptr;
};