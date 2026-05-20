#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrajectoryReplayComponent.generated.h"

USTRUCT()
struct FTrajectoryReplaySample
{
	GENERATED_BODY()

	float TimeSeconds = 0.0f;
	FVector Location = FVector::ZeroVector;
	double UtmEasting = 0.0;
	double UtmNorthing = 0.0;
	int32 UtmZone = 0;
	float SpeedKmh = 0.0f;
	float Yaw = 0.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAM26_API UTrajectoryReplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTrajectoryReplayComponent();

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	bool LoadReplayFromCsv(const FString& CsvFilePath);

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	void Play();

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	void Pause();

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	void Stop();

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	void Seek(float NewTimeSeconds);

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	void SetPlaybackRate(float NewPlaybackRate);

	UFUNCTION(BlueprintCallable, Category="Trajectory Replay")
	void SetTargetActor(AActor* NewTargetActor);

	UFUNCTION(BlueprintPure, Category="Trajectory Replay")
	float GetCurrentTimeSeconds() const { return CurrentTimeSeconds; }

	UFUNCTION(BlueprintPure, Category="Trajectory Replay")
	float GetDurationSeconds() const { return DurationSeconds; }

	UFUNCTION(BlueprintPure, Category="Trajectory Replay")
	bool IsPlaying() const { return bIsPlaying; }

	UFUNCTION(BlueprintPure, Category="Trajectory Replay")
	float GetPlaybackRate() const { return PlaybackRate; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	bool ParseCsvLineToSample(const FString& Line, FTrajectoryReplaySample& OutSample) const;
	void ApplySampleAtCurrentTime();
	int32 FindSampleIndexForTime(float TimeSeconds) const;

private:
	UPROPERTY(EditAnywhere, Category="Trajectory Replay")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category="Trajectory Replay", meta=(ClampMin="0.1", ClampMax="8.0"))
	float PlaybackRate = 1.0f;

	TArray<FTrajectoryReplaySample> Samples;

	int32 CurrentSampleIndex = 0;
	float CurrentTimeSeconds = 0.0f;
	float DurationSeconds = 0.0f;
	bool bIsPlaying = false;
};