#include "DataLogger/VehicleGeographicHudManager.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "GeographicCoordinates.h"
#include "GeoReferencingSystem.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DataLogger/VehicleGeographicHudWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogVehicleGeographicHudManager, Log, All);

namespace
{
	constexpr int32 ScreenMessageKey = -1;
	constexpr float ScreenMessageDurationSeconds = 5.0f;
	constexpr float MinimumUpdateRateHz = 1.0f;
	constexpr float MinimumResolveRetryIntervalSeconds = 0.05f;
	constexpr int32 MaxCoordinateDecimalPlaces = 6;
	constexpr int32 MaxAltitudeDecimalPlaces = 2;

	const FText WaitingForVehicleLocationText = NSLOCTEXT(
		"VehicleGeographicHudManager",
		"WaitingForVehicleLocation",
		"Waiting for vehicle geographic location..."
	);

	const FText HudCreatedText = NSLOCTEXT(
		"VehicleGeographicHudManager",
		"HudCreated",
		"GEO HUD CREATED"
	);

	const FText ResolveFailedText = NSLOCTEXT(
		"VehicleGeographicHudManager",
		"ResolveFailed",
		"Vehicle or GeoReferencingSystem not found"
	);

	const FText LocationUnavailableText = NSLOCTEXT(
		"VehicleGeographicHudManager",
		"LocationUnavailable",
		"Vehicle geographic location unavailable"
	);

	const FText VehicleNotFoundText = NSLOCTEXT(
		"VehicleGeographicHudManager",
		"VehicleNotFound",
		"Vehicle not found"
	);

	const FText GeoReferencingSystemNotFoundText = NSLOCTEXT(
		"VehicleGeographicHudManager",
		"GeoReferencingSystemNotFound",
		"GeoReferencingSystem not found"
	);

	void ShowScreenMessage(const FColor& Color, const TCHAR* Message)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				ScreenMessageKey,
				ScreenMessageDurationSeconds,
				Color,
				Message
			);
		}
	}

	FString FormatDecimal(const double Value, const int32 DecimalPlaces)
	{
		return FString::Printf(TEXT("%.*f"), DecimalPlaces, Value);
	}
}

AVehicleGeographicHudManager::AVehicleGeographicHudManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVehicleGeographicHudManager::BeginPlay()
{
	Super::BeginPlay();

	ResolveGeoReferencingSystem();
	if (bStartAutomatically)
	{
		if (InitialStartDelaySeconds > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
				InitialStartTimerHandle,
				this,
				&AVehicleGeographicHudManager::DelayedStartHud,
				InitialStartDelaySeconds,
				false
			);
		}
		else
		{
			StartHud();
		}
	}
}

void AVehicleGeographicHudManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopHud();

	Super::EndPlay(EndPlayReason);
}

bool AVehicleGeographicHudManager::StartHud()
{
	ShowScreenMessage(FColor::Cyan, TEXT("VehicleGeographicHudManager StartHud"));

	UE_LOG(LogVehicleGeographicHudManager, Log, TEXT("[VehicleGeographicHudManager] StartHud called."));

	if (bIsHudRunning)
	{
		return true;
	}

	CreateHudWidget();

	StopResolveRetry();
	ResolveAttemptCount = 0;

	const bool bHasTarget = ResolveTargetVehicle();
	const bool bHasGeoRef = ResolveGeoReferencingSystem();

	if (!bHasTarget || !bHasGeoRef)
	{
		SetHudText(WaitingForVehicleLocationText);

		if (bRetryUntilTargetFound)
		{
			StartResolveRetry();
		}

		return false;
	}

	bIsHudRunning = true;

	StartUpdateTimer();
	UpdateHudText();

	if (bLogDebug)
	{
		UE_LOG(LogVehicleGeographicHudManager, Log, TEXT("[VehicleGeographicHudManager] HUD started. Target=%s, GeoRef=%s"), *GetNameSafe(TargetVehicle.Get()), *GetNameSafe(GeoReferencingSystem.Get()));
	}

	return true;
}

void AVehicleGeographicHudManager::StopHud()
{
	StopResolveRetry();
	StopUpdateTimer();

	bIsHudRunning = false;

	DestroyHudWidget();

	if (bLogDebug)
	{
		UE_LOG(LogVehicleGeographicHudManager, Log, TEXT("[VehicleGeographicHudManager] HUD stopped."));
	}
}

