// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26SportsWheelFront.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26SportsWheelFront() {}

// Begin Cross Module References
TEAM26_API UClass* Z_Construct_UClass_UTeam26SportsWheelFront();
TEAM26_API UClass* Z_Construct_UClass_UTeam26SportsWheelFront_NoRegister();
TEAM26_API UClass* Z_Construct_UClass_UTeam26WheelFront();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26SportsWheelFront
void UTeam26SportsWheelFront::StaticRegisterNativesUTeam26SportsWheelFront()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26SportsWheelFront);
UClass* Z_Construct_UClass_UTeam26SportsWheelFront_NoRegister()
{
	return UTeam26SportsWheelFront::StaticClass();
}
struct Z_Construct_UClass_UTeam26SportsWheelFront_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Front wheel definition for Sports Car.\n */" },
#endif
		{ "IncludePath", "Team26SportsWheelFront.h" },
		{ "ModuleRelativePath", "Team26SportsWheelFront.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Front wheel definition for Sports Car." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26SportsWheelFront>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UTeam26SportsWheelFront_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UTeam26WheelFront,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26SportsWheelFront_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26SportsWheelFront_Statics::ClassParams = {
	&UTeam26SportsWheelFront::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26SportsWheelFront_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26SportsWheelFront_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26SportsWheelFront()
{
	if (!Z_Registration_Info_UClass_UTeam26SportsWheelFront.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26SportsWheelFront.OuterSingleton, Z_Construct_UClass_UTeam26SportsWheelFront_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26SportsWheelFront.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26SportsWheelFront>()
{
	return UTeam26SportsWheelFront::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26SportsWheelFront);
UTeam26SportsWheelFront::~UTeam26SportsWheelFront() {}
// End Class UTeam26SportsWheelFront

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelFront_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26SportsWheelFront, UTeam26SportsWheelFront::StaticClass, TEXT("UTeam26SportsWheelFront"), &Z_Registration_Info_UClass_UTeam26SportsWheelFront, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26SportsWheelFront), 2846550390U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelFront_h_1681974755(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelFront_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsWheelFront_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
