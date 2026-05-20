#include "Replay/ReplayFileLibrarys.h"

#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
	constexpr int32 RequiredReplayCsvColumnCount = 9;

	static bool SplitCsvLine(const FString& Line, TArray<FString>& OutColumns)
	{
		OutColumns.Reset();
		Line.ParseIntoArray(OutColumns, TEXT(","), true);
		return OutColumns.Num() >= RequiredReplayCsvColumnCount;
	}

	static FString FormatDuration(const float Seconds)
	{ 
		const float ClampedSeconds = FMath::Max(0.0f, Seconds);
		const int32 TotalSeconds = FMath::FloorToInt(ClampedSeconds);
		const int32 Minutes = TotalSeconds / 60;
		const int32 RemainSeconds = TotalSeconds % 60;
		const int32 Millis = FMath::Clamp(
			FMath::RoundToInt((ClampedSeconds - TotalSeconds) * 1000.0f),
			0,
			999
		);

		return FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, RemainSeconds, Millis);
	}

	static bool ReadReplaySummary(const FString& FilePath, float& OutDurationSeconds, int32& OutSampleCount)
	{
		OutDurationSeconds = 0.0f;
		OutSampleCount = 0;

		TArray<FString> Lines;
		if (!FFileHelper::LoadFileToStringArray(Lines, *FilePath))
		{
			return false;
		}

		bool bFoundValidSample = false;

		for (int32 Index = 1; Index < Lines.Num(); ++Index)
		{
			const FString& Line = Lines[Index];
			if (Line.TrimStartAndEnd().IsEmpty())
			{
				continue;
			}

			TArray<FString> Columns;
			if (!SplitCsvLine(Line, Columns))
			{
				continue;
			}

			++OutSampleCount;
			OutDurationSeconds = FCString::Atof(*Columns[0]);
			bFoundValidSample = true;
		}

		return bFoundValidSample;
	}

	static FString MakeReplayDisplayName(const FString& FilePath)
	{
		FString DisplayName = FPaths::GetBaseFilename(FilePath);
		DisplayName.RemoveFromStart(TEXT("AgentData-"));
		return FString::Printf(TEXT("Replay - %s"), *DisplayName);
	}

	static bool BuildReplayFileInfo(
		const FString& FilePath,
		FReplayFileInfo& OutInfo,
		const bool bRequireSamples
	)
	{
		OutInfo = FReplayFileInfo();

		if (!FPaths::FileExists(FilePath))
		{
			return false;
		}

		const FString FullPath = FPaths::ConvertRelativePathToFull(FilePath);
		float Duration = 0.0f;
		int32 SampleCount = 0;
		const bool bHasSamples = ReadReplaySummary(FullPath, Duration, SampleCount);

		if (bRequireSamples && (!bHasSamples || SampleCount <= 0))
		{
			return false;
		}

		const FFileStatData Stat = IFileManager::Get().GetStatData(*FullPath);

		OutInfo.FilePath = FullPath;
		OutInfo.DisplayName = MakeReplayDisplayName(FullPath);
		OutInfo.DurationSeconds = Duration;
		OutInfo.DurationText = FormatDuration(Duration);
		OutInfo.SampleCount = SampleCount;

		if (Stat.bIsValid)
		{
			OutInfo.DateText = Stat.ModificationTime.ToString(TEXT("%Y-%m-%d"));
			OutInfo.TimeText = Stat.ModificationTime.ToString(TEXT("%H:%M"));
			OutInfo.ModifiedTicks = Stat.ModificationTime.GetTicks();
		}
		else
		{
			OutInfo.DateText = TEXT("-");
			OutInfo.TimeText = TEXT("-");
			OutInfo.ModifiedTicks = 0;
		}

		return true;
	}

	static bool IsPathInsideDirectory(const FString& FilePath, const FString& Directory)
	{
		FString NormalizedFilePath = FPaths::ConvertRelativePathToFull(FilePath);
		FString NormalizedDirectory = FPaths::ConvertRelativePathToFull(Directory);

		FPaths::NormalizeFilename(NormalizedFilePath);
		FPaths::NormalizeDirectoryName(NormalizedDirectory);

		if (!NormalizedDirectory.EndsWith(TEXT("/")))
		{
			NormalizedDirectory += TEXT("/");
		}

		return NormalizedFilePath.StartsWith(NormalizedDirectory, ESearchCase::IgnoreCase);
	}
}

FString UReplayFileLibrary::GetReplayOutputDirectory()
{
	
	return FPaths::Combine(FPaths::ProjectDir(), TEXT("Output"));
}

void UReplayFileLibrary::FindTrajectoryReplays(TArray<FReplayFileInfo>& OutReplays)
{
	OutReplays.Reset();

	const FString OutputDir = GetReplayOutputDirectory();
	const FString SearchPattern = FPaths::Combine(OutputDir, TEXT("*.csv"));

	TArray<FString> FileNames;
	IFileManager::Get().FindFiles(FileNames, *SearchPattern, true, false);

	for (const FString& FileName : FileNames)
	{
		const FString FullPath = FPaths::ConvertRelativePathToFull(
			FPaths::Combine(OutputDir, FileName)
		);

		FReplayFileInfo Info;
		if (BuildReplayFileInfo(FullPath, Info, false))
		{
			OutReplays.Add(Info);
		}
	}

	OutReplays.Sort([](const FReplayFileInfo& A, const FReplayFileInfo& B)
	{
		return A.ModifiedTicks > B.ModifiedTicks;
	});
}

bool UReplayFileLibrary::TryGetReplayFileInfo(const FString& FilePath, FReplayFileInfo& OutReplay)
{
	return BuildReplayFileInfo(FilePath, OutReplay, true);
}

bool UReplayFileLibrary::DeleteTrajectoryReplay(const FString& FilePath)
{
	if (!FPaths::FileExists(FilePath) || !FPaths::GetExtension(FilePath).Equals(TEXT("csv"), ESearchCase::IgnoreCase))
	{
		return false;
	}

	const FString FullPath = FPaths::ConvertRelativePathToFull(FilePath);
	if (!IsPathInsideDirectory(FullPath, GetReplayOutputDirectory()))
	{
		return false;
	}

	return IFileManager::Get().Delete(*FullPath);
}