void AVehicleGeographicHudManager::SetTargetVehicle(AActor* NewTargetVehicle, bool bStartHudNow)
{
	if (!IsValid(NewTargetVehicle))
	{
		UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] Invalid target vehicle."));
		return;
	}

	TargetVehicle = NewTargetVehicle;

	StopResolveRetry();

	if (bLogDebug)
	{
		UE_LOG(LogVehicleGeographicHudManager, Log, TEXT("[VehicleGeographicHudManager] Target vehicle set: %s"), *GetNameSafe(TargetVehicle.Get()));
	}

	if (bStartHudNow)
	{
		if (bIsHudRunning)
		{
			StopUpdateTimer();
			bIsHudRunning = false;
		}

		StartHud();
	}
}

bool AVehicleGeographicHudManager::ResolveTargetVehicle()
{
	if (IsValid(TargetVehicle))
	{
		return true;
	}

	switch (TargetResolveMode)
	{
	case EVehicleGeographicTargetResolveMode::ExplicitActor:
		break;

	case EVehicleGeographicTargetResolveMode::PlayerPawn:
		TargetVehicle = UGameplayStatics::GetPlayerPawn(this, 0);
		break;

	case EVehicleGeographicTargetResolveMode::ActorClass:
		if (TargetVehicleClass.Get())
		{
			TargetVehicle = UGameplayStatics::GetActorOfClass(this, TargetVehicleClass);
		}
		break;

	case EVehicleGeographicTargetResolveMode::ActorTag:
		if (!TargetVehicleTag.IsNone())
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(this, TargetVehicleTag, FoundActors);

			for (AActor* Candidate : FoundActors)
			{
				if (!IsValid(Candidate))
				{
					continue;
				}

				if (TargetVehicleClass.Get() && !Candidate->IsA(TargetVehicleClass.Get()))
				{
					continue;
				}

				TargetVehicle = Candidate;
				break;
			}
		}
		break;

	default:
		break;
	}

	if (!IsValid(TargetVehicle))
	{
		if (bLogDebug)
		{
			UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] Target vehicle not found."));
		}

		return false;
	}

	return true;
}

bool AVehicleGeographicHudManager::ResolveGeoReferencingSystem()
{
	if (IsValid(GeoReferencingSystem))
	{
		return true;
	}

	GeoReferencingSystem = AGeoReferencingSystem::GetGeoReferencingSystem(this);

	if (!IsValid(GeoReferencingSystem))
	{
		if (bLogDebug)
		{
			UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] GeoReferencingSystem not found."));
		}

		return false;
	}

	return true;
}

bool AVehicleGeographicHudManager::CreateHudWidget()
{
	if (HudWidget && HudWidget->IsInViewport())
	{
		return true;
	}

	if (!HudWidgetClass)
	{
		UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] HudWidgetClass is not assigned."));

		ShowScreenMessage(FColor::Red, TEXT("Geo HUD Error: HudWidgetClass is None"));

		return false;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] PlayerController not found."));

		ShowScreenMessage(FColor::Red, TEXT("Geo HUD Error: PlayerController not found"));

		return false;
	}

	HudWidget = CreateWidget<UVehicleGeographicHudWidget>(PlayerController, HudWidgetClass);

	if (!HudWidget)
	{
		UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] Failed to create HUD widget."));

		ShowScreenMessage(FColor::Red, TEXT("Geo HUD Error: CreateWidget failed"));

		return false;
	}

	HudWidget->AddToViewport(HudZOrder);

	SetHudText(HudCreatedText);

	ShowScreenMessage(FColor::Green, TEXT("Geo HUD Created"));

	UE_LOG(LogVehicleGeographicHudManager, Log, TEXT("[VehicleGeographicHudManager] HUD widget created. Class=%s"), *GetNameSafe(HudWidgetClass.Get()));

	return true;
}

void AVehicleGeographicHudManager::DestroyHudWidget()
{
	if (HudWidget)
	{
		HudWidget->RemoveFromParent();
		HudWidget = nullptr;
	}
}

void AVehicleGeographicHudManager::SetHudText(const FText& NewText) const
{
	if (HudWidget)
	{
		HudWidget->SetGeographicText(NewText);
	}
}

