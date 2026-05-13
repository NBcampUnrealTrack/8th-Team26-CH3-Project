// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team26Pawn.h"
#include "Team26WheelFront.h"
#include "Team26WheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Team26PlayerController.h"
#include "Sensor/CameraSensorComponent.h"
#include "Sensor/LidarSensorComponent.h" // [추가] 한길님 라이다
#include "Components/SceneCaptureComponent2D.h" // [추가][이한길] 센서뷰 관련.
#include "Engine/TextureRenderTarget2D.h" // [추가][이한길] 센서뷰 관련.
#include "TimerManager.h" // [추가] [강민서] 자동 복구 타이머

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ATeam26Pawn::ATeam26Pawn()
{
	PrimaryActorTick.bCanEverTick = true;

	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	// [추가][이한길] 카메라 센서 생성 및 부착
	CameraSensor = CreateDefaultSubobject<UCameraSensorComponent>(TEXT("CameraSensor"));
	CameraSensor->SetupAttachment(GetMesh()); // 자동차 본체에 부착
	CameraSensor->SetRelativeLocation(FVector(200.f, 0.f, 50.f));

	// [추가][이한길] 라이다 센서 생성 및 부착
	LidarSensor = CreateDefaultSubobject<ULidarSensorComponent>(TEXT("LidarSensor"));
	LidarSensor->SetupAttachment(GetMesh());
	LidarSensor->SetRelativeLocation(FVector(0.f, 0.f, 150.f));

	// [추가] [강민서] 초기값
	bIsResetting = false;
	LastSafeLocation = FVector::ZeroVector;
	LastSafeRotation = FRotator::ZeroRotator;
}

void ATeam26Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ATeam26Pawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ATeam26Pawn::Steering);

		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ATeam26Pawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ATeam26Pawn::Throttle);

		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ATeam26Pawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ATeam26Pawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ATeam26Pawn::StopBrake);

		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ATeam26Pawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ATeam26Pawn::StopHandbrake);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ATeam26Pawn::LookAround);

		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ATeam26Pawn::ToggleCamera);

		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ATeam26Pawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ATeam26Pawn::Tick(float Delta)
{
	Super::Tick(Delta);

	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();

	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;

	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));

	// [추가] [강민서] 정상 주행 위치 저장
	if (!bIsResetting && GetVelocity().Size() > 100.f)
	{
		LastSafeLocation = GetActorLocation();
		LastSafeRotation = GetActorRotation();
	}

	// [추가] [강민서] 자율주행 기본 전진
	if (bAutoDrive && !bIsResetting)
	{
		DoThrottle(0.7f);
	}

	// [추가] [강민서] 차량 멈춤 감지
	if (!bIsResetting && bAutoDrive)
	{
		if (GetVelocity().Size() < 5.f)
		{
			StopTime += Delta;

			if (StopTime >= 2.0f)
			{
				RecoverVehicle();
			}
		}
		else
		{
			StopTime = 0.f;
		}
	}
}

void ATeam26Pawn::Steering(const FInputActionValue& Value)
{
	ChaosVehicleMovement->SetSteeringInput(Value.Get<float>());
}

void ATeam26Pawn::Throttle(const FInputActionValue& Value)
{
	if (!bAutoDrive)
	{
		ChaosVehicleMovement->SetThrottleInput(Value.Get<float>());
	}
}

void ATeam26Pawn::Brake(const FInputActionValue& Value)
{
	ChaosVehicleMovement->SetBrakeInput(Value.Get<float>());
}

void ATeam26Pawn::StartBrake(const FInputActionValue& Value)
{
	BrakeLights(true);
}

