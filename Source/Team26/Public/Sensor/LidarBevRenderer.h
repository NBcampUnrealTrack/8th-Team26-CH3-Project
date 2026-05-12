// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Team26CameraSensorTypes.h"
#include "LidarBevRenderer.generated.h"


class UTexture2D;

UCLASS()
class TEAM26_API ULidarBevRenderer : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(const FBevRenderConfig& InConfig);
	void RenderPointCloud(const FLidarPointCloudData& PointCloud, const FTransform& SensorTransform);
	UTexture2D* GetRenderTarget() const {return DynamicTexture;}
	void UpdateConfig(const FBevRenderConfig& InConfig);
	
private:
	void CreateTexture();
	void BuildColorLut();
	
	UPROPERTY()
	TObjectPtr<UTexture2D> DynamicTexture;
	
	TArray<FColor> PixelBuffer;
	FUpdateTextureRegion2D UpdateRegion;
	FBevRenderConfig Config;
	FColor ColorLut[256];
};
