// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team26WheelFront.h"
#include "UObject/ConstructorHelpers.h"

UTeam26WheelFront::UTeam26WheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}