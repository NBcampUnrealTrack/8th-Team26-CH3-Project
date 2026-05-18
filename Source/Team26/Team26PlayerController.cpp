// Copyright Epic Games, Inc. All Rights Reserved.

#include "Team26PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "Team26Pawn.h"
#include "Team26UI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "public/Sensor/SensorViewWidget.h"
#include "Sensor/LidarSensorComponent.h"

void ATeam26PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// spawn the UI widget and add it to the viewport
	VehicleUI = CreateWidget<UTeam26UI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();
	
	if (SensorViewWidgetClass)
	{
		SensorViewWidget = CreateWidget<USensorViewWidget>(this, SensorViewWidgetClass);
		if (SensorViewWidget)
		{
			SensorViewWidget->AddToViewport(10);
			SensorViewWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
	}
}

void ATeam26PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ToggleControlPanelAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleControlPanelAction,
				ETriggerEvent::Started,
				this,
				&ATeam26PlayerController::ToggleControlPanel
			);
		}
		
		// [추가][이한길] 센서뷰 위젯 토글 관련 함수 바인딩
		if (ToggleSensorAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleSensorAction,
				ETriggerEvent::Started,
				this,
				&ATeam26PlayerController::HandleSensorToggle
			);
		}
		if (ToggleLidarAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleLidarAction,
				ETriggerEvent::Started,
				this,
				&ATeam26PlayerController::HandleLidarToggle
			);
		}
	}
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);

		// optionally add the steering wheel context
		if (bUseSteeringWheelControls && SteeringWheelInputMappingContext)
		{
			Subsystem->AddMappingContext(SteeringWheelInputMappingContext, 1);
		}
	}
}

void ATeam26PlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		UChaosWheeledVehicleMovementComponent* VehicleMovement = VehiclePawn->GetChaosVehicleMovement();

		if (VehicleMovement)
		{
			VehicleUI->UpdateSpeed(VehicleMovement->GetForwardSpeed());
			VehicleUI->UpdateGear(VehicleMovement->GetCurrentGear());
			VehicleUI->UpdateRPM(VehicleMovement->GetEngineRotationSpeed());
		}
	}
}

// [추가][이한길] 센서뷰 위젯 토글 관련 함수 4개 정의.
void ATeam26PlayerController::ToggleSensorView(UTextureRenderTarget2D* InCameraRT)
{
	if (!SensorViewWidget) return;
	
	if (InCameraRT)
	{
		SensorViewWidget->SetRenderTarget(InCameraRT);
	}
	SensorViewWidget->ToggleCameraView();
}

void ATeam26PlayerController::ToggleLidarView(UTexture2D* InLidarRT)
{
	if (!SensorViewWidget) return;
	
	if (InLidarRT)
	{
		SensorViewWidget->SetLidarRenderTarget(InLidarRT);
	}
	SensorViewWidget->ToggleLidarView();
}

void ATeam26PlayerController::HandleSensorToggle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Camera Sensor View Toggled!"));
	ToggleSensorView(nullptr);
}

void ATeam26PlayerController::HandleLidarToggle()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Lidar Sensor View Toggled!"));
	if (VehiclePawn && SensorViewWidget)
	{
		// 라이다 센서의 BEV 렌더 타겟(UTexture2D)을 가져옵니다.
		UTexture2D* LidarTex = VehiclePawn->GetLidarSensor()->GetBevRenderTarget();
		ToggleLidarView(LidarTex);
	}
	else
	{
		ToggleLidarView(nullptr);
	}
}

void ATeam26PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	VehiclePawn = CastChecked<ATeam26Pawn>(InPawn);
}

void ATeam26PlayerController::ToggleControlPanel()
{
	if (IsValid(VehicleUI))
	{
		bControlPanelVisible = !bControlPanelVisible;

		VehicleUI->ToggleControlPanel();

		bShowMouseCursor = bControlPanelVisible;

		if (bControlPanelVisible)
		{
			FInputModeGameAndUI InputMode;
			SetInputMode(InputMode);
		}
		else
		{
			FInputModeGameOnly InputMode;
			SetInputMode(InputMode);
		}
	}
}