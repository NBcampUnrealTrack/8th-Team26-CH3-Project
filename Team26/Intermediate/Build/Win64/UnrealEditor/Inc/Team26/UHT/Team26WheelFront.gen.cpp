// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26WheelFront.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26WheelFront() {}

// Begin Cross Module References
CHAOSVEHICLES_API UClass* Z_Construct_UClass_UChaosVehicleWheel();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelFront();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelFront_NoRegister();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26WheelFront
void UTeam26WheelFront::StaticRegisterNativesUTeam26WheelFront()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26WheelFront);
UClass* Z_Construct_UClass_UTeam26WheelFront_NoRegister()
{
	return UTeam26WheelFront::StaticClass();
}
struct Z_Construct_UClass_UTeam26WheelFront_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Base front wheel definition.\n */" },
#endif
		{ "IncludePath", "Team26WheelFront.h" },
		{ "ModuleRelativePath", "Team26WheelFront.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Base front wheel definition." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26WheelFront>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTeam26WheelFront_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UChaosVehicleWheel,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26WheelFront_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26WheelFront_Statics::ClassParams = {
	&UTeam26WheelFront::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26WheelFront_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26WheelFront_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26WheelFront()
{
	if (!Z_Registration_Info_UClass_UTeam26WheelFront.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26WheelFront.OuterSingleton, Z_Construct_UClass_UTeam26WheelFront_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26WheelFront.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26WheelFront>()
{
	return UTeam26WheelFront::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26WheelFront);
UTeam26WheelFront::~UTeam26WheelFront() {}
// End Class UTeam26WheelFront

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelFront_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26WheelFront, UTeam26WheelFront::StaticClass, TEXT("UTeam26WheelFront"), &Z_Registration_Info_UClass_UTeam26WheelFront, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26WheelFront), 3430462438U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelFront_h_3085924214(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelFront_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26WheelFront_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
