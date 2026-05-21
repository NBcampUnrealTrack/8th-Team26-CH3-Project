#include "Replay/TrajectoryReplayComponent.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogTrajectoryReplay, Log, All);

UTrajectoryReplayComponent::UTrajectoryReplayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTrajectoryReplayComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetActor)
	{
		TargetActor = GetOwner();
	}
}

bool UTrajectoryReplayComponent::LoadReplayFromCsv(const FString& CsvFilePath)
{
	Stop();

	Samples.Reset();
	CurrentSampleIndex = 0;
	CurrentTimeSeconds = 0.0f;
	DurationSeconds = 0.0f;

	if (!FPaths::FileExists(CsvFilePath))
	{
		UE_LOG(LogTrajectoryReplay, Warning, TEXT("File not found: %s"), *CsvFilePath);
		return false;
	}

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *CsvFilePath))
	{
		UE_LOG(LogTrajectoryReplay, Warning, TEXT("Failed to load csv: %s"), *CsvFilePath);
		return false;
	}

	Samples.Reserve(FMath::Max(0, Lines.Num() - 1));

	// 0번 줄은 Header:
	// Timestamp,World_X,World_Y,World_Z,UTM_Easting,UTM_Northing,UTM_Zone,Velocity_kmh,Yaw
	for (int32 Index = 1; Index < Lines.Num(); ++Index)
	{
		const FString Line = Lines[Index].TrimStartAndEnd();
		if (Line.IsEmpty())
		{
			continue;
		}

		FTrajectoryReplaySample Sample;
		if (ParseCsvLineToSample(Line, Sample))
		{
			Samples.Add(Sample);
		}
	}

	if (Samples.Num() < 2)
	{
		UE_LOG(LogTrajectoryReplay, Warning, TEXT("Not enough samples: %d"), Samples.Num());
		return false;
	}

	DurationSeconds = Samples.Last().TimeSeconds;
	ApplySampleAtCurrentTime();

	UE_LOG(
		LogTrajectoryReplay,
		Log,
		TEXT("Loaded %d samples. Duration %.3f sec"),
		Samples.Num(),
		DurationSeconds
	);

	return true;
}

bool UTrajectoryReplayComponent::ParseCsvLineToSample(
	const FString& Line,
	FTrajectoryReplaySample& OutSample
) const
{
	TArray<FString> Columns;
	Line.ParseIntoArray(Columns, TEXT(","), true);

	if (Columns.Num() < 9)
	{
		return false;
	}

	OutSample.TimeSeconds = FCString::Atof(*Columns[0]);

	OutSample.Location.X = FCString::Atod(*Columns[1]);
	OutSample.Location.Y = FCString::Atod(*Columns[2]);
	OutSample.Location.Z = FCString::Atod(*Columns[3]);

	OutSample.UtmEasting = FCString::Atod(*Columns[4]);
	OutSample.UtmNorthing = FCString::Atod(*Columns[5]);
	OutSample.UtmZone = FCString::Atoi(*Columns[6]);
	OutSample.SpeedKmh = FCString::Atof(*Columns[7]);
	OutSample.Yaw = FCString::Atof(*Columns[8]);

	return true;
}

void UTrajectoryReplayComponent::Play()
{
	if (Samples.Num() < 2)
	{
		return;
	}

	bIsPlaying = true;
	SetComponentTickEnabled(true);
}

void UTrajectoryReplayComponent::Pause()
{
	bIsPlaying = false;
	SetComponentTickEnabled(false);
}

void UTrajectoryReplayComponent::Stop()
{
	bIsPlaying = false;
	SetComponentTickEnabled(false);

	CurrentTimeSeconds = 0.0f;
	CurrentSampleIndex = 0;

	if (Samples.Num() > 0)
	{
		ApplySampleAtCurrentTime();
	}
}

void UTrajectoryReplayComponent::Seek(float NewTimeSeconds)
{
	if (Samples.Num() < 2)
	{
		return;
	}

	CurrentTimeSeconds = FMath::Clamp(NewTimeSeconds, 0.0f, DurationSeconds);
	CurrentSampleIndex = FindSampleIndexForTime(CurrentTimeSeconds);
	ApplySampleAtCurrentTime();
}

void UTrajectoryReplayComponent::SetPlaybackRate(float NewPlaybackRate)
{
	PlaybackRate = FMath::Clamp(NewPlaybackRate, 0.1f, 8.0f);
}

void UTrajectoryReplayComponent::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

void UTrajectoryReplayComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsPlaying || Samples.Num() < 2)
	{
		return;
	}

	CurrentTimeSeconds += DeltaTime * PlaybackRate;

	if (CurrentTimeSeconds >= DurationSeconds)
	{
		CurrentTimeSeconds = DurationSeconds;
		CurrentSampleIndex = FindSampleIndexForTime(CurrentTimeSeconds);
		ApplySampleAtCurrentTime();
		Pause();
		return;
	}

	while (
		CurrentSampleIndex < Samples.Num() - 2 &&
		Samples[CurrentSampleIndex + 1].TimeSeconds < CurrentTimeSeconds
	)
	{
		++CurrentSampleIndex;
	}

	ApplySampleAtCurrentTime();
}

int32 UTrajectoryReplayComponent::FindSampleIndexForTime(float TimeSeconds) const
{
	if (Samples.Num() < 2)
	{
		return 0;
	}

	int32 Low = 0;
	int32 High = Samples.Num() - 1;

	while (Low < High)
	{
		const int32 Mid = (Low + High) / 2;

		if (Samples[Mid].TimeSeconds < TimeSeconds)
		{
			Low = Mid + 1;
		}
		else
		{
			High = Mid;
		}
	}

	return FMath::Clamp(Low - 1, 0, Samples.Num() - 2);
}

void UTrajectoryReplayComponent::ApplySampleAtCurrentTime()
{
	if (!IsValid(TargetActor) || Samples.Num() == 0)
	{
		return;
	}

	if (Samples.Num() == 1)
	{
		const FTrajectoryReplaySample& Only = Samples[0];
		TargetActor->SetActorLocationAndRotation(
			Only.Location,
			FRotator(0.0f, Only.Yaw, 0.0f),
			false,
			nullptr,
			ETeleportType::TeleportPhysics
		);
		return;
	}

	const int32 AIndex = FMath::Clamp(CurrentSampleIndex, 0, Samples.Num() - 2);
	const int32 BIndex = AIndex + 1;

	const FTrajectoryReplaySample& A = Samples[AIndex];
	const FTrajectoryReplaySample& B = Samples[BIndex];

	const float SegmentDuration = FMath::Max(B.TimeSeconds - A.TimeSeconds, KINDA_SMALL_NUMBER);
	const float Alpha = FMath::Clamp(
		(CurrentTimeSeconds - A.TimeSeconds) / SegmentDuration,
		0.0f,
		1.0f
	);

	const FVector Location = FMath::Lerp(A.Location, B.Location, Alpha);

	// Yaw는 359도 → 1도 같은 케이스가 있어서 일반 Lerp 대신 각도 차이로 보간.
	const float DeltaYaw = FMath::FindDeltaAngleDegrees(A.Yaw, B.Yaw);
	const float Yaw = A.Yaw + DeltaYaw * Alpha;

	TargetActor->SetActorLocationAndRotation(
		Location,
		FRotator(0.0f, Yaw, 0.0f),
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);
}
