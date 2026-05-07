// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team26WheelRear.h"
#include "UObject/ConstructorHelpers.h"

UTeam26WheelRear::UTeam26WheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}