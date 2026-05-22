// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Component/SplineFollowerComponent.h"
#include "Sensor/LidarSensorComponent.h"
#include "LidarSplineFollower.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAM26_API ULidarSplineFollower : public USplineFollowerComponent
{
	GENERATED_BODY()

protected:
	// [추가] [강민서] 장애물 회피 조향
	virtual float ComputeExtraSteer() const override;

private:
	// [추가] [강민서] 회피 시작 거리 (기본 1000cm = 10m)
	UPROPERTY(EditAnywhere, Category = "LidarAvoid")
	float AvoidDistance = 1000.f;

	// [추가] [강민서] 회피 조향 강도 (기본 0.5)
	UPROPERTY(EditAnywhere, Category = "LidarAvoid")
	float AvoidSteerStrength = 0.5f;
};
