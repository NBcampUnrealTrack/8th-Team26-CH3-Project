#include "Replay/ReplayPlaybackManager.h"

#include "Components/PrimitiveComponent.h"
#include "GameFramework/MovementComponent.h"
#include "Misc/Paths.h"
#include "Replay/ReplayFileLibrarys.h"
#include "Replay/ReplayGameInstanceSubsystem.h"
#include "Replay/TrajectoryReplayComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogReplayPlaybackManager, Log, All);

AReplayPlaybackManager::AReplayPlaybackManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AReplayPlaybackManager::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(ReplayVehicleActor))
	{
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("ReplayVehicleActor is not assigned."));
		return;
	}

	PrepareReplayActor();

	const FString CsvPath = ResolveReplayCsvPath();

	if (CsvPath.IsEmpty())
	{
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("CSV path is empty."));
		return;
	}

	if (!FPaths::FileExists(CsvPath))
	{
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("CSV file not found: %s"), *CsvPath);
		return;
	}

	UE_LOG(LogReplayPlaybackManager, Log, TEXT("Replay CSV Path: %s"), *CsvPath);

	ReplayComponent = ReplayVehicleActor->FindComponentByClass<UTrajectoryReplayComponent>();

	if (!ReplayComponent)
	{
		ReplayComponent = NewObject<UTrajectoryReplayComponent>(
			ReplayVehicleActor,
			UTrajectoryReplayComponent::StaticClass(),
			TEXT("TrajectoryReplayComponent")
		);

		if (!ReplayComponent)
		{
			UE_LOG(LogReplayPlaybackManager, Error, TEXT("Failed to create TrajectoryReplayComponent."));
			return;
		}

		ReplayVehicleActor->AddInstanceComponent(ReplayComponent);
		ReplayComponent->RegisterComponent();
	}

	ReplayComponent->SetTargetActor(ReplayVehicleActor);

	const bool bLoaded = ReplayComponent->LoadReplayFromCsv(CsvPath);

	if (!bLoaded)
	{
		UE_LOG(LogReplayPlaybackManager, Error, TEXT("LoadReplayFromCsv failed."));
		return;
	}

	UE_LOG(LogReplayPlaybackManager, Log, TEXT("Replay loaded. Duration: %.2f sec"), ReplayComponent->GetDurationSeconds() );

	if (bAutoPlay)
	{
		ReplayComponent->Play();
	}
	else
	{
		UE_LOG(LogReplayPlaybackManager, Log, TEXT("Replay loaded but AutoPlay is false."));
	}
}

void AReplayPlaybackManager::PrepareReplayActor()
{
	if (!IsValid(ReplayVehicleActor))
	{
		return;
	}

	// 차량 Pawn 자체 Tick이 자율주행/센서/차량 로직을 계속 돌릴 수 있으므로 꺼준다.
	ReplayVehicleActor->SetActorTickEnabled(false);

	if (!bDisablePhysicsAndCollision)
	{
		return;
	}

	ReplayVehicleActor->SetActorEnableCollision(false);

	// Chaos Vehicle Movement 같은 MovementComponent를 정지.
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

		UE_LOG(LogReplayPlaybackManager, Verbose, TEXT("Movement disabled: %s"), *GetNameSafe(Movement));
	}

	// Mesh / Collision / Physics 정지.
	TArray<UPrimitiveComponent*> PrimitiveComponents;
	ReplayVehicleActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	for (UPrimitiveComponent* Primitive : PrimitiveComponents)
	{
		if (!Primitive)
		{
			continue;
		}

		Primitive->SetMobility(EComponentMobility::Movable);
		Primitive->SetSimulatePhysics(false);
		Primitive->SetEnableGravity(false);
		Primitive->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Primitive->SetGenerateOverlapEvents(false);

		UE_LOG(LogReplayPlaybackManager, Verbose, TEXT("Primitive prepared: %s"), *GetNameSafe(Primitive));
	}
}

FString AReplayPlaybackManager::ResolveReplayCsvPath() const
{
	auto TryUsePath = [](const FString& Path, const TCHAR* SourceName) -> FString
	{
		if (Path.IsEmpty())
		{
			return FString();
		}

		if (FPaths::FileExists(Path))
		{
			UE_LOG(LogReplayPlaybackManager, Log, TEXT("Using %s replay path: %s"), SourceName, *Path);
			return Path;
		}

		UE_LOG(LogReplayPlaybackManager, Warning, TEXT("Ignoring missing %s replay path: %s"), SourceName, *Path);
		return FString();
	};

	const UReplayGameInstanceSubsystem* ReplaySubsystem =
		GetGameInstance() ? GetGameInstance()->GetSubsystem<UReplayGameInstanceSubsystem>() : nullptr;

	if (ReplaySubsystem)
	{
		const FString SelectedPath = TryUsePath(ReplaySubsystem->GetSelectedReplayPath(), TEXT("selected"));
		if (!SelectedPath.IsEmpty())
		{
			return SelectedPath;
		}

		const FString LastRecordedPath = TryUsePath(ReplaySubsystem->GetLastRecordedReplayPath(), TEXT("last recorded"));
		if (!LastRecordedPath.IsEmpty())
		{
			return LastRecordedPath;
		}
	}

	const FString TestPath = TryUsePath(TestReplayCsvPath, TEXT("test"));
	if (!TestPath.IsEmpty())
	{
		return TestPath;
	}

	if (bUseLatestCsvIfSelectedPathIsEmpty)
	{
		const FString LatestCsv = FindLatestReplayCsv();

		if (!LatestCsv.IsEmpty())
		{
			UE_LOG(LogReplayPlaybackManager, Log, TEXT("Using latest CSV: %s"), *LatestCsv);
			return LatestCsv;
		}
	}

	return FString();
}

FString AReplayPlaybackManager::FindLatestReplayCsv() const
{
	TArray<FReplayFileInfo> Replays;
	UReplayFileLibrary::FindTrajectoryReplays(Replays);

	for (const FReplayFileInfo& Replay : Replays)
	{
		if (Replay.SampleCount > 0 && FPaths::FileExists(Replay.FilePath))
		{
			return Replay.FilePath;
		}
	}

	return Replays.Num() > 0 ? Replays[0].FilePath : FString();
}
