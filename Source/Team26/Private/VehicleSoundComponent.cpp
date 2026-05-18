// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleSoundComponent.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"


UVehicleSoundComponent::UVehicleSoundComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVehicleSoundComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	VehicleMovement = Owner->FindComponentByClass<UChaosWheeledVehicleMovementComponent>();
	
	OwnerPrimitiveComponent = Cast<UPrimitiveComponent>(Owner->GetRootComponent());

	if (!OwnerPrimitiveComponent)
	{
		OwnerPrimitiveComponent = Owner->FindComponentByClass<UPrimitiveComponent>();
	}

	if (OwnerPrimitiveComponent)
	{
		OwnerPrimitiveComponent->SetNotifyRigidBodyCollision(true);

		OwnerPrimitiveComponent->OnComponentHit.AddDynamic(
			this,
			&UVehicleSoundComponent::OnVehicleHit
		);
	}

	EngineAudioComponent = NewObject<UAudioComponent>(Owner, TEXT("EngineAudioComponent"));
	if (EngineAudioComponent)
	{
		EngineAudioComponent->bAutoActivate = false;
		EngineAudioComponent->RegisterComponent();

		if (Owner->GetRootComponent())
		{
			EngineAudioComponent->AttachToComponent(
				Owner->GetRootComponent(),
				FAttachmentTransformRules::KeepRelativeTransform
			);
		}
	}

	BrakeAudioComponent = NewObject<UAudioComponent>(Owner, TEXT("BrakeAudioComponent"));
	if (BrakeAudioComponent)
	{
		BrakeAudioComponent->bAutoActivate = false;
		BrakeAudioComponent->RegisterComponent();

		if (Owner->GetRootComponent())
		{
			BrakeAudioComponent->AttachToComponent(
				Owner->GetRootComponent(),
				FAttachmentTransformRules::KeepRelativeTransform
			);
		}
	}

	ChangeEngineSound(IdleSound);
}

void UVehicleSoundComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateVehicleSound(DeltaTime);
}

