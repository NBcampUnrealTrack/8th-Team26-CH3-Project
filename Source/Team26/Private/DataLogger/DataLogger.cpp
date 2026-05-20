#include "DataLogger/DataLogger.h"
#include "Containers/StringConv.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "Replay/ReplayFileLibrarys.h"
#include "Replay/ReplayGameInstanceSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogDataLogger, Log, All);

UDataLogger::UDataLogger()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDataLogger::BeginPlay()
{
	Super::BeginPlay();
	OriginUtmZone = GetUtmZone(OriginLongitude);
	LatLonToUtm(OriginLatitude, OriginLongitude, OriginUtmZone, OriginUtmEasting, OriginUtmNorthing);

	if (bEnableLogging)
	{
		StartRecording();
	}
}

void UDataLogger::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	StopRecording();
	Super::EndPlay(EndPlayReason);
}

void UDataLogger::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsRecording)
	{
		return;
	}

	ElapsedRecordingTime += DeltaTime;
	TimeSinceLastSave += DeltaTime;

	const float SaveInterval = 1.0f / FMath::Max(SaveFrequencyHz, 0.1f);
	if (TimeSinceLastSave >= SaveInterval)
	{
		AppendRow();
		TimeSinceLastSave -= SaveInterval;
	}
}

void UDataLogger::StartRecording()
{
	if (bIsRecording)
	{
		return;
	}

	if (IsCurrentLevelBlocked())
	{
		UE_LOG(LogDataLogger, Log, TEXT("Data logging is disabled on this level."));
		return;
	}

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UReplayGameInstanceSubsystem* ReplaySubsystem = GameInstance->GetSubsystem<UReplayGameInstanceSubsystem>())
			{
				ReplaySubsystem->ClearLastRecordedReplayPath();
			}
		}
	}

	if (!CreateCsvFile())
	{
		return;
	}

	bIsRecording = true;
	SetComponentTickEnabled(true);
	TimeSinceLastSave = 0.0f;
	ElapsedRecordingTime = 0.0f;
	WrittenSampleCount = 0;
}

void UDataLogger::StopRecording()
{
	if (!bIsRecording && !CsvArchive)
	{
		return;
	}

	bIsRecording = false;
	SetComponentTickEnabled(false);
	CloseCsvFile();
	PublishLastRecordedReplayPath();
}

bool UDataLogger::CreateCsvFile()
{
	CloseCsvFile();

	const FString OutputDir = UReplayFileLibrary::GetReplayOutputDirectory();
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.CreateDirectoryTree(*OutputDir))
	{
		UE_LOG(LogDataLogger, Error, TEXT("Failed to create output directory: %s"), *OutputDir);
		return false;
	}

	const FDateTime Now = FDateTime::Now();
	const FString FileName = FString::Printf(
		TEXT("AgentData-%04d_%02d-%02d-%02d-%02d-%02d.csv"),
		Now.GetYear(), Now.GetMonth(), Now.GetDay(),
		Now.GetHour(), Now.GetMinute(), Now.GetSecond()
	);

	CsvFilePath = FPaths::Combine(OutputDir, FileName);

	const FString Header =
		TEXT("Timestamp,World_X,World_Y,World_Z,UTM_Easting,UTM_Northing,UTM_Zone,Velocity_kmh,Yaw\n"
	);

	CsvArchive.Reset(IFileManager::Get().CreateFileWriter(*CsvFilePath));
	if (!CsvArchive)
	{
		UE_LOG(LogDataLogger, Error, TEXT("Failed to open csv for writing: %s"), *CsvFilePath);
		CsvFilePath.Reset();
		return false;
	}

	if (!WriteCsvLine(Header))
	{
		CloseCsvFile();
		CsvFilePath.Reset();
		return false;
	}

	UE_LOG(LogDataLogger, Log, TEXT("Recording to: %s (%.1f Hz)"), *CsvFilePath, SaveFrequencyHz);
	return true;
}

void UDataLogger::CloseCsvFile()
{
	if (!CsvArchive)
	{
		return;
	}

	CsvArchive->Flush();
	CsvArchive.Reset();
}

void UDataLogger::AppendRow()
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const FVector WorldLoc = Owner->GetActorLocation();        // cm
	const FRotator WorldRot = Owner->GetActorRotation();
	const FVector Velocity = Owner->GetVelocity();             // cm/s

	const double SpeedKmh = Velocity.Size() * 0.01 * 3.6;
	const double Yaw = WorldRot.Yaw;

	double UtmEasting = 0.0;
	double UtmNorthing = 0.0;
	WorldToUtm(WorldLoc, UtmEasting, UtmNorthing);

	const FString Row = FString::Printf(
		TEXT("%.3f,%.2f,%.2f,%.2f,%.4f,%.4f,%d,%.2f,%.4f\n"),
		ElapsedRecordingTime,
		WorldLoc.X, WorldLoc.Y, WorldLoc.Z,
		UtmEasting, UtmNorthing, OriginUtmZone,
		SpeedKmh,
		Yaw
	);

	if (!WriteCsvLine(Row))
	{
		UE_LOG(LogDataLogger, Error, TEXT("Stopping recording after csv write failure: %s"), *CsvFilePath);
		StopRecording();
		return;
	}

	++WrittenSampleCount;
}

