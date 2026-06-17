// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyComponent.h"

#ifdef HARMONY_HarmonyComponent_generated_h
#error "HarmonyComponent.generated.h already included, missing '#pragma once' in HarmonyComponent.h"
#endif
#define HARMONY_HarmonyComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin ScriptStruct FHarmonyAlphaFadeSettings *****************************************
struct Z_Construct_UScriptStruct_FHarmonyAlphaFadeSettings_Statics;
#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_20_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyAlphaFadeSettings_Statics; \
	static class UScriptStruct* StaticStruct();


struct FHarmonyAlphaFadeSettings;
// ********** End ScriptStruct FHarmonyAlphaFadeSettings *******************************************

// ********** Begin ScriptStruct FHarmonyDepthOffsetSettings ***************************************
struct Z_Construct_UScriptStruct_FHarmonyDepthOffsetSettings_Statics;
#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_38_GENERATED_BODY \
	friend struct ::Z_Construct_UScriptStruct_FHarmonyDepthOffsetSettings_Statics; \
	static class UScriptStruct* StaticStruct();


struct FHarmonyDepthOffsetSettings;
// ********** End ScriptStruct FHarmonyDepthOffsetSettings *****************************************

// ********** Begin Class UHarmonyComponent ********************************************************
struct Z_Construct_UClass_UHarmonyComponent_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonyComponent_NoRegister();

#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_62_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUHarmonyComponent(); \
	friend struct ::Z_Construct_UClass_UHarmonyComponent_Statics; \
	static UClass* GetPrivateStaticClass(); \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonyComponent_NoRegister(); \
public: \
	DECLARE_CLASS2(UHarmonyComponent, USceneComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonyComponent_NoRegister) \
	DECLARE_SERIALIZER(UHarmonyComponent)


#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_62_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyComponent(UHarmonyComponent&&) = delete; \
	UHarmonyComponent(const UHarmonyComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyComponent) \
	NO_API virtual ~UHarmonyComponent();


#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_59_PROLOG
#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_62_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_62_INCLASS_NO_PURE_DECLS \
	FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h_62_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyComponent;

// ********** End Class UHarmonyComponent **********************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260617115404_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyComponent_h

// ********** Begin Enum EHarmonySplatDepthOffsetMode **********************************************
#define FOREACH_ENUM_EHARMONYSPLATDEPTHOFFSETMODE(op) \
	op(EHarmonySplatDepthOffsetMode::SimpleViewSpace) \
	op(EHarmonySplatDepthOffsetMode::GroundWeighted) 

enum class EHarmonySplatDepthOffsetMode : uint8;
template<> struct TIsUEnumClass<EHarmonySplatDepthOffsetMode> { enum { Value = true }; };
template<> HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonySplatDepthOffsetMode>();
// ********** End Enum EHarmonySplatDepthOffsetMode ************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
