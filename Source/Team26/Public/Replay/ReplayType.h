#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ReplayType.generated.h"

USTRUCT(BlueprintType)
struct FReplayFileInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	FString FilePath;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	FString DateText;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	FString TimeText;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	FString DurationText;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	float DurationSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	int32 SampleCount = 0;

	UPROPERTY(BlueprintReadOnly, Category="Replay")
	int64 ModifiedTicks = 0;
};

UCLASS(BlueprintType)
class TEAM26_API UReplayEntryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Replay")
	FReplayFileInfo Info;
};