// Copyright NBC, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/SplineFollowerComponent.h"
#include "SplineFollowerAvoidanceComponent.generated.h"

UENUM(BlueprintType)
enum class ESplineFollowerState : uint8
{
	Following   UMETA(DisplayName = "Following"),    // 평소 주행
	Avoiding    UMETA(DisplayName = "Avoiding"),     // 회피 중
	Stopping    UMETA(DisplayName = "Stopping"),     // 정지
	Restarting  UMETA(DisplayName = "Restarting")    // 옆으로 다시 출발
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM26_API USplineFollowerAvoidanceComponent : public USplineFollowerComponent
{
	GENERATED_BODY()

public:
	USplineFollowerAvoidanceComponent();

protected:
	// 라이다 상태머신 처리. Stopping/Restarting 이면 true 반환해 부모의 평소 주행 로직 차단.
	virtual bool HandleStateOverride() override;

	// 기본 스티어에 회피 보정값을 더하기 위한 hook.
	virtual float ComputeExtraSteer() const override;

private:
	// 라이다 기반 회피 스티어값 계산 (-1~+1, 평상시 0)
	float ComputeAvoidanceSteer() const;

private:
	//-------------------------------------
	//------------ 회피 ----------------
	//-------------------------------------

	// 이 거리 안에 들어오면 회피 시작 (cm). 기본 15m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(AllowPrivateAccess="true"))
	float AvoidanceStartDist = 1500.f;

	// 회피 핸들 최대 강도 (0~1). 기본 0.7 = 핸들의 70% 까지 보정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(ClampMin="0.0", ClampMax="1.0", AllowPrivateAccess="true"))
	float AvoidanceStrength = 0.7f;

	// 좌/우 거리 차이가 이 값(cm) 이상이어야 한쪽으로 결정. 지터 방지용. 기본 1m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(AllowPrivateAccess="true"))
	float AvoidanceHysteresis = 100.f;

	// 정면 거리가 이 값(cm) 미만이면 풀브레이크. 기본 5m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(AllowPrivateAccess="true"))
	float CriticalStopDist = 500.f;

	// 멈춤 판단 속도 (cm/s). 이 밑이면 정지 상태로 봄
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(AllowPrivateAccess="true"))
	float StoppedSpeedThreshold = 100.f;

	// Restart 시 핸들 강도 (0~1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(ClampMin="0.0", ClampMax="1.0", AllowPrivateAccess="true"))
	float RestartSteerStrength = 0.7f;

	// Restart 시 스로틀 강도 (0~1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Avoidance",
		meta=(ClampMin="0.0", ClampMax="1.0", AllowPrivateAccess="true"))
	float RestartThrottle = 0.3f;

private:
	// 현재 상태
	ESplineFollowerState CurrentState = ESplineFollowerState::Following;
	// Restart 시 핸들 고정 방향 (-1=왼쪽, +1=오른쪽). 매 틱 다시 정하면 떨림
	float RestartSteerDir = 0.f;
};
