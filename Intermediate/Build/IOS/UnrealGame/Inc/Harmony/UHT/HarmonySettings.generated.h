// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonySettings.h"

#ifdef HARMONY_HarmonySettings_generated_h
#error "HarmonySettings.generated.h already included, missing '#pragma once' in HarmonySettings.h"
#endif
#define HARMONY_HarmonySettings_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonySettings *********************************************************
struct Z_Construct_UClass_UHarmonySettings_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonySettings(ETypeConstructPhase);

#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h_35_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonySettings_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonySettings(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonySettings, UDeveloperSettings, COMPILED_IN_FLAGS(0 | CLASS_DefaultConfig | CLASS_Config), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonySettings) \
	DECLARE_SERIALIZER(UHarmonySettings) \
	static constexpr const TCHAR* StaticConfigName() {return TEXT("Engine");} \



#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h_35_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UHarmonySettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonySettings(UHarmonySettings&&) = delete; \
	UHarmonySettings(const UHarmonySettings&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonySettings); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonySettings); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UHarmonySettings) \
	NO_API virtual ~UHarmonySettings();


#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h_32_PROLOG
#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h_35_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h_35_INCLASS_NO_PURE_DECLS \
	FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h_35_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonySettings;

// ********** End Class UHarmonySettings ***********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260802065047_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonySettings_h

// ********** Begin Enum EHarmonyTonemapMaskMode ***************************************************
#define FOREACH_ENUM_EHARMONYTONEMAPMASKMODE(op) \
	op(EHarmonyTonemapMaskMode::OpaqueAndTranslucency) \
	op(EHarmonyTonemapMaskMode::OpaqueOnly) \
	op(EHarmonyTonemapMaskMode::OpaqueMinusTranslucency) 

enum class EHarmonyTonemapMaskMode : uint8;
template<> struct TIsUEnumClass<EHarmonyTonemapMaskMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyTonemapMaskMode>();
// ********** End Enum EHarmonyTonemapMaskMode *****************************************************

// ********** Begin Enum EHarmonyTonemappingMode ***************************************************
#define FOREACH_ENUM_EHARMONYTONEMAPPINGMODE(op) \
	op(EHarmonyTonemappingMode::EarlyMasked) \
	op(EHarmonyTonemappingMode::TonemapCompensation) \
	op(EHarmonyTonemappingMode::StockTonemapping) 

enum class EHarmonyTonemappingMode : uint8;
template<> struct TIsUEnumClass<EHarmonyTonemappingMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyTonemappingMode>();
// ********** End Enum EHarmonyTonemappingMode *****************************************************

// ********** Begin Enum EHarmonyTonemapCompensationMethod *****************************************
#define FOREACH_ENUM_EHARMONYTONEMAPCOMPENSATIONMETHOD(op) \
	op(EHarmonyTonemapCompensationMethod::Invalid) \
	op(EHarmonyTonemapCompensationMethod::FixedACES) \
	op(EHarmonyTonemapCompensationMethod::UEFilmCurve) 

enum class EHarmonyTonemapCompensationMethod : uint8;
template<> struct TIsUEnumClass<EHarmonyTonemapCompensationMethod> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyTonemapCompensationMethod>();
// ********** End Enum EHarmonyTonemapCompensationMethod *******************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
