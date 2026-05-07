// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Team26PlayerController.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef TEAM26_Team26PlayerController_generated_h
#error "Team26PlayerController.generated.h already included, missing '#pragma once' in Team26PlayerController.h"
#endif
#define TEAM26_Team26PlayerController_generated_h

#define FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h_20_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesATeam26PlayerController(); \
	friend struct Z_Construct_UClass_ATeam26PlayerController_Statics; \
public: \
	DECLARE_CLASS(ATeam26PlayerController, APlayerController, COMPILED_IN_FLAGS(CLASS_Abstract | CLASS_Config), CASTCLASS_None, TEXT("/Script/Team26"), NO_API) \
	DECLARE_SERIALIZER(ATeam26PlayerController)


#define FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h_20_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API ATeam26PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	ATeam26PlayerController(ATeam26PlayerController&&); \
	ATeam26PlayerController(const ATeam26PlayerController&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ATeam26PlayerController); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ATeam26PlayerController); \
	DEFINE_ABSTRACT_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(ATeam26PlayerController) \
	NO_API virtual ~ATeam26PlayerController();


#define FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h_17_PROLOG
#define FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h_20_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h_20_INCLASS_NO_PURE_DECLS \
	FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h_20_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> TEAM26_API UClass* StaticClass<class ATeam26PlayerController>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_8th_Team26_CH3_Project_Team26_Source_Team26_Team26PlayerController_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
