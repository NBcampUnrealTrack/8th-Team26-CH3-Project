// Copyright NBC, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Team26/Team26Pawn.h"
#include "Vehicle/Team26VehicleConfigRow.h"
#include "Team26DefaultCar.generated.h"

class UDataTable;

/**
 *  범용 차량 클래스.
 *  Chaos 셋업(휠 본 매핑, 엔진/변속/조향)을 BP 디테일 패널에서 직접 채워서 차종마다 클래스 추가 없이 BP 만으로 새 차량 등록.
 *
 *  사용 방법:
 *    1. BP 만들 때 부모 클래스로 Team26DefaultCar 선택
 *    2. BP 안에서 Skeletal Mesh 지정
 *    3. VehicleMovementComp 디테일에서 Wheel Setups 의 BoneName / WheelClass 4개 채우기
 *    4. (선택) ConfigTable + ConfigRowName 지정해서 차종별 엔진/변속/조향 적용
 */
UCLASS(abstract)
class TEAM26_API ATeam26DefaultCar : public ATeam26Pawn
{
	GENERATED_BODY()

public:
	ATeam26DefaultCar();

protected:
	virtual void BeginPlay() override;

	// 엔진/변속/조향 셋업 데이터 테이블. 지정하면 BeginPlay 에서 ConfigRowName 행을 읽어 적용.
	// 비워두면 생성자 디폴트 그대로 사용.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Config")
	TObjectPtr<UDataTable> ConfigTable;

	// ConfigTable 에서 가져올 행 이름 (예: "Hatchback", "BoxTruck")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle|Config")
	FName ConfigRowName = NAME_None;

private:
	// DataTable 에서 값을 읽어 ChaosVehicleMovement 에 적용
	void ApplyConfigRow(const FTeam26VehicleConfigRow& Row);
};
