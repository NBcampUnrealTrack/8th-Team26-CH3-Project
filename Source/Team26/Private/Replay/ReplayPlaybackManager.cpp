#include "Replay/ReplayPlaybackManager.h"

#include "Components/PrimitiveComponent.h"
#include "DataLogger/VehicleGeographicHudManager.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Paths.h"
#include "Replay/ReplayGameInstanceSubsystem.h"
#include "Replay/TrajectoryReplayComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogReplayPlaybackManager, Log, All);

namespace
{
	constexpr int32 ReplayScreenMessageKey = -1;
	constexpr float ReplayScreenMessageDurationSeconds = 5.0f;

	FString NormalizeReplayPath(const FString& Path)
	{
		FString NormalizedPath = Path.TrimStartAndEnd();
		if (NormalizedPath.IsEmpty())
		{
			return FString();
		}

		FPaths::NormalizeFilename(NormalizedPath);
		return FPaths::ConvertRelativePathToFull(NormalizedPath);
	}

	bool IsEnabledReplayModeValue(const FString& Value)
	{
		const FString NormalizedValue = Value.TrimStartAndEnd();

		return
			NormalizedValue.IsEmpty() ||
			NormalizedValue == TEXT("1") ||
			NormalizedValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
			NormalizedValue.Equals(TEXT("yes"), ESearchCase::IgnoreCase) ||
			NormalizedValue.Equals(TEXT("on"), ESearchCase::IgnoreCase);
	}
}

