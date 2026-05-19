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
        TArray<int32> NeighborIndices = RegionQuery(Dataset, i, Epsilon);

        if (NeighborIndices.Num() < MinPoints)
        {
            Dataset[i].State = EClusterState::Noise; // 포인트 부족 시 일단 노이즈 처리
        }
        else
        {
            // 새로운 군집 생성 및 확장 시작
            CurrentClusterId++;
            if (ExpandCluster(Dataset, i, NeighborIndices, CurrentClusterId, Epsilon, MinPoints))
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

        // 해당 클러스터 오브젝트의 레퍼런스를 가져옴
        FDetectedObject& TargetObj = ObjectMap[Id];

        // 1) 물체를 구성하는 포인트 배열에 원본 좌표 추가
        TargetObj.Points.Add(DbscanPoint.Location);

        // 2) 바운딩 박스에 포인트를 추가하여 실시간으로 크기를 확장
        TargetObj.BoundingBox.AddPoint(DbscanPoint.Location);
    }

    // TMap에 예쁘게 모인 오브젝트들을 최종 반환용 TArray로 변환
    ObjectMap.GenerateValueArray(DetectedObjects);

    return DetectedObjects;
}

TArray<int32> ULidarClusterer::RegionQuery(const TArray<FDbscanPoint>& Dataset, int32 TargetIdx, float Epsilon)
{
    TArray<int32> NeighborIndices;
    const FVector TargetLoc = Dataset[TargetIdx].Location;
    const float EpsSquared = FMath::Square(Epsilon); // ⭐️ 성능 최적화: 제곱근 연산 회피

    // [⚠️ 주의] 현재 구조는 O(N^2) 전체 순회입니다. 
    // 나중에는 이 부분을 격자(Grid) 검색이나 Octree 검색으로 교체해야 프레임 드랍이 없습니다.
    for (int32 i = 0; i < Dataset.Num(); ++i)
    {
        if (FVector::DistSquared(TargetLoc, Dataset[i].Location) <= EpsSquared)
        {
            NeighborIndices.Add(i);
        }
    }
    return NeighborIndices;
}

bool ULidarClusterer::ExpandCluster(TArray<FDbscanPoint>& Dataset, int32 TargetIdx, const TArray<int32>& NeighborIndices, int32 ClusterId, float Epsilon, int32 MinPoints)
{
    Dataset[TargetIdx].ClusterId = ClusterId;

    // 이웃들을 검사할 큐(Queue)처럼 사용할 동적 배열 생성
    TArray<int32> Seeds = NeighborIndices;

    for (int32 i = 0; i < Seeds.Num(); ++i)
    {
        int32 CurrentSeedIdx = Seeds[i];

        // 노이즈로 분류되었던 포인트라면, 리더는 못 되어도 이 군집의 '경계 포인트'로는 합류 가능
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
        TArray<int32> CurrentSeedNeighbors = RegionQuery(Dataset, CurrentSeedIdx, Epsilon);

        if (CurrentSeedNeighbors.Num() >= MinPoints)
        {
            // 새로운 핵심 포인트를 찾았으므로, 검사 대기열(Seeds)에 이웃들을 추가
            for (int32 NeighborIdx : CurrentSeedNeighbors)
            {
                if (!Seeds.Contains(NeighborIdx)) // 중복 방지
                {
                    Seeds.Add(NeighborIdx);
                }
            }
        }
    }
    return true;
}