// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26OffroadWheelFront.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26OffroadWheelFront() {}

// Begin Cross Module References
TEAM26_API UClass* Z_Construct_UClass_UTeam26OffroadWheelFront();
TEAM26_API UClass* Z_Construct_UClass_UTeam26OffroadWheelFront_NoRegister();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelFront();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26OffroadWheelFront
void UTeam26OffroadWheelFront::StaticRegisterNativesUTeam26OffroadWheelFront()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26OffroadWheelFront);
UClass* Z_Construct_UClass_UTeam26OffroadWheelFront_NoRegister()
{
	return UTeam26OffroadWheelFront::StaticClass();
}
struct Z_Construct_UClass_UTeam26OffroadWheelFront_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Front wheel definition for Offroad Car.\n */" },
#endif
		{ "IncludePath", "Team26OffroadWheelFront.h" },
		{ "ModuleRelativePath", "Team26OffroadWheelFront.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Front wheel definition for Offroad Car." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26OffroadWheelFront>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTeam26OffroadWheelFront_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UTeam26WheelFront,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26OffroadWheelFront_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26OffroadWheelFront_Statics::ClassParams = {
	&UTeam26OffroadWheelFront::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26OffroadWheelFront_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26OffroadWheelFront_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26OffroadWheelFront()
{
	if (!Z_Registration_Info_UClass_UTeam26OffroadWheelFront.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26OffroadWheelFront.OuterSingleton, Z_Construct_UClass_UTeam26OffroadWheelFront_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26OffroadWheelFront.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26OffroadWheelFront>()
{
	return UTeam26OffroadWheelFront::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26OffroadWheelFront);
UTeam26OffroadWheelFront::~UTeam26OffroadWheelFront() {}
// End Class UTeam26OffroadWheelFront

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelFront_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26OffroadWheelFront, UTeam26OffroadWheelFront::StaticClass, TEXT("UTeam26OffroadWheelFront"), &Z_Registration_Info_UClass_UTeam26OffroadWheelFront, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26OffroadWheelFront), 452466246U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelFront_h_2046760439(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelFront_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26OffroadWheelFront_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
