// LidarSensorComponent.cpp


#include "Sensor/LidarSensorComponent.h"
#include "Sensor/LidarBevRenderer.h"
#include "Sensor/LidarClusterer.h"
#include "Engine/World.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h" // DragDebugLine 사용위한 헤더.
#include "Sensor/FLidarPoint.h"

DEFINE_LOG_CATEGORY_STATIC(LogLidarSensor, Log, All);


ULidarSensorComponent::ULidarSensorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ULidarSensorComponent::StartScan()
{
	bSensorEnabled = true;
	SetComponentTickEnabled(true);
	StartScanTimer();
}

void ULidarSensorComponent::StopScan()
{
	bSensorEnabled = false;
	StopScanTimer();
}

UTexture2D* ULidarSensorComponent::GetBevRenderTarget() const
{
	return BevRenderer ? BevRenderer->GetRenderTarget() : nullptr;
}


void ULidarSensorComponent::BeginPlay()
{
	Super::BeginPlay();
	ApplyPreset(Preset);
	InitializeSensor();
}

void ULidarSensorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopScanTimer();
	Super::EndPlay(EndPlayReason);
}

void ULidarSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bHasPendingTraces && GFrameCounter > FireFrameNumber)
	{
		CollectAsyncResults();
		//SetComponentTickEnabled(false); 라이다 센서가 지속적으로 작동을 안해서 주석처리함.
	}
	
	/* 이제 필요 없어보여서 주석처리했습니다. 필요시 다시 키셔도 됩니다.
	// 라이다 센서 감지 문제 확인 위한 디버그 라인
	FVector Start = GetComponentLocation();
	FVector ForwardVector = GetForwardVector();
	FVector End = Start + (ForwardVector * Config.MaxRange);

	// 충돌 감지 로직 (기존 코드)
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner()); // 본인 차량 무시 설정

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	// 디버그 라인 출력 코드
	FColor LineColor = bHit ? FColor::Red : FColor::Green; // 감지되면 빨간색, 아니면 초록색
    
	// DrawDebugLine(월드, 시작점, 끝점, 색상, 지속여부, 수명, 우선순위, 두께)
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, -1.0f, 0, 2.0f);
	*/
}

#if WITH_EDITOR
void ULidarSensorComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropName = PropertyChangedEvent.GetPropertyName();
	if (PropName == GET_MEMBER_NAME_CHECKED(ULidarSensorComponent, Preset))
	{
		ApplyPreset(Preset);
	}
	bDirectionsDirty = true;
}
#endif

void ULidarSensorComponent::ApplyPreset(ELidarSensorPreset NewPreset)
{
	Preset = NewPreset;
	bDirectionsDirty = true;
	
	switch (NewPreset)
	{
	case ELidarSensorPreset::VelodyneVLP16:
		Config = {16, 1800, 10.0f, 10000.0f, 50.0f, 15.0f, -15.0f, 360.0f, 2.0f};
		break;
		
	case ELidarSensorPreset::VelodyneVLP32:
		Config = { 32, 60, 10.0f, 20000.0f, 50.0f, 15.0f, -25.0f, 360.0f, 2.0f };
		break;

	case ELidarSensorPreset::OusterOS1_64:
		Config = { 64, 45, 10.0f, 12000.0f, 50.0f, 22.5f, -22.5f, 360.0f, 1.5f };
		break;

	case ELidarSensorPreset::Livox_Mid360:
		Config = { 8, 45, 10.0f, 7000.0f, 100.0f, 52.0f, -7.0f, 360.0f, 3.0f };
		break;

	case ELidarSensorPreset::Custom:
	default:
		break;
	}
}

void ULidarSensorComponent::SetScanRate(float Hz)
{
	Config.RotationRate = FMath::Clamp(Hz, 1.0f, 30.0f);
	StopScanTimer();
	if (bSensorEnabled)
	{
		StartScanTimer();
	}
}

void ULidarSensorComponent::RefreshSettings()
{
	bDirectionsDirty = true;
	BevConfig.ViewRange = Config.MaxRange;
	
	if (BevRenderer)
	{
		BevRenderer->UpdateConfig(BevConfig);
	}
	
	if (bSensorEnabled)
	{
		StartScanTimer();
	}
}

