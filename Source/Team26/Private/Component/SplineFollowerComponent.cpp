// Copyright NBC, Inc. All Rights Reserved.

#include "Component/SplineFollowerComponent.h"
#include "Team26/Team26.h"
#include "Team26/Team26Pawn.h"
#include "EngineUtils.h"
#include "LandscapeSplineActor.h"
#include "LandscapeSplineControlPoint.h"
#include "LandscapeSplineSegment.h"
#include "LandscapeSplinesComponent.h"
#include "Sensor/LidarSensorComponent.h" // [추가] 한길님 라이다

DEFINE_LOG_CATEGORY_STATIC(LogPathFollowingComponent, Log, All);

USplineFollowerComponent::USplineFollowerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// 경로 없는상태에서 Tick은 무의미. 시작할 땐 Tick 꺼짐. BuildPath() 성공하면 켬
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USplineFollowerComponent::BuildPath()
{
	if (OwnerPawn.IsValid() == false)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("Failed to cache OwnerPawn."));
		return;
	}

	// 기존 경로 비우기 (재호출 가능성)
	PathPoints.Reset();
	bClosedLoop = false;

	/////////////////////////////////////////////////////////////////////////////
	// Landscape Spline Component 찾기
	
	// 월드의 모든 ALandscapeSplineActor 순회 첫 번째 발견된거 사용
	// 차 두 대 경로 지정시 여길 수정
	ULandscapeSplinesComponent* SplinesComp = nullptr;
	for (TActorIterator<ALandscapeSplineActor> It(GetWorld()); It; ++It)
	{
		SplinesComp = It->GetSplinesComponent();
		if (SplinesComp) break;
	}
	if (!SplinesComp)
	{
		UE_LOG(LogDigitalTwinNbc, Warning, TEXT("SplineFollower: No ALandscapeSplineActor found"));
		return;
	}
	
	/////////////////////////////////////////////////////////////////////////////
	// 차량 주변 SearchRadius 안에서 가장 가까운 Landscape Spline 컨트롤 포인트를 찾기

	// 스플라인 컴포넌트의 로컬좌표를 월드좌표로 변환하기 위한 캐싱
	const FTransform ToWorld = SplinesComp->GetComponentTransform();
	// 차량의 월드좌표
	const FVector VehicleLoc = OwnerPawn->GetActorLocation();

	float BestDist = SearchRadius;
	ULandscapeSplineControlPoint* NearestCP = nullptr;
	for (ULandscapeSplineControlPoint* CP : SplinesComp->GetControlPoints())
	{
		// 컨트롤포인트의 로컬좌표를 월드좌표로 변환하고 차량과의 거리를 계산
		const float D = FVector::Dist(ToWorld.TransformPosition(CP->Location), VehicleLoc);
		if (D < BestDist)
		{
			BestDist = D;
			NearestCP = CP;
		}
	}
	if (!NearestCP)
	{
		UE_LOG(LogDigitalTwinNbc, Warning, TEXT("SplineFollower: No control point within SearchRadius"));
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// 경로의 시작점(StartCP) 찾기
	
	// 방문한 세그먼트 저장
	TSet<ULandscapeSplineSegment*> Visited;
	ULandscapeSplineControlPoint* StartCP = NearestCP;
	while (true)
	{
		ULandscapeSplineSegment* Seg = nullptr;
		// StartCP에 연결된 세그먼트 중 방문하지 않은 첫 세그먼트를 고른뒤 break
		for (const auto& Conn : StartCP->ConnectedSegments)
		{
			if (!Visited.Contains(Conn.Segment))
			{
				Seg = Conn.Segment;
				break;
			}
		}
		
		// 더 이상 가볼 세그먼트가 없으면 종료(현재 StartCP가 경로의 끝)
		if (!Seg) break;
		// 방문 처리. 다음 점에서 이쪽 세그먼트를 통해 순회하는걸 막음
		Visited.Add(Seg);
		
		// 세그먼트는 양 끝에 CP를 2개 가짐. 그 중 선택된 CP의 반대 CP(Other)를 찾음
		// 간선을 따라서 이웃 정점으로 이동하기위해
		ULandscapeSplineControlPoint* Other =
			(Seg->Connections[0].ControlPoint == StartCP)
			? Seg->Connections[1].ControlPoint
			: Seg->Connections[0].ControlPoint;

		// 순환경로완성으로 종료
		if (Other == NearestCP) 
			{ StartCP = NearestCP; break; }
		// 다음 CP로 이동
		StartCP = Other;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 보간 포인트 수집 (실제 경로 데이터 수집)
	
	Visited.Reset();
	ULandscapeSplineControlPoint* CurCP = StartCP;
	while (true)
	{
		ULandscapeSplineSegment* Seg = nullptr;
		for (const auto& Conn : CurCP->ConnectedSegments)
		{
			if (!Visited.Contains(Conn.Segment)) 
				{ Seg = Conn.Segment; break; }
		}
		if (!Seg) break;
		Visited.Add(Seg);

		// Seg->GetPoints()는 Conn[0] -> Conn[1] 방향으로 정렬된 보간포인트들을 반환
		// 그러나 세그먼트의 어느 CP쪽으로 진입했냐에따라 진행 방향이 다름 그래서 역순을 판변하는 플래그를 둠
		const bool bReversed = (Seg->Connections[1].ControlPoint == CurCP); // true면 역방향
		// 세그먼트가 가진 포인트들은 양쪽 CP를 보간하는 작은 포인트
		const TArray<FLandscapeSplineInterpPoint>& Pts = Seg->GetPoints();

		// 세그먼트 포인트가 2보다 작으면 선분으로서 의미없으므로 스킵
		if (Pts.Num() >= 2)
		{
			//                                   Seg1       Seg2
			// 인접한 두 세그먼트는 CP를 공유함. CP1 ------ CP2 ------ CP3 
			// PathPoints에 Pts를 추가할때 CP2가 중복됨.
			// 첫 세그먼트의 포인트들은 다 넣고 (인덱스 0부터)
			// 다음 세그먼트는 첫 포인트는 건너뜀 (인덱스 1부터 넣음)
			const bool bSkipFirst = PathPoints.Num() > 0;
			if (!bReversed) // 정방향
				for (int32 i = (bSkipFirst ? 1 : 0); i < Pts.Num(); ++i)
					PathPoints.Add(ToWorld.TransformPosition(Pts[i].Center));
			else
				for (int32 i = Pts.Num() - 1 - (bSkipFirst ? 1 : 0); i >= 0; --i)
					PathPoints.Add(ToWorld.TransformPosition(Pts[i].Center));
		}

		// 역방향이였다면 Conn[0]으로 정방향이면 Conn[1]쪽으로 이동
		ULandscapeSplineControlPoint* NextCP = bReversed
			? Seg->Connections[0].ControlPoint : Seg->Connections[1].ControlPoint;
		// 닫힌 루프면 종료. 
		if (NextCP == StartCP) 
			{ bClosedLoop = true; break; }
		CurCP = NextCP;
	}

	// 점이 3개 미만이면 곡선 보간이 불가능하므로 종료.
	if (PathPoints.Num() < 3)
	{
		UE_LOG(LogDigitalTwinNbc, Warning, TEXT("SplineFollower: Too few path points (%d)"), PathPoints.Num());
		return;
	}

	// PathPoints의 각 구간을 Catmull-Rom 곡선으로 보간한 뒤 
	// 일정 간격으로 리샘플링해서 부드럽고 균일한 점들로 교체.
	ResampleCatmullRom();

	//////////////////////////////////////////////////////////////////////////////////////////
	// 차량과 가장 가까운 점을 찾아 거기서 부터 시작하게함.
	
	// 루프가 끝나면 CurrentPointIndex가 차량과 가장 가까운 점의 인덱스가됨
	float BestSq = TNumericLimits<float>::Max();
	CurrentPointIndex = 0;
	for (int32 i = 0; i < PathPoints.Num(); ++i)
	{
		const float Sq = FVector::DistSquared(PathPoints[i], VehicleLoc);
		if (Sq < BestSq) 
			{ BestSq = Sq; CurrentPointIndex = i; }
	}
	
	// 출발 직후부터 최대 속도를 향해 가속이 시작
	SmoothedTargetSpeed = MaxSpeed;
	// 틱 컴폰너트 활성화
	SetComponentTickEnabled(true);

	UE_LOG(LogDigitalTwinNbc, Log, TEXT("SplineFollower: %d pts, loop=%s, start=%d"),
		PathPoints.Num(), bClosedLoop ? TEXT("Y") : TEXT("N"), CurrentPointIndex);
}

static FVector EvalCatmullRom(const FVector& P0, const FVector& P1,
	const FVector& P2, const FVector& P3, float T)
{
	const float T2 = T * T, T3 = T2 * T;
	return 0.5f * (
		(2.f * P1) +
		(-P0 + P2) * T +
		(2.f * P0 - 5.f * P1 + 4.f * P2 - P3) * T2 +
		(-P0 + 3.f * P1 - 3.f * P2 + P3) * T3);
}

void USplineFollowerComponent::ResampleCatmullRom()
{
	const int32 N = PathPoints.Num();
	if (N < 3) return;

	auto Idx = [&](int32 i) -> int32
		{
			return bClosedLoop ? ((i % N) + N) % N : FMath::Clamp(i, 0, N - 1);
		};

	TArray<FVector> Out;
	Out.Reserve(N * 12);

	for (int32 i = 0; i < N - 1; ++i)
	{
		const FVector& P0 = PathPoints[Idx(i - 1)];
		const FVector& P1 = PathPoints[i];
		const FVector& P2 = PathPoints[i + 1];
		const FVector& P3 = PathPoints[Idx(i + 2)];
		const int32 Steps = FMath::Max(
			1,
			FMath::CeilToInt32(FVector::Dist(P1, P2) / ResampleSpacing)
		);

		for (int32 s = 0; s < Steps; ++s)
		{
			FVector CatmullRom = EvalCatmullRom(P0, P1, P2, P3, (float)s / (float)Steps);
			Out.Add(CatmullRom);
		}
	}
	Out.Add(PathPoints.Last());

	if (bClosedLoop)
	{
		const FVector& P0 = PathPoints[N - 2]; const FVector& P1 = PathPoints[N - 1];
		const FVector& P2 = PathPoints[0];     const FVector& P3 = PathPoints[1];
		const int32 Steps = FMath::Max(1, FMath::CeilToInt32(FVector::Dist(P1, P2) / ResampleSpacing));
		for (int32 s = 1; s < Steps; ++s)
			Out.Add(EvalCatmullRom(P0, P1, P2, P3, (float)s / (float)Steps));
	}

	UE_LOG(LogDigitalTwinNbc, Log, TEXT("SplineFollower: Resampled %d -> %d pts"), N, Out.Num());
	PathPoints = MoveTemp(Out);
}

FVector USplineFollowerComponent::GetPointAhead(FVector& OutDir, float Distance) const
{
	if (OwnerPawn.IsValid() == false)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("Failed to cache OwnerPawn."));
		return FVector::ZeroVector;
	}

	const int32 Num = PathPoints.Num();
	const FVector Loc = OwnerPawn->GetActorLocation();

	const int32 Nxt = bClosedLoop ? (CurrentPointIndex + 1) % Num
		: FMath::Min(CurrentPointIndex + 1, Num - 1);
	const FVector A = PathPoints[CurrentPointIndex], B = PathPoints[Nxt];
	const FVector AB = B - A;
	const float Len = AB.Size();
	const float T = (Len > KINDA_SMALL_NUMBER)
		? FMath::Clamp(FVector::DotProduct(Loc - A, AB) / (Len * Len), 0.f, 1.f) : 0.f;
	const float Remain = (1.f - T) * Len;

	if (Distance <= Remain && Len > KINDA_SMALL_NUMBER)
	{
		OutDir = AB.GetSafeNormal();
		return FMath::Lerp(A, B, FMath::Min(T + Distance / Len, 1.f));
	}

	float Left = Distance - Remain;
	int32 Idx = Nxt;
	while (Left > 0.f)
	{
		const int32 Next = bClosedLoop ? (Idx + 1) % Num : Idx + 1;
		if (!bClosedLoop && Next >= Num)
		{
			OutDir = (PathPoints[Idx] - PathPoints[FMath::Max(0, Idx - 1)]).GetSafeNormal();
			return PathPoints[Idx];
		}
		const FVector Seg = PathPoints[Next] - PathPoints[Idx];
		const float SLen = Seg.Size();
		if (Left <= SLen && SLen > KINDA_SMALL_NUMBER)
		{
			OutDir = Seg.GetSafeNormal();
			return FMath::Lerp(PathPoints[Idx], PathPoints[Next], Left / SLen);
		}
		Left -= SLen;
		Idx = Next;
		if (Idx == CurrentPointIndex) break;
	}

	const int32 Prev = bClosedLoop ? ((Idx - 1 + Num) % Num) : FMath::Max(0, Idx - 1);
	OutDir = (PathPoints[Idx] - PathPoints[Prev]).GetSafeNormal();
	return PathPoints[Idx];
}

