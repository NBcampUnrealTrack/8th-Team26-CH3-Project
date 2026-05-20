#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ReplayGameInstanceSubsystem.generated.h"

UCLASS()
class TEAM26_API UReplayGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Replay")
	void SetSelectedReplayPath(const FString& InPath)
	{
		SelectedReplayPath = InPath;
	}

	UFUNCTION(BlueprintPure, Category="Replay")
	const FString& GetSelectedReplayPath() const
	{
		return SelectedReplayPath;
	}

	UFUNCTION(BlueprintCallable, Category="Replay")
	void ClearSelectedReplayPath()
	{
		SelectedReplayPath.Reset();
	}

	UFUNCTION(BlueprintCallable, Category="Replay")
	void SetLastRecordedReplayPath(const FString& InPath)
	{
		LastRecordedReplayPath = InPath;
	}

	UFUNCTION(BlueprintPure, Category="Replay")
	const FString& GetLastRecordedReplayPath() const
	{
		return LastRecordedReplayPath;
	}

	UFUNCTION(BlueprintCallable, Category="Replay")
	void ClearLastRecordedReplayPath()
	{
		LastRecordedReplayPath.Reset();
	}

private:
	UPROPERTY()
	FString SelectedReplayPath;

	UPROPERTY()
	FString LastRecordedReplayPath;
};