void UVehicleSoundComponent::UpdateVehicleSound(float DeltaTime)
{
	if (!VehicleMovement || !EngineAudioComponent)
	{
		return;
	}

	const float ForwardSpeedKmh = VehicleMovement->GetForwardSpeed() * 0.036f;
	const float SpeedKmh = FMath::Abs(ForwardSpeedKmh);
	const float ThrottleInput = VehicleMovement->GetThrottleInput();
	const float BrakeInput = VehicleMovement->GetBrakeInput();
	const float HandbrakeInput = VehicleMovement->GetHandbrakeInput();
	const int32 CurrentGear = VehicleMovement->GetCurrentGear();
	const float CurrentRPM = VehicleMovement->GetEngineRotationSpeed();

	const bool bHasThrottleInput = ThrottleInput > ThrottleThreshold;
	const bool bHasBrakeInput =
	BrakeInput > BrakeThreshold ||
	HandbrakeInput > BrakeThreshold;
	const bool bIsMoving = SpeedKmh > IdleSpeedKmh;

	// 엑셀을 밟고 있어도 실제 속도가 거의 없으면 가속음 재생하지 않음.
	// 충돌로 막혀 있는데 엑셀음이 계속 나는 문제 방지.
	const bool bAllWheelsOnGround = AreAllWheelsOnGround();

	const bool bCanPlayAccelSound =
		bHasThrottleInput &&
		CurrentGear > 0 &&
		SpeedKmh > AccelSoundMinSpeedKmh &&
		bAllWheelsOnGround;

	// 브레이크 소리는 일정 속도 이상에서만 재생.
	const FVector OwnerVelocity = GetOwner() ? GetOwner()->GetVelocity() : FVector::ZeroVector;
	const FVector OwnerForward = GetOwner() ? GetOwner()->GetActorForwardVector() : FVector::ForwardVector;

	const float ForwardMoveDot = FVector::DotProduct(
		OwnerVelocity.GetSafeNormal(),
		OwnerForward
	);

	const bool bIsActuallyMovingForward = ForwardMoveDot > 0.2f;

	const bool bCanPlayBrakeSound =
		bHasBrakeInput &&
		SpeedKmh > BrakeSoundMinSpeedKmh &&
		bIsActuallyMovingForward &&
		bAllWheelsOnGround;
	
	const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	// 엑셀음 차단용.
	// 브레이크 효과음 조건보다 널널하게 잡아야 함.
	// 그래야 자율주행이 브레이크를 톡톡 눌러도 엑셀음으로 바로 안 돌아감.
	const bool bBrakeInputForEngineLock =
		bHasBrakeInput &&
		bIsActuallyMovingForward &&
		bAllWheelsOnGround;

	if (bBrakeInputForEngineLock)
	{
		LastBrakeInputTime = Now;
	}

	const bool bRecentlyBraking =
		GetWorld() &&
		Now - LastBrakeInputTime < BrakeSoundReleaseDelay;

	USoundBase* TargetEngineSound = IdleSound;
	float TargetEngineVolume = IdleVolume;

	if (bRecentlyBraking)
	{
		// 브레이크가 방금 들어왔으면,
		// 잠깐 브레이크가 풀려도 바로 엑셀음으로 돌아가지 않게 막음.
		TargetEngineSound = ThrottleOffSound ? ThrottleOffSound : IdleSound;
		TargetEngineVolume = EngineVolumeWhenBraking;
	}
	else if (bCanPlayAccelSound)
	{
		TargetEngineSound = GetAccelSoundByGear(CurrentGear);
		TargetEngineVolume = AccelVolume;
	}
	else if (bIsMoving)
	{
		// 움직이고 있는데 엑셀을 안 밟는 상태
		TargetEngineSound = ThrottleOffSound ? ThrottleOffSound : IdleSound;
		TargetEngineVolume = ThrottleOffVolume;
	}
	else
	{
		// 정지 또는 거의 정지 상태
		TargetEngineSound = IdleSound;
		TargetEngineVolume = IdleVolume;
	}

	ChangeEngineSound(TargetEngineSound);

	const float NormalizedRPM = FMath::GetMappedRangeValueClamped(
		FVector2D(MinRPM, MaxRPM),
		FVector2D(0.0f, 1.0f),
		CurrentRPM
	);

	const float TargetPitch = FMath::Lerp(MinPitch, MaxPitch, NormalizedRPM);

	EngineAudioComponent->SetPitchMultiplier(TargetPitch);
	EngineAudioComponent->SetVolumeMultiplier(TargetEngineVolume);

	if (BrakeAudioComponent && BrakeSound && GetWorld())
	{

		if (bCanPlayBrakeSound)
		{
			LastBrakeSoundRequestTime = Now;
		}

		const bool bShouldKeepBrakeSound =
			Now - LastBrakeSoundRequestTime < BrakeSoundReleaseDelay;

		float TargetBrakeVolume = 0.0f;

		if (bShouldKeepBrakeSound)
		{
			TargetBrakeVolume = FMath::GetMappedRangeValueClamped(
				FVector2D(BrakeSoundMinSpeedKmh, 120.0f),
				FVector2D(BrakeMinVolume, BrakeVolume),
				SpeedKmh
			);
		}

		CurrentBrakeVolume = FMath::FInterpTo(
			CurrentBrakeVolume,
			TargetBrakeVolume,
			DeltaTime,
			8.0f
		);

		// 브레이크가 필요한 순간에만 처음 재생 시작
		if (bShouldKeepBrakeSound && !BrakeAudioComponent->IsPlaying())
		{
			BrakeAudioComponent->SetSound(BrakeSound);
			BrakeAudioComponent->SetVolumeMultiplier(0.0f);
			BrakeAudioComponent->Play();
		}

		// 재생 중일 때만 볼륨 조절
		if (BrakeAudioComponent->IsPlaying())
		{
			BrakeAudioComponent->SetVolumeMultiplier(CurrentBrakeVolume);
			BrakeAudioComponent->SetPitchMultiplier(1.0f);
		}

		// 브레이크가 완전히 끝났고 볼륨도 거의 0이면 정지
		if (!bShouldKeepBrakeSound &&
			BrakeAudioComponent->IsPlaying() &&
			CurrentBrakeVolume <= 0.01f)
		{
			BrakeAudioComponent->Stop();
		}
	}
}

