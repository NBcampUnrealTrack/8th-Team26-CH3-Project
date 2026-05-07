// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26SportsCar.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26SportsCar() {}

// Begin Cross Module References
TEAM26_API UClass* Z_Construct_UClass_ATeam26Pawn();
TEAM26_API UClass* Z_Construct_UClass_ATeam26SportsCar();
TEAM26_API UClass* Z_Construct_UClass_ATeam26SportsCar_NoRegister();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class ATeam26SportsCar
void ATeam26SportsCar::StaticRegisterNativesATeam26SportsCar()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(ATeam26SportsCar);
UClass* Z_Construct_UClass_ATeam26SportsCar_NoRegister()
{
	return ATeam26SportsCar::StaticClass();
}
struct Z_Construct_UClass_ATeam26SportsCar_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Sports car wheeled vehicle implementation\n */" },
#endif
		{ "HideCategories", "Navigation" },
		{ "IncludePath", "Team26SportsCar.h" },
		{ "ModuleRelativePath", "Team26SportsCar.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Sports car wheeled vehicle implementation" },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<ATeam26SportsCar>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_ATeam26SportsCar_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_ATeam26Pawn,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_ATeam26SportsCar_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_ATeam26SportsCar_Statics::ClassParams = {
	&ATeam26SportsCar::StaticClass,
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
	0x009000A5u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_ATeam26SportsCar_Statics::Class_MetaDataParams), Z_Construct_UClass_ATeam26SportsCar_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_ATeam26SportsCar()
{
	if (!Z_Registration_Info_UClass_ATeam26SportsCar.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_ATeam26SportsCar.OuterSingleton, Z_Construct_UClass_ATeam26SportsCar_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_ATeam26SportsCar.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<ATeam26SportsCar>()
{
	return ATeam26SportsCar::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(ATeam26SportsCar);
ATeam26SportsCar::~ATeam26SportsCar() {}
// End Class ATeam26SportsCar

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsCar_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_ATeam26SportsCar, ATeam26SportsCar::StaticClass, TEXT("ATeam26SportsCar"), &Z_Registration_Info_UClass_ATeam26SportsCar, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(ATeam26SportsCar), 43476655U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsCar_h_2036357819(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsCar_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26SportsCar_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
