// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Team26Pawn.h"
#include "Team26SportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class TEAM26_API ATeam26SportsCar : public ATeam26Pawn
{
	GENERATED_BODY()
	
public:

	ATeam26SportsCar();
};
