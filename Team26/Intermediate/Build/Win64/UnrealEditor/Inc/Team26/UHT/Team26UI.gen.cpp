// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Team26/Team26UI.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTeam26UI() {}

// Begin Cross Module References
TEAM26_API UClass* Z_Construct_UClass_UTeam26UI();
TEAM26_API UClass* Z_Construct_UClass_UTeam26UI_NoRegister();
UMG_API UClass* Z_Construct_UClass_UUserWidget();
UPackage* Z_Construct_UPackage__Script_Team26();
// End Cross Module References

// Begin Class UTeam26UI Function OnGearUpdate
struct Team26UI_eventOnGearUpdate_Parms
{
	int32 NewGear;
};
static const FName NAME_UTeam26UI_OnGearUpdate = FName(TEXT("OnGearUpdate"));
void UTeam26UI::OnGearUpdate(int32 NewGear)
{
	Team26UI_eventOnGearUpdate_Parms Parms;
	Parms.NewGear=NewGear;
	UFunction* Func = FindFunctionChecked(NAME_UTeam26UI_OnGearUpdate);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Vehicle" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Implemented in Blueprint to display the new gear */" },
#endif
		{ "ModuleRelativePath", "Team26UI.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Implemented in Blueprint to display the new gear" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FIntPropertyParams NewProp_NewGear;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FIntPropertyParams Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::NewProp_NewGear = { "NewGear", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Team26UI_eventOnGearUpdate_Parms, NewGear), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::NewProp_NewGear,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTeam26UI, nullptr, "OnGearUpdate", nullptr, nullptr, Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::PropPointers), sizeof(Team26UI_eventOnGearUpdate_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08080800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::Function_MetaDataParams), Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::Function_MetaDataParams) };
static_assert(sizeof(Team26UI_eventOnGearUpdate_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UTeam26UI_OnGearUpdate()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UTeam26UI_OnGearUpdate_Statics::FuncParams);
	}
	return ReturnFunction;
}
// End Class UTeam26UI Function OnGearUpdate

// Begin Class UTeam26UI Function OnSpeedUpdate
struct Team26UI_eventOnSpeedUpdate_Parms
{
	float NewSpeed;
};
static const FName NAME_UTeam26UI_OnSpeedUpdate = FName(TEXT("OnSpeedUpdate"));
void UTeam26UI::OnSpeedUpdate(float NewSpeed)
{
	Team26UI_eventOnSpeedUpdate_Parms Parms;
	Parms.NewSpeed=NewSpeed;
	UFunction* Func = FindFunctionChecked(NAME_UTeam26UI_OnSpeedUpdate);
	ProcessEvent(Func,&Parms);
}
struct Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "Vehicle" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Implemented in Blueprint to display the new speed */" },
#endif
		{ "ModuleRelativePath", "Team26UI.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Implemented in Blueprint to display the new speed" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFloatPropertyParams NewProp_NewSpeed;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::NewProp_NewSpeed = { "NewSpeed", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(Team26UI_eventOnSpeedUpdate_Parms, NewSpeed), METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::NewProp_NewSpeed,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::PropPointers) < 2048);
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UTeam26UI, nullptr, "OnSpeedUpdate", nullptr, nullptr, Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::PropPointers), sizeof(Team26UI_eventOnSpeedUpdate_Parms), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x08080800, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::Function_MetaDataParams), Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::Function_MetaDataParams) };
static_assert(sizeof(Team26UI_eventOnSpeedUpdate_Parms) < MAX_uint16);
UFunction* Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate_Statics::FuncParams);
	}
	return ReturnFunction;
}
// End Class UTeam26UI Function OnSpeedUpdate

// Begin Class UTeam26UI
void UTeam26UI::StaticRegisterNativesUTeam26UI()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UTeam26UI);
UClass* Z_Construct_UClass_UTeam26UI_NoRegister()
{
	return UTeam26UI::StaticClass();
}
struct Z_Construct_UClass_UTeam26UI_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n *  Simple Vehicle HUD class\n *  Displays the current speed and gear.\n *  Widget setup is handled in a Blueprint subclass.\n */" },
#endif
		{ "IncludePath", "Team26UI.h" },
		{ "ModuleRelativePath", "Team26UI.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Simple Vehicle HUD class\nDisplays the current speed and gear.\nWidget setup is handled in a Blueprint subclass." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsMPH_MetaData[] = {
		{ "Category", "Vehicle" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Controls the display of speed in Km/h or MPH */" },
#endif
		{ "ModuleRelativePath", "Team26UI.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Controls the display of speed in Km/h or MPH" },
#endif
	};
#endif // WITH_METADATA
	static void NewProp_bIsMPH_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsMPH;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_UTeam26UI_OnGearUpdate, "OnGearUpdate" }, // 1434887220
		{ &Z_Construct_UFunction_UTeam26UI_OnSpeedUpdate, "OnSpeedUpdate" }, // 3234675244
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UTeam26UI>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
void Z_Construct_UClass_UTeam26UI_Statics::NewProp_bIsMPH_SetBit(void* Obj)
{
	((UTeam26UI*)Obj)->bIsMPH = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UTeam26UI_Statics::NewProp_bIsMPH = { "bIsMPH", nullptr, (EPropertyFlags)0x0020080000000015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UTeam26UI), &Z_Construct_UClass_UTeam26UI_Statics::NewProp_bIsMPH_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsMPH_MetaData), NewProp_bIsMPH_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UTeam26UI_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UTeam26UI_Statics::NewProp_bIsMPH,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26UI_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UTeam26UI_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UUserWidget,
	(UObject* (*)())Z_Construct_UPackage__Script_Team26,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26UI_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UTeam26UI_Statics::ClassParams = {
	&UTeam26UI::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_UTeam26UI_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26UI_Statics::PropPointers),
	0,
	0x00B010A1u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UTeam26UI_Statics::Class_MetaDataParams), Z_Construct_UClass_UTeam26UI_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UTeam26UI()
{
	if (!Z_Registration_Info_UClass_UTeam26UI.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UTeam26UI.OuterSingleton, Z_Construct_UClass_UTeam26UI_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UTeam26UI.OuterSingleton;
}
template<> TEAM26_API UClass* StaticClass<UTeam26UI>()
{
	return UTeam26UI::StaticClass();
}
UTeam26UI::UTeam26UI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UTeam26UI);
UTeam26UI::~UTeam26UI() {}
// End Class UTeam26UI

// Begin Registration
struct Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26UI_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UTeam26UI, UTeam26UI::StaticClass, TEXT("UTeam26UI"), &Z_Registration_Info_UClass_UTeam26UI, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UTeam26UI), 1932193886U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26UI_h_2861788778(TEXT("/Script/Team26"),
	Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26UI_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26UI_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
