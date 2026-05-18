#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VehicleGeographicHudManager.generated.h"

class AGeoReferencingSystem;
class UVehicleGeographicHudWidget;

UENUM(BlueprintType)
enum class EVehicleGeographicTargetResolveMode : uint8
{
	ExplicitActor UMETA(DisplayName = "Explicit Actor"),
	PlayerPawn UMETA(DisplayName = "Player Pawn"),
	ActorClass UMETA(DisplayName = "Actor Class"),
	ActorTag UMETA(DisplayName = "Actor Tag")
};

UCLASS()
class TEAM26_API AVehicleGeographicHudManager : public AActor
{
	GENERATED_BODY()

public:
	AVehicleGeographicHudManager();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Geographic HUD")
	bool StartHud();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Geographic HUD")
	void StopHud();

	UFUNCTION(BlueprintCallable, Category = "Vehicle Geographic HUD")
	void SetTargetVehicle(AActor* NewTargetVehicle, bool bStartHudNow = true);

	UFUNCTION(BlueprintCallable, Category = "Vehicle Geographic HUD")
	void UpdateHudText();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool ResolveTargetVehicle();
	bool ResolveGeoReferencingSystem();

	bool CreateHudWidget();
	void DestroyHudWidget();
	void SetHudText(const FText& NewText) const;

	void StartUpdateTimer();
	void StopUpdateTimer();

	void StartResolveRetry();
	void StopResolveRetry();
	void HandleResolveRetry();

	FText BuildGeographicText() const;
	UFUNCTION()
	void DelayedStartHud();

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD", meta = (AllowPrivateAccess = "true"))
	bool bStartAutomatically = true;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (AllowPrivateAccess = "true"))
	EVehicleGeographicTargetResolveMode TargetResolveMode = EVehicleGeographicTargetResolveMode::PlayerPawn;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> TargetVehicle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> TargetVehicleClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (AllowPrivateAccess = "true"))
	FName TargetVehicleTag = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Widget", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UVehicleGeographicHudWidget> HudWidgetClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Widget", meta = (AllowPrivateAccess = "true"))
	int32 HudZOrder = 9999;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Update", meta = (ClampMin = "1.0", ClampMax = "60.0", Units = "Hz", AllowPrivateAccess = "true"))
	float UpdateRateHz = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (AllowPrivateAccess = "true"))
	bool bRetryUntilTargetFound = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (ClampMin = "0.05", ClampMax = "5.0", Units = "s", AllowPrivateAccess = "true"))
	float ResolveRetryIntervalSeconds = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Target", meta = (AllowPrivateAccess = "true"))
	int32 MaxResolveAttempts = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Display", meta = (AllowPrivateAccess = "true"))
	bool bUseLabelFormat = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Display", meta = (AllowPrivateAccess = "true"))
	int32 CoordinateDecimalPlaces = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Display", meta = (AllowPrivateAccess = "true"))
	int32 AltitudeDecimalPlaces = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD|Debug", meta = (AllowPrivateAccess = "true"))
	bool bLogDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Geographic HUD", meta = (ClampMin = "0.0", ClampMax = "5.0", Units = "s", AllowPrivateAccess = "true"))
	float InitialStartDelaySeconds = 0.1f;

	UPROPERTY(Transient)
	TObjectPtr<UVehicleGeographicHudWidget> HudWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<AGeoReferencingSystem> GeoReferencingSystem = nullptr;

	FTimerHandle UpdateTimerHandle;
	FTimerHandle ResolveRetryTimerHandle;
	FTimerHandle InitialStartTimerHandle;

	int32 ResolveAttemptCount = 0;
	bool bIsHudRunning = false;
};
