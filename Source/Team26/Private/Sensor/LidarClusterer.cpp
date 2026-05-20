// LidarClusterer.cpp


#include "Sensor/LidarClusterer.h"
#include "Sensor/FLidarPoint.h"
#include "Math/Box.h"

TArray<FDetectedObject> ULidarClusterer::PerformDBSCAN(const TArray<FVector>& InputPoints, float Epsilon, int32 MinPoints)
{
    TArray<FDetectedObject> DetectedObjects;
    if (InputPoints.Num() == 0) return DetectedObjects;

    // 1. 내부 연산용 데이터셋 초기화 (메모리 미리 할당)
    TArray<FDbscanPoint> Dataset;
    Dataset.SetNumUninitialized(InputPoints.Num());
    for (int32 i = 0; i < InputPoints.Num(); ++i)
    {
        Dataset[i].Location = InputPoints[i];
        Dataset[i].OriginalIndex = i;
        Dataset[i].State = EClusterState::Unvisited;
        Dataset[i].ClusterId = -1;
    }
    
    float GridSize = Epsilon; // 격자 한 칸의 크기를 에프실론 거리로 설정하면 효율적임.
    TMap<FGridKey, TArray<int32>> GridMap;

    for (int32 i = 0; i < Dataset.Num(); ++i)
    {
        int32 GridX = FMath::FloorToInt(Dataset[i].Location.X / GridSize);
        int32 GridY = FMath::FloorToInt(Dataset[i].Location.Y / GridSize);
    
        GridMap.FindOrAdd(FGridKey(GridX, GridY)).Add(i);
    }

    int32 CurrentClusterId = 0;

    // 2. 모든 포인트를 순회하며 클러스터링 수행
    for (int32 i = 0; i < Dataset.Num(); ++i)
    {
        if (Dataset[i].State != EClusterState::Unvisited)
        {
            continue; // 이미 방문한 포인트는 건너뜀
        }

        Dataset[i].State = EClusterState::Visited;

        // 내 주변 이웃 찾기
        TArray<int32> NeighborIndices = RegionQueryGrid(Dataset, GridMap, i, Epsilon, GridSize);

        if (NeighborIndices.Num() < MinPoints)
        {
            Dataset[i].State = EClusterState::Noise; // 포인트 부족 시 일단 노이즈 처리
        }
        else
        {
            // 새로운 군집 생성 및 확장 시작
            CurrentClusterId++;
            if (ExpandCluster(Dataset, GridMap, i, NeighborIndices, CurrentClusterId, Epsilon, MinPoints, GridSize))
            {
                UE_LOG(LogTemp, Log, TEXT("Cluster %d Created Successfully!"), CurrentClusterId);
            }
        }
    }

    // 3. ClusterId 별로 포인트들을 모아서 FDetectedObject 배열로 패킹하고 BoundingBox 계산
    // ClusterId를 Key로, 검출된 오브젝트 정보를 Value로 갖는 임시 맵 생성
    TMap<int32, FDetectedObject> ObjectMap;

    for (const FDbscanPoint& DbscanPoint : Dataset)
    {
        // 노이즈(-1)이거나 아직 할당되지 않은 포인트는 제외
        if (DbscanPoint.ClusterId == -1 || DbscanPoint.State == EClusterState::Noise)
        {
            continue;
        }

        int32 Id = DbscanPoint.ClusterId;

        // 맵에 해당 ClusterId가 없다면 새로 추가하고 초기화
        if (!ObjectMap.Contains(Id))
        {
            FDetectedObject NewObject;
            NewObject.Id = Id;
            // FBox를 안전하게 초기화 (Min/Max 값을 계산할 수 있는 상태로 만듦)
            NewObject.BoundingBox = FBox(ForceInit); 
            
            ObjectMap.Add(Id, NewObject);
        }

        // 해당 클러스터 오브젝트를 가리키는 포인터를 가져옴
        FDetectedObject* TargetObj = ObjectMap.Find(Id);

        if (TargetObj)
        {
            // 1) 물체를 구성하는 포인트 배열에 원본 좌표 추가
            TargetObj->Points.Add(DbscanPoint.Location);

            // 2) 바운딩 박스에 포인트를 추가하여 실시간으로 크기를 확장
            TargetObj->BoundingBox += DbscanPoint.Location;
        }
    
    }

    // TMap에 모인 오브젝트들을 최종 반환용 TArray로 변환
    ObjectMap.GenerateValueArray(DetectedObjects);

    return DetectedObjects;
}

