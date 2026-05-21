#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Replay/ReplayType.h"
#include "ReplayFileLibrarys.generated.h"

UCLASS()
class TEAM26_API UReplayFileLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Replay")
	static void FindTrajectoryReplays(TArray<FReplayFileInfo>& OutReplays);

	UFUNCTION(BlueprintCallable, Category="Replay")
	static bool TryGetReplayFileInfo(const FString& FilePath, FReplayFileInfo& OutReplay);

	UFUNCTION(BlueprintCallable, Category="Replay")
	static bool DeleteTrajectoryReplay(const FString& FilePath);

	UFUNCTION(BlueprintPure, Category="Replay")
	static FString GetReplayOutputDirectory();
};
