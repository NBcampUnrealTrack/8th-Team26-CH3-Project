#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReplayPlaybackManager.generated.h"

class UTrajectoryReplayComponent;

UCLASS()
class TEAM26_API AReplayPlaybackManager : public AActor
{
	GENERATED_BODY()

public:
	AReplayPlaybackManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	bool IsReplayModeRequested() const;
	bool EnsureReplayVehicleActor();
	UTrajectoryReplayComponent* FindOrCreateReplayComponent();

	void PrepareReplayActor();
	void ConnectGeoHudToReplayVehicle();

	FString ResolveReplayCsvPath() const;
	FString FindLatestReplayCsv() const;

	void ScreenMsg(const FString& Message, const FColor& Color = FColor::Yellow) const;

private:
	UPROPERTY(EditAnywhere, Category="Replay|Mode")
	bool bOnlyRunInReplayMode = true;

	UPROPERTY(EditAnywhere, Category="Replay|Mode")
	FString ReplayModeOptionName = TEXT("ReplayMode");

	UPROPERTY(EditAnywhere, Category="Replay|Vehicle")
	bool bSpawnReplayVehicleAtRuntime = true;

	UPROPERTY(EditAnywhere, Category="Replay|Vehicle")
	TObjectPtr<AActor> ReplayVehicleActor = nullptr;

	UPROPERTY(EditAnywhere, Category="Replay|Vehicle")
	TSubclassOf<AActor> ReplayVehicleClass = nullptr;

	UPROPERTY(EditAnywhere, Category="Replay|Vehicle")
	FTransform ReplayVehicleSpawnTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, Category="Replay|Vehicle")
	bool bPossessReplayPawn = true;

	UPROPERTY(EditAnywhere, Category="Replay|Vehicle")
	bool bSetCameraToReplayVehicle = true;

	UPROPERTY(EditAnywhere, Category="Replay")
	bool bAutoPlay = true;

	UPROPERTY(EditAnywhere, Category="Replay")
	bool bDisablePhysicsAndCollision = true;

	UPROPERTY(EditAnywhere, Category="Replay|Debug")
	bool bUseLatestCsvIfSelectedPathIsEmpty = false;

	UPROPERTY(EditAnywhere, Category="Replay|Debug")
	FString TestReplayCsvPath;

	UPROPERTY()
	TObjectPtr<UTrajectoryReplayComponent> ReplayComponent = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> SpawnedReplayVehicleActor = nullptr;
};