void ULidarSensorComponent::InitializeSensor()
{
	BevConfig.ViewRange = Config.MaxRange;
	
	BevRenderer = NewObject<ULidarBevRenderer>(this, TEXT("BevRenderer"));
	BevRenderer->Initialize(BevConfig);
	
	const int32 TotalPts = Config.GetTotalPoints();
	PendingHandles.Reserve(TotalPts);
	PendingWorldDirs.Reserve(TotalPts);
	ScanPoints.Reserve(TotalPts);
	ScanIntensities.Reserve(TotalPts);
	LastPointCloud.Points.Reserve(TotalPts);
	LastPointCloud.Intensities.Reserve(TotalPts);
	
	bDirectionsDirty = true;
	bSensorEnabled = false;
	
	UE_LOG(LogLidarSensor, Log,
		TEXT("LidarSensor initialized : %d ch X %d pts @ %.0f Hz, range %.0f m [AsyncTrace]"),
		Config.NumChannels, Config.PointsPerChannel,
		Config.RotationRate, Config.MaxRange / 100.0f
	);
}

void ULidarSensorComponent::StartScanTimer()
{
	if (GetWorld() == nullptr) return;
	
	const float Interval = 1.0f / FMath::Max(Config.RotationRate, 1.0f);
	GetWorld()->GetTimerManager().SetTimer(
		ScanTimerHandle,
		this,
		&ULidarSensorComponent::OnScanTimer,
		Interval,
		true);
}

void ULidarSensorComponent::StopScanTimer()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
		
		bHasPendingTraces = false;
		SetComponentTickEnabled(false);
	}
}

void ULidarSensorComponent::OnScanTimer()
{
	if (!bSensorEnabled) return;
	
	if (bDirectionsDirty)
	{
		RebuildDirectionCache();
	}
	
	FireAsyncTraces();
	
	++FrameCount;
}

void ULidarSensorComponent::FireAsyncTraces()
{
	UWorld* World = GetWorld();
	if (!World || CachedLocalDirections.IsEmpty()) return;
	
	PendingHandles.Reset();
	PendingWorldDirs.Reset();
	
	const FTransform SensorTransform = GetComponentTransform();
	const FVector SensorLoc = SensorTransform.GetLocation();
	const FQuat SensorQuat = SensorTransform.GetRotation();
	PendingTransform = SensorTransform;
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LidarAsyncTrace), false);
	Params.AddIgnoredActor(GetOwner());
	Params.bReturnPhysicalMaterial = false;
	
	const float MaxRange = Config.MaxRange;
	
	for (const FVector& LocalDir : CachedLocalDirections)
	{
		const FVector WorldDir = SensorQuat.RotateVector(LocalDir);
		const FVector End = SensorLoc + WorldDir * MaxRange;
		
		FTraceHandle Handle = World->AsyncLineTraceByChannel(
			EAsyncTraceType::Single,
			SensorLoc, End,
			ECC_Visibility,
			Params
		);
		
		PendingHandles.Add(Handle);
		bHasPendingTraces = true;
		
		SetComponentTickEnabled(true);
	}
}

