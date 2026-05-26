// Copyright NBC, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Component/SplineFollowerComponent.h"
#include "SplineFollowerACCComponent.generated.h"

/**
 *  Adaptive Cruise Control (ACC) 컴포넌트.
 *  라이다 정면 거리가 FollowGap 안에 들어오면 비례 감속, CriticalGap 이하면 정지.
 *
 *  부모(USplineFollowerComponent) 의 GetExtraSpeedCap() 을 override 해서
 *  Tick 의 SpeedLimit 계산에 자동으로 적용됨.
 *
 *  거리 단계:
 *    [0 ~ CriticalGap]            정지         (cap = 0)
 *    [CriticalGap ~ DesiredGap]   강한 감속    (cap = MaxSpeed * (Dist - Desired) / (Follow - Desired))
 *    [DesiredGap ~ FollowGap]     비례 감속    (위 공식 그대로)
 *    [FollowGap 이상]              영향 없음    (cap = MAX)
 *
 *  주의: 라이다는 차량/장애물 구분 X — 트래픽 콘 앞에서도 감속함 (의도된 안전 동작).
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM26_API USplineFollowerACCComponent : public USplineFollowerComponent
{
	GENERATED_BODY()

public:
	USplineFollowerACCComponent();

protected:
	// 부모 SpeedLimit 계산에 ACC 캡 합성
	virtual float GetExtraSpeedCap() const override;

private:
	// 라이다 정면 가장 가까운 장애물 거리 측정 (없으면 +INF)
	float GetForwardObstacleDistance() const;

private:
	//-------------------------------------
	//------------ ACC 거리 임계값 -----------
	//-------------------------------------

	// 추종 시작 거리 (cm). 이 안에 장애물 들어오면 감속 시작. 기본 15m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|ACC",
		meta=(AllowPrivateAccess="true"))
	float FollowGap = 1500.f;

	// 안전 거리 (cm). 이 안이면 더 강하게 감속. 기본 8m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|ACC",
		meta=(AllowPrivateAccess="true"))
	float DesiredGap = 800.f;

	// 비상 정지 거리 (cm). 이 안이면 SpeedCap = 0 (즉시 정지). 기본 4m
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|ACC",
		meta=(AllowPrivateAccess="true"))
	float CriticalGap = 400.f;
};
