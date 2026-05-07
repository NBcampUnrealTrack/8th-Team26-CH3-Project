// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team26GameMode.h"
#include "Team26PlayerController.h"

ATeam26GameMode::ATeam26GameMode()
{
	PlayerControllerClass = ATeam26PlayerController::StaticClass();
}
