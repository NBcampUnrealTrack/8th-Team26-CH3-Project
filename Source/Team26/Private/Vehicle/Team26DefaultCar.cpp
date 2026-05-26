// Copyright NBC, Inc. All Rights Reserved.

#include "Vehicle/Team26DefaultCar.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Engine/DataTable.h"

ATeam26DefaultCar::ATeam26DefaultCar()
{
	// 휠 슬롯 4개 예약 — BoneName, WheelClass 는 BP 디테일 패널에서 채움
	GetChaosVehicleMovement()->WheelSetups.SetNum(4);
	GetChaosVehicleMovement()->bLegacyWheelFrictionPosition = true;

	// 공통 섀시 — BP 에서 override 가능
	GetChaosVehicleMovement()->ChassisHeight = 144.0f;
	GetChaosVehicleMovement()->DragCoefficient = 0.31f;

	// 공통 엔진 디폴트 (가속 ↑ : MaxTorque, 최고속 ↑ : MaxRPM)
	GetChaosVehicleMovement()->EngineSetup.MaxTorque = 700.f;
	GetChaosVehicleMovement()->EngineSetup.MaxRPM = 6000.f;
	GetChaosVehicleMovement()->EngineSetup.EngineIdleRPM = 900.f;
	GetChaosVehicleMovement()->EngineSetup.EngineBrakeEffect = 0.2f;

	// 공통 변속 디폴트 (최고속 ↑ : FinalRatio ↓ 또는 ForwardGearRatios 마지막 단 ↓)
	GetChaosVehicleMovement()->TransmissionSetup.bUseAutomaticGears = true;
	GetChaosVehicleMovement()->TransmissionSetup.bUseAutoReverse = true;
	GetChaosVehicleMovement()->TransmissionSetup.FinalRatio = 2.81f;
	GetChaosVehicleMovement()->TransmissionSetup.ChangeUpRPM = 5500.f;
	GetChaosVehicleMovement()->TransmissionSetup.ChangeDownRPM = 2000.f;
	GetChaosVehicleMovement()->TransmissionSetup.GearChangeTime = 0.2f;
	GetChaosVehicleMovement()->TransmissionSetup.TransmissionEfficiency = 0.9f;

	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios.SetNum(5);
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[0] = 4.25f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[1] = 2.52f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[2] = 1.66f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[3] = 1.22f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[4] = 1.0f;

	GetChaosVehicleMovement()->TransmissionSetup.ReverseGearRatios.SetNum(1);
	GetChaosVehicleMovement()->TransmissionSetup.ReverseGearRatios[0] = 4.04f;

	// 공통 조향 디폴트 (회전 반경 ↑ : AngleRatio ↓)
	GetChaosVehicleMovement()->SteeringSetup.SteeringType = ESteeringType::Ackermann;
	GetChaosVehicleMovement()->SteeringSetup.AngleRatio = 0.7f;
}

void ATeam26DefaultCar::BeginPlay()
{
	Super::BeginPlay();

	// ConfigTable 지정되어 있으면 행 읽어 적용
	if (ConfigTable && !ConfigRowName.IsNone())
	{
		const FString Context = FString::Printf(TEXT("ATeam26DefaultCar::BeginPlay (%s)"), *GetName());
		if (FTeam26VehicleConfigRow* Row = ConfigTable->FindRow<FTeam26VehicleConfigRow>(ConfigRowName, Context))
		{
			ApplyConfigRow(*Row);
			UE_LOG(LogTemplateVehicle, Log,
				TEXT("[%s] Vehicle config applied from DataTable row '%s'"),
				*GetName(), *ConfigRowName.ToString());
		}
		else
		{
			UE_LOG(LogTemplateVehicle, Warning,
				TEXT("[%s] Vehicle config row '%s' not found in DataTable"),
				*GetName(), *ConfigRowName.ToString());
		}
	}
}

void ATeam26DefaultCar::ApplyConfigRow(const FTeam26VehicleConfigRow& Row)
{
	UChaosWheeledVehicleMovementComponent* Move = GetChaosVehicleMovement();
	if (!Move) return;

	// 엔진
	Move->EngineSetup.MaxTorque         = Row.MaxTorque;
	Move->EngineSetup.MaxRPM            = Row.MaxRPM;
	Move->EngineSetup.EngineIdleRPM     = Row.EngineIdleRPM;
	Move->EngineSetup.EngineBrakeEffect = Row.EngineBrakeEffect;

	// 변속
	Move->TransmissionSetup.FinalRatio     = Row.FinalRatio;
	Move->TransmissionSetup.ChangeUpRPM    = Row.ChangeUpRPM;
	Move->TransmissionSetup.ChangeDownRPM  = Row.ChangeDownRPM;
	Move->TransmissionSetup.GearChangeTime = Row.GearChangeTime;

	if (Row.ForwardGearRatios.Num() > 0)
	{
		Move->TransmissionSetup.ForwardGearRatios = Row.ForwardGearRatios;
	}

	// 조향
	Move->SteeringSetup.AngleRatio = Row.SteeringAngleRatio;

	// 런타임 변경 사항 반영 — 일부 셋업은 PhysicsState 재생성 필요
	Move->RecreatePhysicsState();
}
