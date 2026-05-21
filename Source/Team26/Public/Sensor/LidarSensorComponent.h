// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Team26CameraSensorTypes.h"
#include "LidarSensorComponent.generated.h"

struct FDetectedObject;
class ULidarBevRenderer;
class UTextureRenderTarget2D;

UCLASS( ClassGroup=(Sensor), meta=(BlueprintSpawnableComponent), BlueprintType)
class TEAM26_API ULidarSensorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	ULidarSensorComponent();
	
	UFUNCTION( BlueprintCallable, Category = "LidarSensor")
	void StartScan();
	
	UFUNCTION( BlueprintCallable, Category = "LidarSensor")
	void StopScan();
	
	UFUNCTION(BlueprintPure, Category = "LidarSensor")
	UTexture2D* GetBevRenderTarget() const;
	
	// 전방 60도 이내 가장 가까운 장애물 거리를 반환하는 Getter()함수 (cm단위) 
	UFUNCTION(BlueprintPure, Category = "LidarSensor|Safety")
	float GetClosestForwardDistance() const { return ClosestForwardDistance; }
	
	// 전방 60도 중 왼쪽 절반의 가장 가까운 장애물 거리 (cm)
	UFUNCTION(BlueprintPure, Category = "LidarSensor|Safety")
	float GetClosestForwardLeft() const { return ClosestForwardLeftDistance; }

	// 전방 60도 중 오른쪽 절반의 가장 가까운 장애물 거리 (cm)
	UFUNCTION(BlueprintPure, Category = "LidarSensor|Safety")
	float GetClosestForwardRight() const { return ClosestForwardRightDistance; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
#if WITH_EDITOR
	virtual auto PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) -> void override;
#endif

private:
	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void ApplyPreset(ELidarSensorPreset NewPreset);
	
	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void SetScanRate(float Hz);
	
	UFUNCTION(BlueprintCallable, Category = "LidarSensor")
	void RefreshSettings();
	
	void InitializeSensor();
	void StartScanTimer();
	void StopScanTimer();
	
	void OnScanTimer();
	void FireAsyncTraces();
	void CollectAsyncResults();
	void SavePointCloudData();
	
	void RebuildDirectionCache();
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Config",
		meta=(AllowPrivateAccess="true"))
	ELidarSensorPreset Preset = ELidarSensorPreset::VelodyneVLP16;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Config",
		meta=(AllowPrivateAccess="true"))
	bool bSensorEnabled = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Config",
		meta=(AllowPrivateAccess="true"))
	FLidarSensorConfig Config;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|BEV",
		meta=(AllowPrivateAccess="true"))
	FBevRenderConfig BevConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|DataSave",
		meta=(AllowPrivateAccess="true"))
	bool bIsDataSaving = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|DataSave",
		meta=(AllowPrivateAccess="true"))
	FSensorDataSaveConfig DataSaveConfig;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LidarSensor|Output",
		meta=(AllowPrivateAccess="true"))
	FLidarPointCloudData LastPointCloud;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LidarSensor|Output",
		meta=(AllowPrivateAccess="true"))
	int64 FrameCount = 0;
	
	UFUNCTION(BlueprintPure, Category = "LidarSensor")
	const FLidarPointCloudData& GetPointCloud() const { return LastPointCloud; }
	
	UPROPERTY()
	TObjectPtr<ULidarBevRenderer> BevRenderer;
	
	// DBSCAN 제어 파라미터
	UPROPERTY(EditAnywhere, Category = "LidarSensor|DBSCAN", meta = (ClampMin = "1.0"))
	float DbscanEpsilon = 30.0f; // 이웃 인정 거리 (cm 단위, 기본 50cm)

	UPROPERTY(EditAnywhere, Category = "LidarSensor|DBSCAN", meta = (ClampMin = "1"))
	int32 DbscanMinPoints = 5;   // 군집 인정 최소 포인트 개수
	
private:
	FTimerHandle ScanTimerHandle;
	TArray<FTraceHandle> PendingHandles;
	TArray<FVector> PendingWorldDirs;
	FTransform PendingTransform;
	
	bool bHasPendingTraces = false;
	uint64 FireFrameNumber = 0;
	
	TArray<FVector> CachedLocalDirections;
	
	bool bDirectionsDirty = true;
	
	TArray<FVector> ScanPoints;
	TArray<float> ScanIntensities;
	
	// 아래 변수들은 CollectAsyncResults()에서 계산하는 로직을 추가하였습니다.
	// 전방 60도 범위 내 가장 가까운 장애물 거리 (cm 단위)
	// UI에서 이 값을 쓸 때는 센티미터(cm) 단위를 미터(m)로 바꾸기 위해 ClosestForwardDistance / 100.0f를 사용하세요.
	UPROPERTY(BlueprintReadOnly, Category = "LidarSensor|Safety", meta = (AllowPrivateAccess = "true"))
	float ClosestForwardDistance = 99999.0f; // 장애물이 없을 경우에 오작동이 발생하는 것을 방지하기 위해 큰 숫자 사용했습니다.
	// [추가] 좌측 거리 파악
	UPROPERTY(BlueprintReadOnly, Category = "LidarSensor|Safety", meta = (AllowPrivateAccess = "true"))
	float ClosestForwardLeftDistance = 99999.0f;
	// [추가] 우측 거리 파악
	UPROPERTY(BlueprintReadOnly, Category = "LidarSensor|Safety", meta = (AllowPrivateAccess = "true"))
	float ClosestForwardRightDistance = 99999.0f;

	// 경고 판단을 위한 전방 각도 범위
	UPROPERTY(EditAnywhere, Category = "LidarSensor|Safety")
	float ForwardWarningAngle = 60.0f;
	
	// DBSCAN 결과를 받아서 가까운 객체끼리 상자를 합쳐주는 후처리 함수
	TArray<FDetectedObject> MergeCloseBoxes(const TArray<FDetectedObject>& SrcObjects, float MergeDistanceThreshold);
	
public:
	// 디버그 콜리전, ID 텍스트 ON/OFF
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Debug", meta = (DisplayName = "Show 3D Bounding Box"))
	bool bShowDebugBox = false;      // 3D 오렌지 상자 표시 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Debug", meta = (DisplayName = "Show ID Label"))
	bool bShowDebugString = false;   // ID 및 포인트 개수 텍스트 표시 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LidarSensor|Debug", meta = (DisplayName = "Debug Drawing Life Time"))
	float DebugLifeTime = 0.05f;    // 디버그 드로잉 잔상 시간 (초 단위)
};
