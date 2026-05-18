// Copyright Epic Games, Inc. All Rights Reserved.


#include "Team26SportsCar.h"
#include "Team26SportsWheelFront.h"
#include "Team26SportsWheelRear.h"
#include "ChaosWheeledVehicleMovementComponent.h"

ATeam26SportsCar::ATeam26SportsCar()
{
	// Note: for faster iteration times, the vehicle setup can be tweaked in the Blueprint instead

	// Set up the chassis
	GetChaosVehicleMovement()->ChassisHeight = 144.0f;
	GetChaosVehicleMovement()->DragCoefficient = 0.31f;

	// Set up the wheels
	GetChaosVehicleMovement()->bLegacyWheelFrictionPosition = true;
	GetChaosVehicleMovement()->WheelSetups.SetNum(4);

	GetChaosVehicleMovement()->WheelSetups[0].WheelClass = UTeam26SportsWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[0].BoneName = FName("Phys_Wheel_FL");
	GetChaosVehicleMovement()->WheelSetups[0].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[1].WheelClass = UTeam26SportsWheelFront::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[1].BoneName = FName("Phys_Wheel_FR");
	GetChaosVehicleMovement()->WheelSetups[1].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[2].WheelClass = UTeam26SportsWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[2].BoneName = FName("Phys_Wheel_BL");
	GetChaosVehicleMovement()->WheelSetups[2].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	GetChaosVehicleMovement()->WheelSetups[3].WheelClass = UTeam26SportsWheelRear::StaticClass();
	GetChaosVehicleMovement()->WheelSetups[3].BoneName = FName("Phys_Wheel_BR");
	GetChaosVehicleMovement()->WheelSetups[3].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f);

	// Set up the engine
	// NOTE: Check the Blueprint asset for the Torque Curve
	GetChaosVehicleMovement()->EngineSetup.MaxTorque = 750.0f;
	GetChaosVehicleMovement()->EngineSetup.MaxRPM = 7000.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineIdleRPM = 900.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineBrakeEffect = 0.2f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevUpMOI = 5.0f;
	GetChaosVehicleMovement()->EngineSetup.EngineRevDownRate = 600.0f;

	// Set up the transmission
	GetChaosVehicleMovement()->TransmissionSetup.bUseAutomaticGears = true;
	GetChaosVehicleMovement()->TransmissionSetup.bUseAutoReverse = true;
	GetChaosVehicleMovement()->TransmissionSetup.FinalRatio = 2.81f;
	GetChaosVehicleMovement()->TransmissionSetup.ChangeUpRPM = 6000.0f;
	GetChaosVehicleMovement()->TransmissionSetup.ChangeDownRPM = 2000.0f;
	GetChaosVehicleMovement()->TransmissionSetup.GearChangeTime = 0.2f;
	GetChaosVehicleMovement()->TransmissionSetup.TransmissionEfficiency = 0.9f;

//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios.SetNum(6);
//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[0] = 3.80f;
//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[1] = 2.60f;
//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[2] = 1.90f;
//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[3] = 1.45f;
//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[4] = 1.15f;
//	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[5] = 0.92f;

	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios.SetNum(6);
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[0] = 4.00f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[1] = 2.75f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[2] = 2.00f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[3] = 1.50f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[4] = 1.15f;
	GetChaosVehicleMovement()->TransmissionSetup.ForwardGearRatios[5] = 0.90f;

	GetChaosVehicleMovement()->TransmissionSetup.ReverseGearRatios.SetNum(1);
	GetChaosVehicleMovement()->TransmissionSetup.ReverseGearRatios[0] = 4.04f;

	// Set up the steering
	// NOTE: Check the Blueprint asset for the Steering Curve
	GetChaosVehicleMovement()->SteeringSetup.SteeringType = ESteeringType::Ackermann;
	GetChaosVehicleMovement()->SteeringSetup.AngleRatio = 0.7f;
}