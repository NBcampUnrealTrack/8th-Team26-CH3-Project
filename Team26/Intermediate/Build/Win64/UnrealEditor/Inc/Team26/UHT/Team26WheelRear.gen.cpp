// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26WheelRear.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26WheelRear() {}

// Begin Cross Module References
CHAOSVEHICLES_API UClass* Z_Construct_UClass_UChaosVehicleWheel();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelRear();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelRear_NoRegister();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26WheelRear
void UTeam26WheelRear::StaticRegisterNativesUTeam26WheelRear()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26WheelRear);
UClass* Z_Construct_UClass_UTeam26WheelRear_NoRegister()
{
	return UTeam26WheelRear::StaticClass();
}
struct Z_Construct_UClass_UTeam26WheelRear_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Base rear wheel definition.\n */" },
#endif
		{ "IncludePath", "Team26WheelRear.h" },
		{ "ModuleRelativePath", "Team26WheelRear.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base rear wheel definition." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26WheelRear>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTeam26WheelRear_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UChaosVehicleWheel,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26WheelRear_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26WheelRear_Statics::ClassParams = {
	&UTeam26WheelRear::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x008000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26WheelRear_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26WheelRear_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26WheelRear()
{
	if (!Z_Registration_Info_UClass_UTeam26WheelRear.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26WheelRear.OuterSingleton, Z_Construct_UClass_UTeam26WheelRear_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26WheelRear.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26WheelRear>()
{
	return UTeam26WheelRear::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26WheelRear);
UTeam26WheelRear::~UTeam26WheelRear() {}
// End Class UTeam26WheelRear

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelRear_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26WheelRear, UTeam26WheelRear::StaticClass, TEXT("UTeam26WheelRear"), &Z_Registration_Info_UClass_UTeam26WheelRear, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26WheelRear), 1051437445U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelRear_h_3703635321(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelRear_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelRear_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
