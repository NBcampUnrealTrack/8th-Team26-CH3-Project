// Copyright NBC, Inc. All Rights Reserved.

#include "Component/SplineFollowerAvoidanceComponent.h"
#include "Team26/Team26.h"
#include "Team26/Team26Pawn.h"
#include "Sensor/LidarSensorComponent.h"

namespace
{
	const TCHAR* StateToString(ESplineFollowerState S)
	{
		switch (S)
		{
		case ESplineFollowerState::Following:  return TEXT("Following");
		case ESplineFollowerState::Avoiding:   return TEXT("Avoiding");
		case ESplineFollowerState::Stopping:   return TEXT("Stopping");
		case ESplineFollowerState::Restarting: return TEXT("Restarting");
		}
		return TEXT("?");
	}
}

USplineFollowerAvoidanceComponent::USplineFollowerAvoidanceComponent()
{
}

bool USplineFollowerAvoidanceComponent::HandleStateOverride()
{
	if (OwnerPawn.IsValid() == false) return false;

	//-------------------------------------------------------------------
	// 상태머신 — Following / Avoiding / Stopping / Restarting
	//-------------------------------------------------------------------
	ULidarSensorComponent* Lidar = OwnerPawn->FindComponentByClass<ULidarSensorComponent>();

	ESplineFollowerState NewState = ESplineFollowerState::Following;
	float SnapForward = 99999.f, SnapLeft = 99999.f, SnapRight = 99999.f;
	float CurVel = OwnerPawn->GetVelocity().Size();

	if (Lidar)
	{
		SnapForward = Lidar->GetClosestForwardDistance();
		SnapLeft    = Lidar->GetClosestForwardLeft();
		SnapRight   = Lidar->GetClosestForwardRight();

		const float Diff = SnapLeft - SnapRight;
		const bool bCanAvoid = (FMath::Abs(Diff) > AvoidanceHysteresis);

		// Restarting 은 sticky — 정면이 다시 열릴 때까지 유지
		if (CurrentState == ESplineFollowerState::Restarting)
		{
			if (SnapForward >= CriticalStopDist)
			{
				// 정면이 비워짐 → 평소로 복귀
				NewState = ESplineFollowerState::Following;
			}
			else
			{
				// 아직 막힘 → Restart 계속
				NewState = ESplineFollowerState::Restarting;
			}
		}
		else
		{
			// 평소 상태 판단
			if (SnapForward >= AvoidanceStartDist)
			{
				NewState = ESplineFollowerState::Following;
			}
			else if (SnapForward < CriticalStopDist)
			{
				// 5m 미만 — 정지 후 거의 안 움직이면 Restart 시작
				if (CurVel < StoppedSpeedThreshold && CurrentState == ESplineFollowerState::Stopping)
				{
					NewState = ESplineFollowerState::Restarting;
				}
				else
				{
					NewState = ESplineFollowerState::Stopping;
				}
			}
			else
			{
				// 5m ~ 15m
				if (bCanAvoid)
				{
					NewState = ESplineFollowerState::Avoiding;
				}
				else
				{
					// 좌우 거의 같음 — 회피 불가능 보이지만 일단 한쪽 강제로 시도
					// 더 비어있는 쪽 (살짝이라도) 선택
					RestartSteerDir = (SnapLeft >= SnapRight) ? -1.f : 1.f;
					NewState = ESplineFollowerState::Avoiding;
				}
			}
		}
	}

	// 상태 변경 시에만 로그 (스팸 방지)
	if (NewState != CurrentState)
	{
		UE_LOG(LogDigitalTwinNbc, Log,
			TEXT("[%s] State: %s -> %s | Fwd=%.0fcm L=%.0fcm R=%.0fcm Vel=%.0fcm/s"),
			*OwnerPawn->GetName(),
			StateToString(CurrentState),
			StateToString(NewState),
			SnapForward, SnapLeft, SnapRight, CurVel);

		// Restart 진입 시 방향 결정 (한 번만)
		if (NewState == ESplineFollowerState::Restarting)
		{
			RestartSteerDir = (SnapLeft > SnapRight) ? -1.f : 1.f;
			UE_LOG(LogDigitalTwinNbc, Log,
				TEXT("[%s] Restart direction: %s (Left=%.0fcm Right=%.0fcm)"),
				*OwnerPawn->GetName(),
				(RestartSteerDir < 0.f) ? TEXT("LEFT") : TEXT("RIGHT"),
				SnapLeft, SnapRight);
		}

		CurrentState = NewState;
	}

	// 상태별 행동 — Stopping / Restarting 은 즉시 처리 후 true 반환
	switch (CurrentState)
	{
	case ESplineFollowerState::Stopping:
		OwnerPawn->DoThrottle(0.f);
		OwnerPawn->DoBrake(1.f);
		return true;

	case ESplineFollowerState::Restarting:
		// 가장 비어있는 쪽으로 핸들 꺾고 살살 전진
		OwnerPawn->DoSteering(RestartSteerDir * RestartSteerStrength);
		OwnerPawn->DoThrottle(RestartThrottle);
		OwnerPawn->DoBrake(0.f);
		return true;

	case ESplineFollowerState::Following:
	case ESplineFollowerState::Avoiding:
		// 평소 주행 로직 계속 진행
		return false;
	}
	return false;
}

float USplineFollowerAvoidanceComponent::ComputeExtraSteer() const
{
	return ComputeAvoidanceSteer();
}

float USplineFollowerAvoidanceComponent::ComputeAvoidanceSteer() const
{
	if (!OwnerPawn.IsValid()) return 0.f;

	ULidarSensorComponent* Lidar = OwnerPawn->FindComponentByClass<ULidarSensorComponent>();
	if (!Lidar) return 0.f;

	const float ForwardDist = Lidar->GetClosestForwardDistance();

	// 정면이 충분히 비어있으면 회피 안 함
	if (ForwardDist >= AvoidanceStartDist) return 0.f;

	const float LeftDist  = Lidar->GetClosestForwardLeft();
	const float RightDist = Lidar->GetClosestForwardRight();

	// 위험도: 가까울수록 1 에 가까움 (Forward=0 → 1, Forward=AvoidanceStartDist → 0)
	const float Urgency = 1.f - FMath::Clamp(ForwardDist / AvoidanceStartDist, 0.f, 1.f);

	const float Diff = LeftDist - RightDist;  // 양수 = 왼쪽이 더 비었음

	// 좌/우 거의 같으면 어느 쪽으로 갈지 모호 → 회피 안 함 (지터 방지)
	if (FMath::Abs(Diff) < AvoidanceHysteresis) return 0.f;

	// UE 좌표계: Steer 음수 = 왼쪽, 양수 = 오른쪽
	if (Diff > 0.f)
	{
		// 왼쪽이 더 비었음 → 왼쪽으로 회피
		return -Urgency * AvoidanceStrength;
	}
	else
	{
		// 오른쪽이 더 비었음 → 오른쪽으로 회피
		return +Urgency * AvoidanceStrength;
	}
}