TArray<int32> ULidarClusterer::RegionQueryGrid(const TArray<FDbscanPoint>& Dataset, const TMap<FGridKey, TArray<int32>>& GridMap, int32 TargetIdx, float Epsilon, float GridSize)
{
    TArray<int32> NeighborIndices;
    const FVector TargetLoc = Dataset[TargetIdx].Location;
    const float EpsSquared = FMath::Square(Epsilon); // 성능 최적화: 제곱근 연산 회피

    // 타겟 포인트가 속한 격자 좌표 계산
    int32 TargetGridX = FMath::FloorToInt(TargetLoc.X / GridSize);
    int32 TargetGridY = FMath::FloorToInt(TargetLoc.Y / GridSize);

    // 내 중심 칸을 포함한 주변 3x3 (총 9칸) 격자만 조사
    for (int32 x = -1; x <= 1; ++x)
    {
        for (int32 y = -1; y <= 1; ++y)
        {
            FGridKey NeighborKey(TargetGridX + x, TargetGridY + y);
            
            if (const TArray<int32>* PointsInGrid = GridMap.Find(NeighborKey))
            {
                // 해당 격자 칸 안에 있는 포인트들만 거리 검사
                for (int32 Index : *PointsInGrid)
                {
                    if (FVector::DistSquared(TargetLoc, Dataset[Index].Location) <= EpsSquared)
                    {
                        NeighborIndices.Add(Index);
                    }
                }
            }
        }
    }
    return NeighborIndices;
}

bool ULidarClusterer::ExpandCluster(TArray<FDbscanPoint>& Dataset, 
        const TMap<FGridKey, TArray<int32>>& GridMap,
        int32 TargetIdx, 
        const TArray<int32>& NeighborIndices, 
        int32 ClusterId, 
        float Epsilon, 
        int32 MinPoints,
        float GridSize
    )
{
    Dataset[TargetIdx].ClusterId = ClusterId;

    // [수정] 중복 체크를 고속으로 하기 위해 TSet(집합)을 활용.
    TSet<int32> SeedsSet(NeighborIndices);
    // 이웃들을 검사할 큐(Queue)처럼 사용할 동적 배열 생성
    TArray<int32> SeedsArray = NeighborIndices;

    // 안전장치: 라이다 포인트 총 개수보다 많이 도는 것은 논리적으로 불가능하므로 맥스 값 설정
    const int32 MaxSafetyIteration = Dataset.Num() * 2;
    int32 IterationCount = 0;
    
    for (int32 i = 0; i < SeedsArray.Num(); ++i)
    {
        // 안전장치. (혹시 모를 상황에도 절대 엔진이 크래시 나거나 멈추지 않음)
        if (++IterationCount > MaxSafetyIteration)
        {
            UE_LOG(LogTemp, Warning, TEXT("DBSCAN ExpandCluster: 무한 루프 방지 안전핀이 작동했습니다."));
            break;
        }
        
        int32 CurrentSeedIdx = SeedsArray[i];

        // 노이즈로 분류되었던 포인트라면, 리더는 못 되어도 이 군집의 경계 포인트로는 합류 가능
        if (Dataset[CurrentSeedIdx].State == EClusterState::Noise)
        {
            Dataset[CurrentSeedIdx].State = EClusterState::Visited;
            Dataset[CurrentSeedIdx].ClusterId = ClusterId;
        }

        if (Dataset[CurrentSeedIdx].State != EClusterState::Unvisited)
        {
            continue;
        }

        Dataset[CurrentSeedIdx].State = EClusterState::Visited;
        Dataset[CurrentSeedIdx].ClusterId = ClusterId;

        // 이웃의 이웃을 찾음 (밀도 확장)
        TArray<int32> CurrentSeedNeighbors = RegionQueryGrid(Dataset, GridMap, CurrentSeedIdx, Epsilon, GridSize);

        if (CurrentSeedNeighbors.Num() >= MinPoints)
        {
            // 새로운 핵심 포인트를 찾았으므로, 검사 대기열(Seeds)에 이웃들을 추가
            for (int32 NeighborIdx : CurrentSeedNeighbors)
            {
                // TSet의 Contains는 O(1)이라 초고속이며, 중복을 허용하지 않는다.
                if (!SeedsSet.Contains(NeighborIdx))
                {
                    SeedsSet.Add(NeighborIdx);
                    SeedsArray.Add(NeighborIdx); // 안전하게 순회용 배열에도 추가
                }
            }
        }
    }
    return true;
}