void UVehicleSoundComponent::ChangeEngineSound(USoundBase* NewSound)
{
	if (!EngineAudioComponent || !NewSound)
	{
		return;
	}

	if (CurrentEngineSound == NewSound && EngineAudioComponent->IsPlaying())
	{
		return;
	}

	CurrentEngineSound = NewSound;

	EngineAudioComponent->Stop();
	EngineAudioComponent->SetSound(NewSound);
	EngineAudioComponent->Play();
}

USoundBase* UVehicleSoundComponent::GetAccelSoundByGear(int32 CurrentGear) const
{
	if (CurrentGear <= 1)
	{
		return AccelGear1Sound ? AccelGear1Sound : IdleSound;
	}

	if (CurrentGear == 2)
	{
		return AccelGear2Sound ? AccelGear2Sound : AccelGear1Sound;
	}

	if (CurrentGear == 3)
	{
		return AccelGear3Sound ? AccelGear3Sound : AccelGear2Sound;
	}

	return AccelGear4PlusSound ? AccelGear4PlusSound : AccelGear3Sound;
}

bool UVehicleSoundComponent::AreAllWheelsOnGround() const
{
	if (!VehicleMovement)
	{
		return false;
	}

	const int32 NumWheels = VehicleMovement->GetNumWheels();

	if (NumWheels <= 0)
	{
		return false;
	}

	int32 WheelsOffGround = 0;

	for (int32 WheelIndex = 0; WheelIndex < NumWheels; ++WheelIndex)
	{
		const FWheelStatus WheelStatus = VehicleMovement->GetWheelState(WheelIndex);

		if (!WheelStatus.bInContact)
		{
			++WheelsOffGround;
		}
	}

	// 바퀴가 3개 이상 떠 있으면 주행 관련 소리 차단
	return WheelsOffGround < 3;
}

void UVehicleSoundComponent::OnVehicleHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit
)
{
	if (!GetWorld() || !OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	// 바닥 충돌음 방지.
	// 바닥은 보통 ImpactNormal.Z가 높게 나옴.
	// 벽/장애물은 보통 Z가 낮음.
	if (Hit.ImpactNormal.Z > 0.65f)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();

	if (Now - LastImpactSoundTime < ImpactCooldown)
	{
		return;
	}

	float ImpactSpeedKmh = 0.0f;

	if (GetOwner())
	{
		ImpactSpeedKmh = GetOwner()->GetVelocity().Size() * 0.036f;
	}
	else if (VehicleMovement)
	{
		ImpactSpeedKmh = FMath::Abs(VehicleMovement->GetForwardSpeed()) * 0.036f;
	}

	if (ImpactSpeedKmh < MinImpactSpeedKmh)
	{
		return;
	}

	const bool bHeavyImpact = ImpactSpeedKmh >= HeavyImpactSpeedKmh;

	USoundBase* ImpactSound = bHeavyImpact ? HeavyImpactSound : LightImpactSound;

	if (!ImpactSound)
	{
		ImpactSound = HeavyImpactSound ? HeavyImpactSound : LightImpactSound;
	}

	if (!ImpactSound)
	{
		return;
	}

	const float ImpactVolume = bHeavyImpact ? HeavyImpactVolume : LightImpactVolume;
	const float ImpactPitch = bHeavyImpact ? 0.95f : 1.05f;

	UGameplayStatics::PlaySoundAtLocation(
		this,
		ImpactSound,
		Hit.ImpactPoint,
		ImpactVolume,
		ImpactPitch
	);

	LastImpactSoundTime = Now;
}