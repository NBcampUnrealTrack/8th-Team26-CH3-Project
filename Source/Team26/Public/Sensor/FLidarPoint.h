#pragma once

#include "CoreMinimal.h"
#include "FLidarPoint.generated.h" 

// 클러스터 상태를 나타내는 열거형
UENUM(BlueprintType)
enum class EClusterState : uint8
{
	Unvisited, // 아직 검사하지 않음
	Visited,   // 검사 완료
	Noise      // 노이즈로 분류됨
};

// 클러스터링 연산용 확장 포인트 구조체
struct FDbscanPoint
{
	FVector Location;
	int32 OriginalIndex; // 원본 배열로 역추적하기 위한 인덱스
	EClusterState State = EClusterState::Unvisited;
	int32 ClusterId = -1; // -1은 아직 어떤 그룹에도 속하지 않음을 의미
};

// 하나의 완성된 객체(클러스터) 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FDetectedObject
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Lidar")
	int32 Id;

	UPROPERTY(BlueprintReadOnly, Category = "Lidar")
	TArray<FVector> Points; // 이 물체를 구성하는 포인트들

	UPROPERTY(BlueprintReadOnly, Category = "Lidar")
	FBox BoundingBox; // 물체의 3D 박스 범위
};


