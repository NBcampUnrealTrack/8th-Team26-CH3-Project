// LidarClusterer.h

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LidarClusterer.generated.h"

/**
 * 
 */
UCLASS()
class TEAM26_API ULidarClusterer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** 
	 * DBSCAN 알고리즘을 실행하여 포인트 클라우드를 물체 단위로 분리합니다.
	 * @param InputPoints - 센서로부터 수집된 Raw 포인트 배열
	 * @param Epsilon - 이웃 판단 기준 거리 (센티미터 단위, 예: 50.f)
	 * @param MinPoints - 군집 인정 최소 포인트 개수
	 * @return 검출된 오브젝트들의 배열
	 */
	UFUNCTION(BlueprintCallable, Category = "Lidar|Clustering")
	static TArray<FDetectedObject> PerformDBSCAN(const TArray<FVector>& InputPoints, float Epsilon, int32 MinPoints);

private:
	// 특정 포인트 주변 Epsilon 이내에 있는 이웃 포인트들의 인덱스를 찾는 함수 (핵심 최적화 대상)
	static TArray<int32> RegionQuery(const TArray<FDbscanPoint>& Dataset, int32 TargetIdx, float Epsilon);

	// 조건 만족 시 군집을 주변으로 전염시키며 확장하는 함수
	static bool ExpandCluster(TArray<FDbscanPoint>& Dataset, int32 TargetIdx, const TArray<int32>& NeighborIndices, int32 ClusterId, float Epsilon, int32 MinPoints);
	
};
