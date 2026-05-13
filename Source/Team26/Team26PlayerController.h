// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Team26PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATeam26Pawn;
class UTeam26UI;
class USensorViewWidget;

/**
 *  Vehicle Player Controller class
 *  Handles input mapping and user interface
 */
UCLASS(abstract)
class TEAM26_API ATeam26PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	/** If true, the optional steering wheel input mapping context will be registered */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	bool bUseSteeringWheelControls = false;

	/** Optional Input Mapping Context to be used for steering wheel input.
	 *  This is added alongside the default Input Mapping Context and does not block other forms of input.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta=(EditCondition="bUseSteeringWheelControls"))
	UInputMappingContext* SteeringWheelInputMappingContext;

	/** Pointer to the controlled vehicle pawn */
	TObjectPtr<ATeam26Pawn> VehiclePawn;

	/** Type of the UI to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<UTeam26UI> VehicleUIClass;

	/** Pointer to the UI widget */
	TObjectPtr<UTeam26UI> VehicleUI;
	
	bool bControlPanelVisible = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleControlPanelAction;

	// [추가][이한길] 센서뷰 위젯 토글 관련 변수 4개 추가.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI)
	TSubclassOf<USensorViewWidget> SensorViewWidgetClass;
	
	UPROPERTY()
	TObjectPtr<USensorViewWidget> SensorViewWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleSensorAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleLidarAction;
	
	// Begin Actor interface
protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:

	virtual void Tick(float Delta) override;
	
	// [추가][이한길] 센서뷰 위젯 토글 관련 함수 4개 추가.
	void ToggleSensorView(UTextureRenderTarget2D* InCameraRT);
	void ToggleLidarView(UTexture2D* InLidarRT);
	void HandleSensorToggle();
	void HandleLidarToggle();

	// End Actor interface

	// Begin PlayerController interface
protected:

	virtual void OnPossess(APawn* InPawn) override;
	void ToggleControlPanel();

	// End PlayerController interface
};
