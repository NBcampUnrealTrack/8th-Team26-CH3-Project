// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "TimerManager.h" // [추가] [강민서] 복구 타이머
#include "Team26Pawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class UCameraSensorComponent;
class ULidarSensorComponent;

struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

UCLASS(abstract)
class ATeam26Pawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	// [추가][이한길] 차량에 센서 부착
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sensor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraSensorComponent> CameraSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sensor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULidarSensorComponent> LidarSensor;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	bool bFrontCameraActive = false;

public:

	ATeam26Pawn();

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	virtual void Tick(float Delta) override;

	// AI 자율주행 제어 함수
	void DoThrottle(float Value);  // 액셀
	void DoBrake(float Value);     // 브레이크
	void DoSteering(float Value);  // 핸들

	// 자율주행 on/off (true = 자율주행, false = 수동)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoDrive")
	bool bAutoDrive = true;

	// [백종태] BeginPlay 시 라이다 자동 스캔 여부. 기본 false — 회피 차량 BP 에서만 true 로 켬.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sensor")
	bool bAutoStartLidar = false;

	// [백종태] BeginPlay 시 카메라 캡쳐 자동 시작 여부. 기본 false — 플레이어 차량 BP 에서 true 로 켬.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sensor")
	bool bAutoStartCamera = false;
	
	//[강민서] 충돌 후 자동 복구
	void RecoverVehicle();

	//[강민서] 복구 완료 처리
	void FinishRecoverVehicle();

protected:

	void Steering(const FInputActionValue& Value);

	void Throttle(const FInputActionValue& Value);

	void Brake(const FInputActionValue& Value);

	void StartBrake(const FInputActionValue& Value);

	void StopBrake(const FInputActionValue& Value);

	void StartHandbrake(const FInputActionValue& Value);

	void StopHandbrake(const FInputActionValue& Value);

	void LookAround(const FInputActionValue& Value);

	void ToggleCamera(const FInputActionValue& Value);

	void ResetVehicle(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void BrakeLights(bool bBraking);

	// [추가] [이한길] 센서 UI관련 로직.
	virtual void BeginPlay() override;

private:

	//[강민서] 복구 중 체크
	bool bIsResetting = false;

	//[강민서] 정지 시간 체크
	float StopTime = 0.f;

	//[강민서] 마지막 정상 위치 저장
	FVector LastSafeLocation;

	//[강민서] 마지막 정상 회전 저장
	FRotator LastSafeRotation;

	//[강민서] 복구 타이머
	FTimerHandle RecoverTimerHandle;

	// [추가] [강민서] 전복 감지 타이머
	float FlipTimer = 0.f;

	// [추가] [강민서] 전복 판단 각도 (기본 60도)
	UPROPERTY(EditAnywhere, Category = "VehicleReset")
	float FlipAngleThreshold = 60.f;

	// [추가] [강민서] 전복 후 리셋까지 대기 시간 (기본 3초)
	UPROPERTY(EditAnywhere, Category = "VehicleReset")
	float FlipResetDelay = 3.f;

public:

	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }

	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }

	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }

	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }

	// [추가][이한길] 센서뷰 관련.
	FORCEINLINE UCameraSensorComponent* GetCameraSensor() const { return CameraSensor; }

	FORCEINLINE ULidarSensorComponent* GetLidarSensor() const { return LidarSensor; }
};