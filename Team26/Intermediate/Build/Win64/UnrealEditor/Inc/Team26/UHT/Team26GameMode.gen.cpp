// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26GameMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26GameMode() {}

// Begin Cross Module References
ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
TEAM26_API UClass* Z_Construct_UClass_ATeam26GameMode();
TEAM26_API UClass* Z_Construct_UClass_ATeam26GameMode_NoRegister();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class ATeam26GameMode
void ATeam26GameMode::StaticRegisterNativesATeam26GameMode()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATeam26GameMode);
UClass* Z_Construct_UClass_ATeam26GameMode_NoRegister()
{
	return ATeam26GameMode::StaticClass();
}
struct Z_Construct_UClass_ATeam26GameMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering HLOD WorldPartition DataLayers Transformation" },
		{ "IncludePath", "Team26GameMode.h" },
		{ "ModuleRelativePath", "Team26GameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATeam26GameMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ATeam26GameMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_AGameModeBase,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATeam26GameMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATeam26GameMode_Statics::ClassParams = {
	&ATeam26GameMode::StaticClass,
	"Game",
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008802ACu,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATeam26GameMode_Statics::Class_MetaDataParams), Z_Construct_UClass_ATeam26GameMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATeam26GameMode()
{
	if (!Z_Registration_Info_UClass_ATeam26GameMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATeam26GameMode.OuterSingleton, Z_Construct_UClass_ATeam26GameMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATeam26GameMode.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<ATeam26GameMode>()
{
	return ATeam26GameMode::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATeam26GameMode);
ATeam26GameMode::~ATeam26GameMode() {}
// End Class ATeam26GameMode

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26GameMode_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATeam26GameMode, ATeam26GameMode::StaticClass, TEXT("ATeam26GameMode"), &Z_Registration_Info_UClass_ATeam26GameMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATeam26GameMode), 3267731786U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26GameMode_h_796985793(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26GameMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26GameMode_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
