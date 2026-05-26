// Copyright NBC, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Team26VehicleConfigRow.generated.h"

/**
 *  DataTable 로 차량 엔진/변속/조향 설정을 관리하기 위한 행 정의.
 *
 *  사용 방법:
 *    1. CSV 작성 — Name, MaxTorque, MaxRPM, FinalRatio, ForwardGearRatios, ChangeUpRPM, ChangeDownRPM, AngleRatio
 *    2. 콘텐츠 브라우저에서 CSV import → "Team26VehicleConfigRow" 행 타입 선택 → DataTable 에셋 생성
 *    3. Team26DefaultCar 자식 BP 의 디테일 패널에서 ConfigTable + ConfigRowName 지정
 *
 *  속도/가속 튜닝 가이드:
 *    - 최고속도 ↑ : MaxRPM ↑, FinalRatio ↓, ForwardGearRatios 마지막 단 ↓
 *    - 가속     ↑ : MaxTorque ↑, ForwardGearRatios 첫 단 ↑
 *    - 회전 반경 ↓ : AngleRatio ↑
 */
USTRUCT(BlueprintType)
struct TEAM26_API FTeam26VehicleConfigRow : public FTableRowBase
{
	GENERATED_BODY()

	// 엔진 최대 토크 (가속력)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float MaxTorque = 700.f;

	// 엔진 최대 RPM (최고속도 한계)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float MaxRPM = 6000.f;

	// 아이들 RPM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float EngineIdleRPM = 900.f;

	// 엔진 브레이크 강도 (0~1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Engine")
	float EngineBrakeEffect = 0.2f;

	// 최종 기어비 (낮을수록 최고속도 ↑, 가속 ↓)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float FinalRatio = 2.81f;

	// 업쉬프트 RPM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float ChangeUpRPM = 5500.f;

	// 다운쉬프트 RPM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float ChangeDownRPM = 2000.f;

	// 변속 시간 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	float GearChangeTime = 0.2f;

	// 전진 기어비 배열 (예: [4.25, 2.52, 1.66, 1.22, 1.0])
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transmission")
	TArray<float> ForwardGearRatios;

	// 조향 핸들 각도 비율 (0~1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steering")
	float SteeringAngleRatio = 0.7f;
};
