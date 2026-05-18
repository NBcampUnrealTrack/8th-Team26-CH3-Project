// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VehicleSoundComponent.generated.h"

class UAudioComponent;
class USoundBase;
class UChaosWheeledVehicleMovementComponent;
class UPrimitiveComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM26_API UVehicleSoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVehicleSoundComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	void UpdateVehicleSound(float DeltaTime);
	void ChangeEngineSound(USoundBase* NewSound);
	USoundBase* GetAccelSoundByGear(int32 CurrentGear) const;
	bool AreAllWheelsOnGround() const;
	
	UFUNCTION()
	void OnVehicleHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

private:
	UPROPERTY()
	TObjectPtr<UChaosWheeledVehicleMovementComponent> VehicleMovement;

	UPROPERTY()
	TObjectPtr<UAudioComponent> EngineAudioComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BrakeAudioComponent;
	
	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> OwnerPrimitiveComponent;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Engine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> IdleSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Engine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> AccelGear1Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Engine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> AccelGear2Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Engine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> AccelGear3Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Engine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> AccelGear4PlusSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Engine", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> ThrottleOffSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Brake", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> BrakeSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> LightImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundBase> HeavyImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	float MinImpactSpeedKmh = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	float HeavyImpactSpeedKmh = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	float ImpactCooldown = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	float LightImpactVolume = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Impact", meta=(AllowPrivateAccess="true"))
	float HeavyImpactVolume = 0.9f;

	float LastImpactSoundTime = -999.0f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Threshold", meta=(AllowPrivateAccess="true"))
	float ThrottleThreshold = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Threshold", meta=(AllowPrivateAccess="true"))
	float BrakeThreshold = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Threshold", meta=(AllowPrivateAccess="true"))
	float IdleSpeedKmh = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Threshold", meta=(AllowPrivateAccess="true"))
	float BrakeSoundMinSpeedKmh = 25.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Threshold", meta=(AllowPrivateAccess="true"))
	float AccelSoundMinSpeedKmh = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Volume", meta=(AllowPrivateAccess="true"))
	float BrakeMinVolume = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Volume", meta=(AllowPrivateAccess="true"))
	float EngineVolumeWhenBraking = 0.25f;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Pitch", meta=(AllowPrivateAccess="true"))
	float MinRPM = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Pitch", meta=(AllowPrivateAccess="true"))
	float MaxRPM = 8000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Pitch", meta=(AllowPrivateAccess="true"))
	float MinPitch = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Pitch", meta=(AllowPrivateAccess="true"))
	float MaxPitch = 1.12f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Volume", meta=(AllowPrivateAccess="true"))
	float IdleVolume = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Volume", meta=(AllowPrivateAccess="true"))
	float AccelVolume = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Volume", meta=(AllowPrivateAccess="true"))
	float ThrottleOffVolume = 0.65f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Volume", meta=(AllowPrivateAccess="true"))
	float BrakeVolume = 0.45f;
	
	float CurrentBrakeVolume = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Brake", meta=(AllowPrivateAccess="true"))
	float BrakeSoundReleaseDelay = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Brake", meta=(AllowPrivateAccess="true"))
	float BrakeFadeInTime = 0.02f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Vehicle Sound|Brake", meta=(AllowPrivateAccess="true"))
	float BrakeFadeOutTime = 0.4f;

	float LastBrakeSoundRequestTime = -999.0f;
	float LastBrakeInputTime = -999.0f;

	bool bBrakeSoundActive = false;
	
private:
	UPROPERTY()
	TObjectPtr<USoundBase> CurrentEngineSound;
};