AReplayPlaybackManager::AReplayPlaybackManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AReplayPlaybackManager::BeginPlay()
{
	Super::BeginPlay();

	if (bOnlyRunInReplayMode && !IsReplayModeRequested())
	{
		UE_LOG(LogReplayPlaybackManager, Log, TEXT("ReplayMode was not requested. Manager disabled."));
		return;
	}

	ScreenMsg(TEXT("ReplayPlaybackManager BeginPlay - ReplayMode"), FColor::Cyan);

	if (!EnsureReplayVehicleActor())
	{
		return;
	}

	ScreenMsg(
		FString::Printf(TEXT("ReplayVehicleActor OK: %s"), *GetNameSafe(ReplayVehicleActor)),
		FColor::Green
	);

	PrepareReplayActor();
	ConnectGeoHudToReplayVehicle();

	const FString CsvPath = ResolveReplayCsvPath();

	if (CsvPath.IsEmpty())
	{
		ScreenMsg(TEXT("ERROR: CSV path is empty."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("CSV path is empty."));
		return;
	}

	if (!FPaths::FileExists(CsvPath))
	{
		ScreenMsg(FString::Printf(TEXT("ERROR: CSV file not found: %s"), *CsvPath), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("CSV file not found: %s"), *CsvPath);
		return;
	}

	ScreenMsg(FString::Printf(TEXT("Replay CSV Path: %s"), *CsvPath), FColor::Green);

	ReplayComponent = FindOrCreateReplayComponent();
	if (!ReplayComponent)
	{
		ScreenMsg(TEXT("ERROR: TrajectoryReplayComponent is not available."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("TrajectoryReplayComponent is not available."));
		return;
	}

	ReplayComponent->SetTargetActor(ReplayVehicleActor);

	if (!ReplayComponent->LoadReplayFromCsv(CsvPath))
	{
		ScreenMsg(TEXT("ERROR: LoadReplayFromCsv failed."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("LoadReplayFromCsv failed: %s"), *CsvPath);
		return;
	}

	ScreenMsg(
		FString::Printf(TEXT("Replay Loaded. Duration: %.2f sec"), ReplayComponent->GetDurationSeconds()),
		FColor::Green
	);

	if (bAutoPlay)
	{
		ReplayComponent->Play();
		ScreenMsg(TEXT("Replay AutoPlay Start"), FColor::Green);
	}
}

void AReplayPlaybackManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(SpawnedReplayVehicleActor))
	{
		SpawnedReplayVehicleActor->Destroy();
		SpawnedReplayVehicleActor = nullptr;
		ReplayVehicleActor = nullptr;
		ReplayComponent = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

bool AReplayPlaybackManager::IsReplayModeRequested() const
{
	const UWorld* World = GetWorld();
	const FString OptionName = ReplayModeOptionName.TrimStartAndEnd();

	if (!World || OptionName.IsEmpty())
	{
		return false;
	}

	if (!World->URL.HasOption(*OptionName))
	{
		return false;
	}

	const FString Value = World->URL.GetOption(*(OptionName + TEXT("=")), TEXT(""));
	return IsEnabledReplayModeValue(Value);
}

bool AReplayPlaybackManager::EnsureReplayVehicleActor()
{
	if (!bSpawnReplayVehicleAtRuntime)
	{
		if (IsValid(ReplayVehicleActor))
		{
			return true;
		}

		ScreenMsg(TEXT("ERROR: ReplayVehicleActor is not assigned."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("ReplayVehicleActor is not assigned."));
		return false;
	}

	if (IsValid(SpawnedReplayVehicleActor))
	{
		ReplayVehicleActor = SpawnedReplayVehicleActor;
		return true;
	}

	if (!ReplayVehicleClass)
	{
		ScreenMsg(TEXT("ERROR: ReplayVehicleClass is not assigned."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("ReplayVehicleClass is not assigned."));
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		ScreenMsg(TEXT("ERROR: World is null."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("World is null."));
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedReplayVehicleActor = World->SpawnActor<AActor>(
		ReplayVehicleClass.Get(),
		ReplayVehicleSpawnTransform,
		SpawnParams
	);

	ReplayVehicleActor = SpawnedReplayVehicleActor;

	if (!IsValid(ReplayVehicleActor))
	{
		ScreenMsg(TEXT("ERROR: Failed to spawn ReplayVehicleActor."), FColor::Red);
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("Failed to spawn ReplayVehicleActor from class %s."), *GetNameSafe(ReplayVehicleClass.Get()));
		return false;
	}

	ScreenMsg(
		FString::Printf(TEXT("Spawned ReplayVehicle: %s"), *GetNameSafe(ReplayVehicleActor)),
		FColor::Green
	);

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (bPossessReplayPawn)
		{
			if (APawn* ReplayPawn = Cast<APawn>(ReplayVehicleActor))
			{
				PC->Possess(ReplayPawn);
				ScreenMsg(TEXT("PlayerController possessed ReplayVehicle"), FColor::Green);
			}
		}

		if (bSetCameraToReplayVehicle)
		{
			PC->SetViewTargetWithBlend(ReplayVehicleActor, 0.0f);
		}
	}

	return true;
}

UTrajectoryReplayComponent* AReplayPlaybackManager::FindOrCreateReplayComponent()
{
	if (!IsValid(ReplayVehicleActor))
	{
		return nullptr;
	}

	if (IsValid(ReplayComponent) && ReplayComponent->GetOwner() == ReplayVehicleActor)
	{
		return ReplayComponent;
	}

	if (UTrajectoryReplayComponent* ExistingComponent = ReplayVehicleActor->FindComponentByClass<UTrajectoryReplayComponent>())
	{
		return ExistingComponent;
	}

	UTrajectoryReplayComponent* NewReplayComponent = NewObject<UTrajectoryReplayComponent>(
		ReplayVehicleActor,
		UTrajectoryReplayComponent::StaticClass(),
		TEXT("TrajectoryReplayComponent")
	);

	if (!NewReplayComponent)
	{
		return nullptr;
	}

	NewReplayComponent->CreationMethod = EComponentCreationMethod::Instance;
	ReplayVehicleActor->AddInstanceComponent(NewReplayComponent);
	NewReplayComponent->RegisterComponent();

	ScreenMsg(TEXT("TrajectoryReplayComponent created dynamically"), FColor::Cyan);
	UE_LOG(LogReplayPlaybackManager, Log, TEXT("Created TrajectoryReplayComponent on %s."), *GetNameSafe(ReplayVehicleActor));

	return NewReplayComponent;
}

void AReplayPlaybackManager::PrepareReplayActor()
{
	if (!IsValid(ReplayVehicleActor))
	{
		return;
	}

	ReplayVehicleActor->SetActorTickEnabled(false);

	if (!bDisablePhysicsAndCollision)
	{
		return;
	}

	ReplayVehicleActor->SetActorEnableCollision(false);

	TArray<UMovementComponent*> MovementComponents;
	ReplayVehicleActor->GetComponents<UMovementComponent>(MovementComponents);

	for (UMovementComponent* Movement : MovementComponents)
	{
		if (!Movement)
		{
			continue;
		}

		Movement->StopMovementImmediately();
		Movement->Deactivate();
		Movement->SetComponentTickEnabled(false);
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;
	ReplayVehicleActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (!Primitive)
		{
			continue;
		}

		Primitive->SetMobility(EComponentMobility::Movable);

		if (Primitive->IsSimulatingPhysics())
		{
			Primitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Primitive->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			Primitive->SetSimulatePhysics(false);
		}

		Primitive->SetEnableGravity(false);
		Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Primitive->SetGenerateOverlapEvents(false);
	}
}

void AReplayPlaybackManager::ConnectGeoHudToReplayVehicle()
{
	if (!IsValid(ReplayVehicleActor) || !GetWorld())
	{
		return;
	}

	int32 ConnectedHudCount = 0;

	for (TActorIterator<AVehicleGeographicHudManager> It(GetWorld()); It; ++It)
	{
		AVehicleGeographicHudManager* HudManager = *It;

		if (!IsValid(HudManager))
		{
			continue;
		}

		HudManager->SetTargetVehicle(ReplayVehicleActor, true);
		++ConnectedHudCount;
	}

	if (ConnectedHudCount > 0)
	{
		ScreenMsg(TEXT("Geo HUD target connected to ReplayVehicle"), FColor::Cyan);
	}
}

FString AReplayPlaybackManager::ResolveReplayCsvPath() const
{
	const UReplayGameInstanceSubsystem* ReplaySubsystem =
		GetGameInstance() ? GetGameInstance()->GetSubsystem<UReplayGameInstanceSubsystem>() : nullptr;

	if (ReplaySubsystem)
	{
		const FString SelectedPath = NormalizeReplayPath(ReplaySubsystem->GetSelectedReplayPath());

		if (!SelectedPath.IsEmpty() && FPaths::FileExists(SelectedPath))
		{
			return SelectedPath;
		}

		const FString LastRecordedPath = NormalizeReplayPath(ReplaySubsystem->GetLastRecordedReplayPath());

		if (!LastRecordedPath.IsEmpty() && FPaths::FileExists(LastRecordedPath))
		{
			return LastRecordedPath;
		}
	}

	const FString TestPath = NormalizeReplayPath(TestReplayCsvPath);

	if (!TestPath.IsEmpty() && FPaths::FileExists(TestPath))
	{
		return TestPath;
	}

	if (bUseLatestCsvIfSelectedPathIsEmpty)
	{
		return FindLatestReplayCsv();
	}

	return FString();
}

FString AReplayPlaybackManager::FindLatestReplayCsv() const
{
	const FString OutputDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Output"));
	const FString SearchPattern = FPaths::Combine(OutputDir, TEXT("*.csv"));

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *SearchPattern, true, false);

	FString LatestPath;
	FDateTime LatestTime = FDateTime::MinValue();

	for (const FString& FileName : FileNames)
	{
		const FString FullPath = FPaths::ConvertRelativePathToFull(
			FPaths::Combine(OutputDir, FileName)
		);

		const FFileStatData StatData = IFileManager::Get().GetStatData(*FullPath);

		if (StatData.bIsValid && StatData.ModificationTime > LatestTime)
		{
			LatestTime = StatData.ModificationTime;
			LatestPath = FullPath;
		}
	}

	return LatestPath;
}

void AReplayPlaybackManager::ScreenMsg(const FString& Message, const FColor& Color) const
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			ReplayScreenMessageKey,
			ReplayScreenMessageDurationSeconds,
			Color,
			Message
		);
	}
}
