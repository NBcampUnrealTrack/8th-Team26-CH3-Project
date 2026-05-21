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

private:
	void PrepareReplayActor();
	FString ResolveReplayCsvPath() const;
	FString FindLatestReplayCsv() const;

private:
	UPROPERTY(EditAnywhere, Category="Replay")
	TObjectPtr<AActor> ReplayVehicleActor = nullptr;

	UPROPERTY(EditAnywhere, Category="Replay")
	bool bAutoPlay = true;

	UPROPERTY(EditAnywhere, Category="Replay")
	bool bDisablePhysicsAndCollision = true;

	// L_Replay를 직접 실행해도 최신 CSV를 자동으로 읽기 위한 옵션
	UPROPERTY(EditAnywhere, Category="Replay|Debug")
	bool bUseLatestCsvIfSelectedPathIsEmpty = false;

	// 직접 CSV 경로를 넣고 테스트하고 싶을 때 사용
	UPROPERTY(EditAnywhere, Category="Replay|Debug")
	FString TestReplayCsvPath;

	UPROPERTY()
	TObjectPtr<UTrajectoryReplayComponent> ReplayComponent = nullptr;
};
