// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Serialization/Archive.h"
#include "DataLogger.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM26_API UDataLogger : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDataLogger();


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	static int32 GetUtmZone(double Longitude);
	static void LatLonToUtm(double Lat, double Lon, int32 Zone, double& OutEasting, double&  OutNorthing);
	void WorldToUtm(const FVector& WorldLocation, double& OutEasting, double& OutNorthing) const;
	bool CreateCsvFile();
	void CloseCsvFile();
	void AppendRow();
	bool WriteCsvLine(const FString& Line);
	bool IsCurrentLevelBlocked() const;
	void PublishLastRecordedReplayPath() const;

	UFUNCTION(BlueprintCallable, Category="Data Logger")
	void StartRecording();

	UFUNCTION(BlueprintCallable, Category="Data Logger")
	void StopRecording();

	UFUNCTION(BlueprintPure, Category="Data Logger")
	bool IsRecording() const { return bIsRecording; }
	
private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data Logger",
		meta=(AllowPrivateAccess="true"))
	bool bEnableLogging = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data Logger",
		meta=(ClampMin="0.1", ClampMax="100.0", Units="Hz", AllowPrivateAccess="true"))
	float SaveFrequencyHz = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data Logger",
		meta=(AllowPrivateAccess="true"))
	TArray<FName> AutoLoggingDisabledLevels = { TEXT("L_Replay") };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data Logger|UTM Reference",
		meta=(ClampMin="-90.0", ClampMax="90.0", Units="deg", AllowPrivateAccess="true"))
	double OriginLatitude = 37.575931;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Data Logger|UTM Reference",
		meta=(ClampMin="-180.0", ClampMax="180.0", Units="deg", AllowPrivateAccess="true"))
	double OriginLongitude = 126.973500;

private:
	double OriginUtmEasting = 0.0;
	double OriginUtmNorthing = 0.0;
	int32 OriginUtmZone = 0;

	FString CsvFilePath;
	TUniquePtr<FArchive> CsvArchive;
	bool bIsRecording = false;
	int32 WrittenSampleCount = 0;
	float TimeSinceLastSave = 0.0f;
	float ElapsedRecordingTime = 0.0f;

};
