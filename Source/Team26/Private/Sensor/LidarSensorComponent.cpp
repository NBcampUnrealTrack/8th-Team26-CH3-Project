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
		TArray<FDetectedObject> RawObjects  = ULidarClusterer::PerformDBSCAN(LastPointCloud.Points, DbscanEpsilon, DbscanMinPoints);

		// 2. [Z축 한정 병합 적용] 수직으로 200cm 이내로 층층이 쌓인 상자들을 위아래로 묶어 기둥으로 만들기
		TArray<FDetectedObject> DetectedObjects = MergeCloseBoxesZOnly(RawObjects, 200.0f);
		
		// 프로젝트에 배치된 장애물들의 측정값 범위를 리스트로 등록 (장애물이 추가되면 여기에 추가하면됨.)
		static const TArray<FLidarClassificationRule> ClassRules = {
			{ TEXT("Dummy"),      140.f,  190.f,   0.f },  // 가로폭 제한은 풀고(0.f), 높이가 140cm ~ 190cm 사이인 날씬한 기둥 모양은 무조건 마네킹으로 분류.
			{ TEXT("Barricade"),  50.f,  200.f,  60.f }, 
			{ TEXT("Cone"),  40.f,  120.f,  0.f },
			{ TEXT("Big Vehicle"),  180.f, 500.f,  200.f }   // 높이 1.8m 이상의 거대 오브젝트
		};
		
		// 분리된 물체들을 순회하며 3D 뷰포트에 오렌지색 상자 그리기
		for (const FDetectedObject& Obj : DetectedObjects)
		{
			FVector Center = Obj.BoundingBox.GetCenter();
			FVector Extent = Obj.BoundingBox.GetExtent();

			// 상자 크기 계산
			float SizeX = Extent.X * 2.0f;
			float SizeY = Extent.Y * 2.0f;
			float SizeZ = Extent.Z * 2.0f;
			float MaxWidth = FMath::Max(SizeX, SizeY);
			
			// 크기 기반 물체 종류 판단.
			FString ObjectType = TEXT("Unknown");

			// 등록된 규칙들을 순회하며 매칭되는 체급 찾기
			for (const FLidarClassificationRule& Rule : ClassRules)
			{
				if (SizeZ >= Rule.MinZ && SizeZ <= Rule.MaxZ && MaxWidth >= Rule.MinWidth)
				{
					ObjectType = Rule.ClassName;
					break; // 매칭되는 것을 찾았으면 규칙 순회 종료
				}
			}
			
			// [필터] 트랙 주변 절벽이나 거대 빌딩 노이즈 차단
			if (SizeX > 500.0f || SizeY > 500.0f || SizeZ > 500.0f)
			{
				continue; // 상자와 텍스트를 그리지 않고 다음 물체로 넘어감
			}
			
			// 차량으로부터 물체 중심까지의 거리 계산.
			FVector SensorLocation = GetComponentLocation();
			// cm 단위를 미터(m) 단위로 변환하기 위해 100.0f로 나눈다.
			float DistanceInMeters = FVector::Dist(SensorLocation, Center) / 100.0f;
			
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
				FVector TextLocation = Center + FVector(0.f, 0.f, Extent.Z + 100.f);
				
				// "물체종류 거리m (포인트수)" 형태로 문자열 포맷팅
				// %.1f를 쓰면 소수점 첫째 짜리까지만 출력됨 (예: 12.4m)
				FString DisplayText = FString::Printf(
					TEXT("%s\n%.1fm"), 
					*ObjectType, 
					DistanceInMeters
				);
				DrawDebugString(
					GetWorld(), 
					TextLocation, 
					DisplayText, 
					nullptr, 
					FColor::White, 
					DebugLifeTime, 
					false, 
					1.3f
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

TArray<FDetectedObject> ULidarSensorComponent::MergeCloseBoxesZOnly(const TArray<FDetectedObject>& SrcObjects, float ZThreshold)
{
    if (SrcObjects.Num() <= 1) return SrcObjects;

    TArray<FDetectedObject> MergedList = SrcObjects;
    bool bMadeMerge = true;
    int32 LoopSanityCheck = 0;

    // 더 이상 Z축으로 합쳐질 상자가 없을 때까지 반복
    while (bMadeMerge && LoopSanityCheck < 10)
    {
        LoopSanityCheck++;
        bMadeMerge = false;
        TArray<FDetectedObject> TempList;
        TSet<int32> SkipIndices;

        for (int32 i = 0; i < MergedList.Num(); ++i)
        {
            if (SkipIndices.Contains(i)) continue;

            FDetectedObject Current = MergedList[i];

            for (int32 j = i + 1; j < MergedList.Num(); ++j)
            {
                if (SkipIndices.Contains(j)) continue;

                // 우선 두 상자가 평면(X, Y)상에서 같은 위치에 있는지 체크.
                // 중심점의 X, Y 거리가 너무 멀면 아예 다른 위치의 장애물이므로 패스.
                float DistX = FMath::Abs(Current.BoundingBox.GetCenter().X - MergedList[j].BoundingBox.GetCenter().X);
                float DistY = FMath::Abs(Current.BoundingBox.GetCenter().Y - MergedList[j].BoundingBox.GetCenter().Y);
                
            	//  두 대상의 원래 가로/세로 크기(체급)를 먼저 파악. 
            	//  가로, 세로뿐만 아니라 높이(Z축)까지 포함하여 상자의 3축 중 가장 큰 길이를 체급 기준으로 잡음.
            	float CurrentMaxDim = Current.BoundingBox.GetSize().GetMax();
            	float TargetMaxDim = MergedList[j].BoundingBox.GetSize().GetMax();

            	// 기본 수평 허용치 (드럼통, 콘 같은 작은 물체용은 80cm로 제한)
            	float DynamicHorizontalThreshold = 80.0f;

            	// 만약 두 상자 중 하나라도 이미 가로/세로/높이 크기중 하나가 2m(200cm)를 넘으면 대형 물체로 판단,
            	// 수평 허용치를 250cm(2.5m)로 크게 열어준다.
            	if (CurrentMaxDim > 250.0f || TargetMaxDim > 250.0f)
            	{
            		DynamicHorizontalThreshold = 250.0f;
            	}

            	// 유동적으로 결정된 허용치를 적용하여 필터링
            	if (DistX > DynamicHorizontalThreshold || DistY > DynamicHorizontalThreshold) 
            	{
            		continue; // 이 범위를 벗어나면 병합하지 않고 스킵
            	}

                // 두 상자의 Z축(높이) 정렬 상태를 계산.
                // 상자 A의 아랫면과 상자 B의 윗면 사이의 순수 수직 거리를 구한다.
                float MaxZ1 = Current.BoundingBox.Max.Z;
                float MinZ1 = Current.BoundingBox.Min.Z;
                float MaxZ2 = MergedList[j].BoundingBox.Max.Z;
                float MinZ2 = MergedList[j].BoundingBox.Min.Z;

                // 두 박스가 수직으로 얼마나 떨어져 있는지 계산 (음수면 이미 겹쳐있다는 뜻)
                float VerticalDistance = FMath::Max(MinZ1 - MaxZ2, MinZ2 - MaxZ1);

                // 수직 거리가 지정한 임계값보다 가까우면 층층이 쌓인 박스로 판단하고 병합
                if (VerticalDistance < ZThreshold)
                {
                    // 두 오브젝트의 흩어진 정점들을 하나로 합친다.
                    Current.Points.Append(MergedList[j].Points);
                    
                    // 합쳐진 정점들을 기준으로 상자의 두께(Z축 포함)를 온전하게 새로 그린다.
                    Current.BoundingBox = FBox(Current.Points);
                    
                    SkipIndices.Add(j);
                    bMadeMerge = true;
                }
            }
            TempList.Add(Current);
        }
        MergedList = TempList;
    }

    return MergedList;
}