void ATeam26Pawn::StopBrake(const FInputActionValue& Value)
{
	BrakeLights(false);

	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ATeam26Pawn::StartHandbrake(const FInputActionValue& Value)
{
	ChaosVehicleMovement->SetHandbrakeInput(true);

	BrakeLights(true);
}

void ATeam26Pawn::StopHandbrake(const FInputActionValue& Value)
{
	ChaosVehicleMovement->SetHandbrakeInput(false);

	BrakeLights(false);
}

void ATeam26Pawn::LookAround(const FInputActionValue& Value)
{
	BackSpringArm->AddLocalRotation(FRotator(0.0f, Value.Get<float>(), 0.0f));
}

void ATeam26Pawn::ToggleCamera(const FInputActionValue& Value)
{
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);

	BackCamera->SetActive(!bFrontCameraActive);
}

void ATeam26Pawn::ResetVehicle(const FInputActionValue& Value)
{
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	FRotator ResetRotation = GetActorRotation();

	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;

	SetActorTransform(
		FTransform(ResetRotation, ResetLocation, FVector::OneVector),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Error, TEXT("Reset Vehicle"));
}

// [추가][이한길] 시작시 센서관련 초기화
void ATeam26Pawn::BeginPlay()
{
	Super::BeginPlay();

	LastSafeLocation = GetActorLocation();
	LastSafeRotation = GetActorRotation();

	if (ATeam26PlayerController* PC = Cast<ATeam26PlayerController>(GetController()))
	{
		if (GetCameraSensor())
		{
			UTextureRenderTarget2D* CameraRT = GetCameraSensor()->GetRenderTarget();

			if (CameraRT)
			{
				PC->ToggleSensorView(CameraRT);
				PC->ToggleSensorView(nullptr);
			}
		}

		if (GetLidarSensor())
		{
			LidarSensor->StartScan();

			UE_LOG(LogTemp, Warning, TEXT("Lidar Scan Started in Pawn BeginPlay"));

			UTexture2D* LidarBEVTexture = GetLidarSensor()->GetBevRenderTarget();

			if (LidarBEVTexture)
			{
				PC->ToggleLidarView(LidarBEVTexture);
				PC->ToggleLidarView(nullptr);
			}
		}
	}
}

// AI 자율주행 제어 함수
void ATeam26Pawn::DoThrottle(float Value)
{
	// 액셀 (0~1)
	ChaosVehicleMovement->SetThrottleInput(Value);
}

void ATeam26Pawn::DoBrake(float Value)
{
	// 브레이크 (0~1), 등도 같이 켜고 끔
	ChaosVehicleMovement->SetBrakeInput(Value);

	BrakeLights(Value > 0.f);
}

void ATeam26Pawn::DoSteering(float Value)
{
	// 핸들 (-1=왼쪽, 1=오른쪽)
	ChaosVehicleMovement->SetSteeringInput(Value);
}

// [추가] [강민서] 충돌 후 2초 정지 후 재출발
void ATeam26Pawn::RecoverVehicle()
{
	if (bIsResetting)
	{
		return;
	}

	bIsResetting = true;

	ChaosVehicleMovement->SetThrottleInput(0.f);
	ChaosVehicleMovement->SetBrakeInput(1.f);
	ChaosVehicleMovement->SetSteeringInput(0.f);

	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	UE_LOG(LogTemplateVehicle, Warning, TEXT("Vehicle Stop"));

	GetWorld()->GetTimerManager().SetTimer(
		RecoverTimerHandle,
		this,
		&ATeam26Pawn::FinishRecoverVehicle,
		2.0f,
		false
	);
}

// [추가] [강민서] 차량 재배치
void ATeam26Pawn::FinishRecoverVehicle()
{
	SetActorLocationAndRotation(
		LastSafeLocation + FVector(0.f, 0.f, 30.f),
		FRotator(0.f, LastSafeRotation.Yaw, 0.f),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

	ChaosVehicleMovement->SetBrakeInput(0.f);
	ChaosVehicleMovement->SetTargetGear(1, true);

	if (bAutoDrive)
	{
		ChaosVehicleMovement->SetThrottleInput(0.7f);
	}

	StopTime = 0.f;
	bIsResetting = false;

	UE_LOG(LogTemplateVehicle, Warning, TEXT("Vehicle Respawn"));
}

#undef LOCTEXT_NAMESPACE