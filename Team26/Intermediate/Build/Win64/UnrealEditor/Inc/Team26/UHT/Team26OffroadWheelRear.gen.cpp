// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26OffroadWheelRear.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26OffroadWheelRear() {}

// Begin Cross Module References
TEAM26_API UClass* Z_Construct_UClass_UTeam26OffroadWheelRear();
TEAM26_API UClass* Z_Construct_UClass_UTeam26OffroadWheelRear_NoRegister();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelRear();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26OffroadWheelRear
void UTeam26OffroadWheelRear::StaticRegisterNativesUTeam26OffroadWheelRear()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26OffroadWheelRear);
UClass* Z_Construct_UClass_UTeam26OffroadWheelRear_NoRegister()
{
	return UTeam26OffroadWheelRear::StaticClass();
}
struct Z_Construct_UClass_UTeam26OffroadWheelRear_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Rear wheel definition for Offroad Car.\n */" },
#endif
		{ "IncludePath", "Team26OffroadWheelRear.h" },
		{ "ModuleRelativePath", "Team26OffroadWheelRear.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Rear wheel definition for Offroad Car." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26OffroadWheelRear>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTeam26OffroadWheelRear_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UTeam26WheelRear,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26OffroadWheelRear_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26OffroadWheelRear_Statics::ClassParams = {
	&UTeam26OffroadWheelRear::StaticClass,
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
	0x009000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26OffroadWheelRear_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26OffroadWheelRear_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26OffroadWheelRear()
{
	if (!Z_Registration_Info_UClass_UTeam26OffroadWheelRear.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26OffroadWheelRear.OuterSingleton, Z_Construct_UClass_UTeam26OffroadWheelRear_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26OffroadWheelRear.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26OffroadWheelRear>()
{
	return UTeam26OffroadWheelRear::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26OffroadWheelRear);
UTeam26OffroadWheelRear::~UTeam26OffroadWheelRear() {}
// End Class UTeam26OffroadWheelRear

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelRear_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26OffroadWheelRear, UTeam26OffroadWheelRear::StaticClass, TEXT("UTeam26OffroadWheelRear"), &Z_Registration_Info_UClass_UTeam26OffroadWheelRear, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26OffroadWheelRear), 1816793450U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelRear_h_2046446578(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelRear_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelRear_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
