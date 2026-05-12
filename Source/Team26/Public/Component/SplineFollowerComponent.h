// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplineFollowerComponent.generated.h"

class ATeam26Pawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM26_API USplineFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USplineFollowerComponent();
	
protected:
	// 경로 빌드
	virtual void BeginPlay() override;

	// 조향 / 가속 결정
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 맵에서 LandscapeSpline찾아 점 목록 만듦 (한번만)
	void    BuildPath();
	// 점 사이를 부드러운 곡선으로 채움
	void    ResampleCatmullRom();
	// "지금부터 N미터 앞 위치"와 "그 방향" 반환
	FVector GetPointAhead(FVector& OutDirection, float Distance) const;
	// 앞쪽 길이 얼마나 굽었는지 (라디안)
	float   EstimateCurvature(float AheadOffset) const;
	// 곡률 -> 안전 속도 환산
	float   ComputeCurveSpeedLimit(float Curvature) const;

private:
	//-------------------------------------
	//------------ 속도 변수 ---------------
	//-------------------------------------
	
	// 최대 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float MaxSpeed = 3000.f; 
	// 최저 속도 (곡선에서도 이 밑으론 안감)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float MinSpeed = 400.f;
	// 속도 차이를 페달 입력으로 환산하는 비율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float ThrottleGain = 0.002f;
	// 목표속도 줄어들 때 따라가는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float DecelRate = 0.5f;
	// 목표속도 늘어날 때 따라가는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Speed",
		meta=(AllowPrivateAccess="true"))
	float AccelRate = 1.f;

	// AccelRate > DecelRate 라서 감속이 가속보다 부드러움.

	//-------------------------------------
	//------------ 조향 변수 ---------------
	//-------------------------------------

	// 앞을 보는 최소 기본 거리(15m)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float LookAheadBase = 1500.f;
	// 속도 * LookAheadSpeedFactor 만큼 더 멀리 봄 (빠르면 더 앞을 봐야 함)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float LookAheadSpeedFactor = 0.3f;
	// 핸들 풀로 돌렸을 때의 한계각 (정규화용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float MaxYawDelta = 40.f;
	// 길 발향 vs 점 위치중 무엇을 더 신경 쓸지 (0~1로 max제한)
	// 0.7f이면 (70% 길 방향 따라가고 30% 목표점으로 가라)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(ClampMin="0", ClampMax="1", AllowPrivateAccess="true"))
	float HeadingWeight = 0.7f;
	// 경로에서 옆으로 벗어났을때 복귀하는 힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Steering",
		meta=(AllowPrivateAccess="true"))
	float CrosstrackGain = 0.0015f;

	//-------------------------------------
	//------------ 곡률/감속 변수 ------------
	//-------------------------------------

	// 타이어 횡방향 마찰.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature",
		meta=(ClampMin="0.1", ClampMax="2.0", AllowPrivateAccess="true"))
	float LateralFriction = 0.8f;
	// 곡률 측정용 두 점 사이 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature",
		meta=(AllowPrivateAccess="true"))
	float CurvatureSampleSpan = 50.f;
	// BrakePreviewDist 앞 곡률까지 미리 보고 감속 (기본 50m)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Curvature",
		meta=(AllowPrivateAccess="true"))
	float BrakePreviewDist = 5000.f;
	// 차 주변 SearchRadius 안에서 가장 가까운 스플라인 점 찾기 (기본 50m)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path",
		meta=(AllowPrivateAccess="true"))
	float SearchRadius = 5000.f;
	// Catmull-Rom 으로 점 채울때 간격 (기본 50cm마다 한점)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineFollower|Path",
		meta=(AllowPrivateAccess="true"))
	float ResampleSpacing = 50.f;

	//-------------------------------------
	//------------ 내부 상태 변수 ------------
	//-------------------------------------

	// 자동차. 약한 참조. Pawn 파괴돼도 댕글링 포인터 안 됨. IsValid()로 체크
	UPROPERTY()
	TWeakObjectPtr<ATeam26Pawn> OwnerPawn;
	
private:
	// 만들어진 경로 점들 (월드 좌표)
	TArray<FVector> PathPoints;
	// 차가 지금 몇 번째 점 부근에 있는지
	int32 CurrentPointIndex = 0;
	// 경로가 닫힌 루프인지 (트랙처럼 한 바퀴)
	bool  bClosedLoop = false;
	// 목표속도 (튀지 않게 부드럽게 따라감) default: MaxSpeed
	float SmoothedTargetSpeed = 0.f;
};