void ULidarSensorComponent::CollectAsyncResults()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	ScanPoints.Reset();
	ScanIntensities.Reset();

	const float MaxRange = Config.MaxRange;
	const float MinRange = Config.MinRange;
	const float NoiseStd = Config.NoiseStdDev;

	for (int32 i = 0; i < PendingHandles.Num(); ++i)
	{
		FTraceDatum Data;
		if (!World->QueryTraceData(PendingHandles[i], Data)) continue;
		if (Data.OutHits.IsEmpty()) continue;

		const FHitResult& Hit = Data.OutHits[0];
		
		// 태그 체크로 센서에서 바닥을 무시하게함.
		if (Hit.GetActor() && Hit.GetActor()->ActorHasTag(TEXT("Track")))
		{
			continue; // 트랙 태그가 있으면 거리 계산에서 제외하고 다음 포인트로 넘어감
		}
		
		if (!Hit.bBlockingHit || Hit.Distance < MinRange) continue;

		FVector HitPoint = Hit.ImpactPoint;
		if (NoiseStd > 0.f && PendingWorldDirs.IsValidIndex(i))
		{
			HitPoint += PendingWorldDirs[i] * FMath::RandRange(-NoiseStd, NoiseStd);
		}
		
		ScanPoints.Add(HitPoint);
		ScanIntensities.Add(FMath::Clamp(1.f - (Hit.Distance / MaxRange), 0.f, 1.f));
	}
	
	LastPointCloud.Points = MoveTemp(ScanPoints);
	LastPointCloud.Intensities = MoveTemp(ScanIntensities);
	LastPointCloud.PointCount = LastPointCloud.Points.Num();
	LastPointCloud.FrameNumber = FrameCount;
	
	ScanPoints.Reserve(Config.GetTotalPoints());
	ScanIntensities.Reserve(Config.GetTotalPoints());
	
	// 전방 장애물 경고 위한 변수 계산 (전체 / 왼쪽 / 오른쪽 동시)
	float MinDist      = Config.MaxRange;  // 전체 (기존)
	float MinDistLeft  = Config.MaxRange;  // 왼쪽 절반 (LocalPt.Y < 0)
	float MinDistRight = Config.MaxRange;  // 오른쪽 절반 (LocalPt.Y > 0)
	const float HalfAngle = ForwardWarningAngle * 0.5f;

	for (const FVector& Point : LastPointCloud.Points)
	{
		// 월드 → 센서 로컬 좌표
		const FVector LocalPt = PendingTransform.InverseTransformPosition(Point);

		// 전방(+X) 만
		if (LocalPt.X <= 0) continue;

		// 부호 있는 각도 (왼쪽 = 음수, 오른쪽 = 양수, UE 좌표계)
		const float AngleDegSigned = FMath::RadiansToDegrees(FMath::Atan2(LocalPt.Y, LocalPt.X));
		const float AngleDeg       = FMath::Abs(AngleDegSigned);

		// 전방 부채꼴 밖이면 스킵
		if (AngleDeg > HalfAngle) continue;

		const float Dist = LocalPt.Size();

		// 전체 최솟값
		if (Dist < MinDist) MinDist = Dist;

		// 좌/우 최솟값 (UE 좌표계: +Y가 오른쪽, -Y가 왼쪽)
		if (AngleDegSigned < 0.f)
		{
			if (Dist < MinDistLeft) MinDistLeft = Dist;
		}
		else
		{
			if (Dist < MinDistRight) MinDistRight = Dist;
		}
	}

	ClosestForwardDistance      = MinDist;
	ClosestForwardLeftDistance  = MinDistLeft;
	ClosestForwardRightDistance = MinDistRight;

	
	// 수집 완료된 LastPointCloud.Points 기반 객체 탐지 및 3D 시각화.
	if (LastPointCloud.Points.Num() > 0)
	{
		// 라이브러리를 정적(Static) 호출하여 물체 분리 및 바운딩 박스 계산
		TArray<FDetectedObject> DetectedObjects = ULidarClusterer::PerformDBSCAN(LastPointCloud.Points, DbscanEpsilon, DbscanMinPoints);

		// 분리된 물체들을 순회하며 3D 뷰포트에 오렌지색 상자 그리기
		for (const FDetectedObject& Obj : DetectedObjects)
		{
			FVector Center = Obj.BoundingBox.GetCenter();
			FVector Extent = Obj.BoundingBox.GetExtent();

			// 3D 바운딩 박스 드로잉
			if (bShowDebugBox)
			{
				DrawDebugBox(
					GetWorld(),
					Center,
					Extent,
					FColor::Orange, // 검출된 물체는 오렌지색
					false,          // 영구 지속 안함
					DebugLifeTime,  // 수명 (프레임 단위 갱신을 위해 짧게 유지)
					0,              // 우선순위
					2.0f            // 선 두께
				);
			}
			

			// 물체 중심점 상단에 ID 및 포인트 개수 텍스트 띄우기
			if (bShowDebugString)
			{
				FVector TextLocation = Center + FVector(0.f, 0.f, Extent.Z + 20.f);
				DrawDebugString(
					GetWorld(), 
					TextLocation, 
					FString::Printf(TEXT("ID: %d (Pts: %d)"), Obj.Id, Obj.Points.Num()), 
					nullptr, 
					FColor::White, 
					DebugLifeTime, 
					false, 
					1.1f
				);
			}
		
		}
	}
	
	// BEV 렌더링 및 데이터 저장.
	if (BevRenderer)
	{
		BevRenderer->RenderPointCloud(LastPointCloud, PendingTransform);
	}
	
	if (bIsDataSaving && LastPointCloud.PointCount > 0)
	{
		SavePointCloudData();
	}
	
	bHasPendingTraces = false;
	
	UE_LOG(LogLidarSensor, Verbose,
		TEXT("LidarSensor frame %lld : %d points"), FrameCount, LastPointCloud.PointCount);
}