bool UDataLogger::WriteCsvLine(const FString& Line)
{
	if (!CsvArchive)
	{
		return false;
	}

	FTCHARToUTF8 ConvertedLine(*Line);
	CsvArchive->Serialize(const_cast<ANSICHAR*>(ConvertedLine.Get()), ConvertedLine.Length());
	CsvArchive->Flush();
	return !CsvArchive->IsError();
}

bool UDataLogger::IsCurrentLevelBlocked() const
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const FString MapName = World->GetMapName();
	for (const FName& DisabledLevel : AutoLoggingDisabledLevels)
	{
		if (!DisabledLevel.IsNone() && MapName.EndsWith(DisabledLevel.ToString()))
		{
			return true;
		}
	}

	return false;
}

void UDataLogger::PublishLastRecordedReplayPath() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return;
	}

	UReplayGameInstanceSubsystem* ReplaySubsystem = GameInstance->GetSubsystem<UReplayGameInstanceSubsystem>();
	if (!ReplaySubsystem)
	{
		return;
	}

	if (WrittenSampleCount > 0 && !CsvFilePath.IsEmpty() && FPaths::FileExists(CsvFilePath))
	{
		ReplaySubsystem->SetLastRecordedReplayPath(CsvFilePath);
		UE_LOG(LogDataLogger, Log, TEXT("Last recorded replay path: %s"), *CsvFilePath);
		return;
	}

	ReplaySubsystem->ClearLastRecordedReplayPath();
}

void UDataLogger::WorldToUtm(const FVector& WorldLocation,
	double& OutEasting, double& OutNorthing) const
{
	const double OffsetEastM  =  WorldLocation.X * 0.01;
	const double OffsetNorthM = -WorldLocation.Y * 0.01;

	OutEasting  = OriginUtmEasting  + OffsetEastM;
	OutNorthing = OriginUtmNorthing + OffsetNorthM;
}

int32 UDataLogger::GetUtmZone(double Longitude)
{
	return FMath::FloorToInt((Longitude + 180.0) / 6.0) + 1;
}

void UDataLogger::LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double& OutNorthing)
{
	// WGS-84 ellipsoid constants
	constexpr double a  = 6378137.0;            // semi-major axis (m)
	constexpr double f  = 1.0 / 298.257223563;  // flattening
	constexpr double k0 = 0.9996;               // UTM scale factor

	const double e2 = 2.0 * f - f * f;          // first eccentricity squared
	const double ep2 = e2 / (1.0 - e2);         // second eccentricity squared

	const double LatRad = FMath::DegreesToRadians(Lat);
	const double CentralMeridian = (Zone - 1) * 6.0 - 180.0 + 3.0;
	const double DeltaLon = FMath::DegreesToRadians(Lon - CentralMeridian);

	const double SinLat = FMath::Sin(LatRad);
	const double CosLat = FMath::Cos(LatRad);
	const double TanLat = FMath::Tan(LatRad);

	const double N = a / FMath::Sqrt(1.0 - e2 * SinLat * SinLat);
	const double T = TanLat * TanLat;
	const double C = ep2 * CosLat * CosLat;
	const double A = CosLat * DeltaLon;

	// Meridional arc (M) — series expansion
	const double e4 = e2 * e2;
	const double e6 = e4 * e2;
	const double M = a * (
		(1.0 - e2 / 4.0 - 3.0 * e4 / 64.0  - 5.0 * e6 / 256.0) * LatRad
		- (3.0 * e2 / 8.0 + 3.0 * e4 / 32.0 + 45.0 * e6 / 1024.0) * FMath::Sin(2.0 * LatRad)
		+ (15.0 * e4 / 256.0 + 45.0 * e6 / 1024.0) * FMath::Sin(4.0 * LatRad)
		- (35.0 * e6 / 3072.0) * FMath::Sin(6.0 * LatRad));

	const double A2 = A * A;
	const double A4 = A2 * A2;
	const double A6 = A4 * A2;

	OutEasting = k0 * N * (
		A
		+ (1.0 - T + C) * A2 * A / 6.0
		+ (5.0 - 18.0 * T + T * T + 72.0 * C - 58.0 * ep2) * A4 * A / 120.0
	) + 500000.0;   // false easting

	OutNorthing = k0 * (M + N * TanLat * (
		A2 / 2.0
		+ (5.0 - T + 9.0 * C + 4.0 * C * C) * A4 / 24.0
		+ (61.0 - 58.0 * T + T * T + 600.0 * C - 330.0 * ep2) * A6 / 720.0
	));

	// Southern hemisphere offset
	if (Lat < 0.0)
		OutNorthing += 10000000.0;
}
