#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VehicleGeographicHudWidget.generated.h"

class UTextBlock;

UCLASS()
class TEAM26_API UVehicleGeographicHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Vehicle Geographic HUD")
	void SetGeographicText(const FText& NewText);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LocationText = nullptr;
};
