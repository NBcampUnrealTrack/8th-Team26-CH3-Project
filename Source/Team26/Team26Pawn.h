// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Sensor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraSensorComponent> CameraSensor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Sensor", meta = (AllowPrivateAccess = "true"))
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

	// [추가] 자율주행 on/off (true = 자율주행, false = 수동)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AutoDrive")
	bool bAutoDrive = true;

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