void ULidarSensorComponent::SavePointCloudData()
{
	const FString Dir = FPaths::ProjectSavedDir() / TEXT("SensorData") / DataSaveConfig.SensorLabel;
	IFileManager::Get().MakeDirectory(*Dir, true);
	
	const FString FilePath = Dir / FString::Printf(TEXT("%06lld.bin"), FrameCount);
	
	TUniquePtr<IFileHandle> File(FPlatformFileManager::Get().GetPlatformFile().OpenWrite(*FilePath));
	if (!File)
	{
		UE_LOG(LogLidarSensor, Warning, TEXT("Failed to open file for  writing : %s"), *FilePath);
		return;
	}
	
	const FTransform InvSensor = PendingTransform.Inverse();
	const int32 NumPoints = LastPointCloud.PointCount;
	
	struct FKittiPoint {float X, Y, Z, Intensity;};
	TArray<FKittiPoint> Buffer;
	Buffer.SetNumUninitialized(NumPoints);
	
	for (int32 i = 0; i < NumPoints; ++i)
	{
		const FVector Local = InvSensor.TransformPosition(LastPointCloud.Points[i]);
		Buffer[i].X = static_cast<float>(Local.X * 0.01);
		Buffer[i].Y = static_cast<float>(Local.Y * 0.01);
		Buffer[i].Z = static_cast<float>(Local.Z * 0.01);
		Buffer[i].Intensity = LastPointCloud.Intensities[i];
	}
	
	File->Write(
		reinterpret_cast<const uint8*>(Buffer.GetData()),
		NumPoints * sizeof(FKittiPoint)
	);
	
	UE_LOG(LogLidarSensor, Verbose, TEXT("Saved %d points -> %s"), NumPoints, *FilePath);
}

void ULidarSensorComponent::RebuildDirectionCache()
{
	const int32 NumCh = Config.NumChannels;
	const int32 NumPts = Config.PointsPerChannel;
	const float VertLow = Config.VerticalFOVLower;
	const float VertRng = Config.VerticalFOVUpper - VertLow;
	const float HorizFOV = Config.HorizontalFOV;
	
	CachedLocalDirections.SetNum(NumCh * NumPts, EAllowShrinking::No);
	
	for (int32 Ch = 0; Ch < NumCh; ++Ch)
	{
		const float VertDeg = (NumCh > 1) ? VertLow + VertRng * (float(Ch) / (NumCh - 1)) : 0.f;
		const float CosVert = FMath::Cos(FMath::DegreesToRadians(VertDeg));
		const float SinVert = FMath::Sin(FMath::DegreesToRadians(VertDeg));
		
		for (int32 Pt = 0; Pt < NumPts; ++Pt)
		{
			const float HorizRad = FMath::DegreesToRadians((float(Pt) / NumPts) * HorizFOV);
			CachedLocalDirections[Ch * NumPts + Pt] = FVector(
				CosVert * FMath::Cos(HorizRad),
				CosVert * FMath::Sin(HorizRad),
				SinVert
			);
		}
	}
	
	bDirectionsDirty = false;
}



