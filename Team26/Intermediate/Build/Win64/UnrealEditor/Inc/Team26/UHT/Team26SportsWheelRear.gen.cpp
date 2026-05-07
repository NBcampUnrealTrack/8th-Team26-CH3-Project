// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26SportsWheelRear.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26SportsWheelRear() {}

// Begin Cross Module References
TEAM26_API UClass* Z_Construct_UClass_UTeam26SportsWheelRear();
TEAM26_API UClass* Z_Construct_UClass_UTeam26SportsWheelRear_NoRegister();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelRear();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26SportsWheelRear
void UTeam26SportsWheelRear::StaticRegisterNativesUTeam26SportsWheelRear()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26SportsWheelRear);
UClass* Z_Construct_UClass_UTeam26SportsWheelRear_NoRegister()
{
	return UTeam26SportsWheelRear::StaticClass();
}
struct Z_Construct_UClass_UTeam26SportsWheelRear_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Rear wheel definition for Sports Car.\n */" },
#endif
		{ "IncludePath", "Team26SportsWheelRear.h" },
		{ "ModuleRelativePath", "Team26SportsWheelRear.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Rear wheel definition for Sports Car." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26SportsWheelRear>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTeam26SportsWheelRear_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UTeam26WheelRear,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26SportsWheelRear_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26SportsWheelRear_Statics::ClassParams = {
	&UTeam26SportsWheelRear::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26SportsWheelRear_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26SportsWheelRear_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26SportsWheelRear()
{
	if (!Z_Registration_Info_UClass_UTeam26SportsWheelRear.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26SportsWheelRear.OuterSingleton, Z_Construct_UClass_UTeam26SportsWheelRear_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26SportsWheelRear.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26SportsWheelRear>()
{
	return UTeam26SportsWheelRear::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26SportsWheelRear);
UTeam26SportsWheelRear::~UTeam26SportsWheelRear() {}
// End Class UTeam26SportsWheelRear

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelRear_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26SportsWheelRear, UTeam26SportsWheelRear::StaticClass, TEXT("UTeam26SportsWheelRear"), &Z_Registration_Info_UClass_UTeam26SportsWheelRear, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26SportsWheelRear), 3370119310U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelRear_h_601115706(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelRear_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelRear_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
