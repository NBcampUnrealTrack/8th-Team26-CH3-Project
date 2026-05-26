// Copyright NBC, Inc. All Rights Reserved.

#include "Component/SplineFollowerACCComponent.h"
#include "Team26/Team26.h"
#include "Team26/Team26Pawn.h"
#include "Sensor/LidarSensorComponent.h"

USplineFollowerACCComponent::USplineFollowerACCComponent()
{
}

float USplineFollowerACCComponent::GetExtraSpeedCap() const
{
	const float Dist = GetForwardObstacleDistance();

	// 추종 거리 밖이면 영향 없음
	if (Dist >= FollowGap)
	{
		return TNumericLimits<float>::Max();
	}

	// 비상 정지 거리 안이면 즉시 0
	if (Dist <= CriticalGap)
	{
		return 0.f;
	}

	// DesiredGap ~ FollowGap 사이를 0 ~ 1 로 정규화하여 부모 MaxSpeed 에 비례 적용
	const float Range = FMath::Max(FollowGap - DesiredGap, 1.f);
	const float Scale = FMath::Clamp((Dist - DesiredGap) / Range, 0.f, 1.f);
	return GetMaxSpeed() * Scale;
}

float USplineFollowerACCComponent::GetForwardObstacleDistance() const
{
	if (!OwnerPawn.IsValid())
	{
		return TNumericLimits<float>::Max();
	}

	ULidarSensorComponent* Lidar = OwnerPawn->FindComponentByClass<ULidarSensorComponent>();
	if (!Lidar)
	{
		return TNumericLimits<float>::Max();
	}

	return Lidar->GetClosestForwardDistance();
}