float USplineFollowerComponent::EstimateCurvature(float AheadOffset) const
{
	FVector D1, D2;
	GetPointAhead(D1, AheadOffset);
	GetPointAhead(D2, AheadOffset + CurvatureSampleSpan);
	return FMath::Acos(FMath::Clamp(FVector::DotProduct(D1, D2), -1.f, 1.f));
}

float USplineFollowerComponent::ComputeCurveSpeedLimit(float Curvature) const
{
	if (Curvature <= KINDA_SMALL_NUMBER)
		return MaxSpeed;
	const float Radius = CurvatureSampleSpan / Curvature;
	return FMath::Clamp(
		FMath::Sqrt(LateralFriction * 980.f * Radius),
		MinSpeed, MaxSpeed
	);
}

void USplineFollowerComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerPawn = Cast<ATeam26Pawn>(GetOwner());
	BuildPath();
}

void USplineFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (OwnerPawn.IsValid() == false)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("Failed to cache OwnerPawn."));
		return;
	}
	if (PathPoints.Num() < 2)
	{
		UE_LOG(LogPathFollowingComponent, Error, TEXT("PathPoints has single point."));
		return;
	}

	// [추가] 자율주행 꺼져있으면 아무것도 안함
	if (OwnerPawn->bAutoDrive == false) return;

	// [추가] 라이다에서 장애물 거리 가져와서 500cm(5m) 이내면 자동 브레이크
	ULidarSensorComponent* Lidar = OwnerPawn->FindComponentByClass<ULidarSensorComponent>();
	if (Lidar && Lidar->GetClosestForwardDistance() < 500.f)
	{
		OwnerPawn->DoThrottle(0.f);
		OwnerPawn->DoBrake(1.f);
		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	// 현재 차량 구간이 어딘지 (CurrentPointIndex 업데이트)
	
	const FVector Location = OwnerPawn->GetActorLocation();	// 차량 월드 위치
	const float Velocity = OwnerPawn->GetVelocity().Size(); // 차량 속력
	// 닫힌루프(모든인덱스 순회가능), 열린경로(마지막 선분이 P[N-2]에서 P[N-1]으로 끝남 그래서 마지막 구간 인덱스가 N-2)
	const int32 Max = bClosedLoop ? PathPoints.Num() : PathPoints.Num() - 2;
	// 한 틱에 몇 점이나 지나칠수 있는지 계산
	const int32 MaxSteps = FMath::CeilToInt32(
		Velocity / (60.f * FMath::Max(ResampleSpacing, 1.f))
	) + 1;

	// 차량이 빨라서 여러점 지났을때 현재 인덱스도 그만큼 증가시킴
	for (int32 S = 0; S < MaxSteps && CurrentPointIndex < Max; ++S)
	{
		const int32 Next = (CurrentPointIndex + 1) % PathPoints.Num();			// 다음 점 인덱스
		const FVector Seg = PathPoints[Next] - PathPoints[CurrentPointIndex];	// 현재 구간 벡터
		// (Location - A) ` (B - A) = |Location - A| * |B - A| * 투영길이 / |Location - A| >= |B - A|^2
		// 투영길이 >= |B - A|
		// 차량의 이동거리(투영길이)가 구간벡터 길이보다 크면 인덱스를 증가
		if (FVector::DotProduct(Location - PathPoints[CurrentPointIndex], Seg) >= Seg.SizeSquared())
			CurrentPointIndex = bClosedLoop ? Next : CurrentPointIndex + 1;
		else
			break; // 차량이 현재 구간 벡터 안에 있음
	}

	/////////////////////////////////////////////////////////////////////////////
	// 트랙 끝 도달 시 정지 (열린 경로 전용)
	if (!bClosedLoop && CurrentPointIndex >= PathPoints.Num() - 2)
	{
		OwnerPawn->DoThrottle(0.f);
		OwnerPawn->DoBrake(1.f);
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// 지각
	
	// 곡률 측정
	const float Yaw = OwnerPawn->GetActorRotation().Yaw;			// 현재 차량 방향 (차량 헤딩)

	// 현재곡률(조향 강도 조정), 앞쪽곡률(코너 감지, 사전 감속)
	const float CurvHere = EstimateCurvature(0.f);		// 현재 차량 위치 기준 곡률
	const float CurvAhead = EstimateCurvature(BrakePreviewDist);	// 차량 앞쪽 기준 곡률


	// 동적 Look-Ahead 거리 계산
	// 곡률에 따른 스케일 계산 (직선에선 1, 강한 코너에선 0.5)
	const float CurvScale = FMath::Lerp(1.f, 0.5f, FMath::Clamp(CurvHere * 3.f, 0.f, 1.f));
	// 전방 응시 거리 (최소 응시 거리 + 속도에 비례한 거리) * 곡선이면 가까이 봄
	const float LADist = (LookAheadBase + Velocity * LookAheadSpeedFactor) * CurvScale;
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	// 판단 (세 가지 오차 계산)

	// PosDelta만으로 제어하면 차량이 코너 안쪽으로 컷팅합니다. HdgDelta만으로는 경로에서 벗어났을 때 복귀를 못함.
	
	FVector PathDir;											// 그 지점에서의 경로 접선 방향
	const FVector LAPos = GetPointAhead(PathDir, LADist);	// 앞쪽 목표 위치

	// 목표점 방향과 차량 헤딩의 각도 차이 (목표점 보려면 얼마나 돌릴까) (위치)
	const float PosDelta = FMath::FindDeltaAngleDegrees(  // 두 각도의 차이를 작은 각도로 반환
		Yaw,
		FMath::Atan2(
			(LAPos - Location).GetSafeNormal().Y,
			(LAPos - Location).GetSafeNormal().X
		) * (180.f / PI)
	);

	// 경로 방향과 차량 헤딩의 각도 차이 (도로의 방향에 맞추려면 얼마나 돌릴까) (방향)
	const float HdgDelta = FMath::FindDeltaAngleDegrees(
		Yaw, FMath::Atan2(PathDir.Y, PathDir.X) * (180.f / PI));

	///////////////////////////////////////////////////////////////////////////////////////
	// Crosstrack Error (횡방향 오차) - 차량이 옆으로 얼마나 어느 쪽 방향으로 벗어났는지
	
	const int32 Nxt = bClosedLoop ?
		(CurrentPointIndex + 1) % PathPoints.Num()
		: FMath::Min(CurrentPointIndex + 1, PathPoints.Num() - 1);
	const FVector SegDir = (PathPoints[Nxt] - PathPoints[CurrentPointIndex]).GetSafeNormal(); // 현재 구간 단위벡터
	const FVector Offset = Location - PathPoints[CurrentPointIndex]; // 시작점에서 차량까지의 벡터
	const float CrossErr = FVector::DotProduct(
		Offset - SegDir * FVector::DotProduct(Offset, SegDir), // 차량에서 경로 방향 수직 벡터(최단거리)
		FVector::CrossProduct(FVector::UpVector, SegDir)	   // 왼손법칙 경로기준 오른쪽 축
	);

	/////////////////////////////////////////////////////////////////////////////////////////
	// 조향 명령
	
	// 코너에선 PosDelta 영향, 직선에선 HdgDelta 영향 많이 받게
	// 직선에선 HeadingWeight값 그대로 코너에선 0.3f까지 줄어듬
	const float HdgW = FMath::Lerp(HeadingWeight, 0.3f, FMath::Clamp(CurvHere * 3.f, 0.f, 1.f));
	const float YawCmd = PosDelta * (1.f - HdgW) + HdgDelta * HdgW; // 목표 조향 각도
	const float Steer = FMath::Clamp(								// 최종 차량 조향 값
		YawCmd / MaxYawDelta - CrossErr * CrosstrackGain,
		-1.f, 1.f
	);
	OwnerPawn->DoSteering(Steer);

	/////////////////////////////////////////////////////////////////////////////////////
	// 속도 명령
	
	// 속도제한. 현재 코너와 다가올 코너 중 더 느린 속도 선택
	// 둘다 직선이면 거의 MaxSpeed
	const float SpeedLimit = FMath::Min(
		ComputeCurveSpeedLimit(CurvHere),
		ComputeCurveSpeedLimit(CurvAhead)
	);

	// 목표 속도 부드럽게 변화 (정지 -> 출발 경우 AccelRate, 곡선 진입 DecelRate)
	const float Rate = (SpeedLimit < SmoothedTargetSpeed) ? DecelRate : AccelRate;
	SmoothedTargetSpeed = FMath::FInterpTo(SmoothedTargetSpeed, SpeedLimit, DeltaTime, Rate);

	// 목표 속도가 현재 속도 보다 크면 악셀, 작으면 브레이크
	const float Cmd = FMath::Clamp(
		(SmoothedTargetSpeed - Velocity) * ThrottleGain,
		-1.f, 1.f
	);
	
	// 작은 오차 0.05이내는 무시해서 안정 상태를 유지
	if (Cmd > 0.05f)
	{
		OwnerPawn->DoThrottle(Cmd);
	}
	else if (Cmd < -0.05f)
	{
		OwnerPawn->DoBrake(-Cmd);
	}
	else
	{
		OwnerPawn->DoThrottle(0.f);
		OwnerPawn->DoBrake(0.f);
	}
}