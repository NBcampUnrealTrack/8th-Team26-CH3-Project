// Fill out your copyright notice in the Description page of Project Settings.
#include "LidarSplineFollower.h"
#include "Team26/Team26Pawn.h"
#include "Sensor/LidarSensorComponent.h"

float ULidarSplineFollower::ComputeExtraSteer() const
{
	if (!OwnerPawn.IsValid()) return 0.f;

	ULidarSensorComponent* Lidar = OwnerPawn->FindComponentByClass<ULidarSensorComponent>();
	if (!Lidar) return 0.f;

	const float DistFront = Lidar->GetClosestForwardDistance();
	const float DistLeft = Lidar->GetClosestForwardLeft();  
	const float DistRight = Lidar->GetClosestForwardRight(); 

	// 장애물이 충분히 멀면 회피 안함
	if (DistFront > AvoidDistance) return 0.f;

	// 왼쪽이 더 가까우면 오른쪽으로, 오른쪽이 더 가까우면 왼쪽으로
	if (DistLeft < DistRight)
		return AvoidSteerStrength;   // 오른쪽
	else
		return -AvoidSteerStrength;  // 왼쪽
}