void AVehicleGeographicHudManager::StartUpdateTimer()
{
	StopUpdateTimer();

	const float SafeRateHz = FMath::Max(UpdateRateHz, MinimumUpdateRateHz);
	const float IntervalSeconds = 1.0f / SafeRateHz;

	GetWorldTimerManager().SetTimer(
		UpdateTimerHandle,
		this,
		&AVehicleGeographicHudManager::UpdateHudText,
		IntervalSeconds,
		true
	);
}

void AVehicleGeographicHudManager::StopUpdateTimer()
{
	GetWorldTimerManager().ClearTimer(UpdateTimerHandle);
}

void AVehicleGeographicHudManager::StartResolveRetry()
{
	if (GetWorldTimerManager().IsTimerActive(ResolveRetryTimerHandle))
	{
		return;
	}

	const float SafeInterval = FMath::Max(ResolveRetryIntervalSeconds, MinimumResolveRetryIntervalSeconds);

	GetWorldTimerManager().SetTimer(
		ResolveRetryTimerHandle,
		this,
		&AVehicleGeographicHudManager::HandleResolveRetry,
		SafeInterval,
		true
	);
}

void AVehicleGeographicHudManager::StopResolveRetry()
{
	GetWorldTimerManager().ClearTimer(ResolveRetryTimerHandle);
}

void AVehicleGeographicHudManager::HandleResolveRetry()
{
	ResolveAttemptCount++;

	const bool bHasTarget = ResolveTargetVehicle();
	const bool bHasGeoRef = ResolveGeoReferencingSystem();

	if (bHasTarget && bHasGeoRef)
	{
		StopResolveRetry();

		bIsHudRunning = true;
		StartUpdateTimer();
		UpdateHudText();

		if (bLogDebug)
		{
			UE_LOG(LogVehicleGeographicHudManager, Log, TEXT("[VehicleGeographicHudManager] Target and GeoRef resolved after %d attempts."), ResolveAttemptCount);
		}

		return;
	}

	if (MaxResolveAttempts > 0 && ResolveAttemptCount >= MaxResolveAttempts)
	{
		StopResolveRetry();

		SetHudText(ResolveFailedText);

		UE_LOG(LogVehicleGeographicHudManager, Warning, TEXT("[VehicleGeographicHudManager] Resolve failed after %d attempts."), ResolveAttemptCount);
	}
}

void AVehicleGeographicHudManager::UpdateHudText()
{
	if (!CreateHudWidget())
	{
		return;
	}

	if (!IsValid(TargetVehicle) || !ResolveGeoReferencingSystem())
	{
		SetHudText(LocationUnavailableText);
		return;
	}

	SetHudText(BuildGeographicText());
}

FText AVehicleGeographicHudManager::BuildGeographicText() const
{
	if (!IsValid(TargetVehicle))
	{
		return VehicleNotFoundText;
	}

	if (!IsValid(GeoReferencingSystem))
	{
		return GeoReferencingSystemNotFoundText;
	}

	FGeographicCoordinates Geographic;
	GeoReferencingSystem->EngineToGeographic(
		TargetVehicle->GetActorLocation(),
		Geographic
	);

	const int32 CoordDecimals = FMath::Clamp(CoordinateDecimalPlaces, 0, MaxCoordinateDecimalPlaces);
	const int32 AltDecimals = FMath::Clamp(AltitudeDecimalPlaces, 0, MaxAltitudeDecimalPlaces);

	const FString LatString = FormatDecimal(Geographic.Latitude, CoordDecimals);
	const FString LonString = FormatDecimal(Geographic.Longitude, CoordDecimals);
	const FString AltString = FormatDecimal(Geographic.Altitude, AltDecimals);

	FString Text;

	if (bUseLabelFormat)
	{
		Text = FString::Printf(TEXT("Lat %s\u00B0   Lon %s\u00B0   Alt %sm"), *LatString, *LonString, *AltString);
	}
	else
	{
		Text = FString::Printf(TEXT("(%s\u00B0, %s\u00B0, %sm)"), *LatString, *LonString, *AltString);
	}

	return FText::FromString(Text);
}

void AVehicleGeographicHudManager::DelayedStartHud()
{
	StartHud();
}
