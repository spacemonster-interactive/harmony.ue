// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "HarmonyVolumeComponent.h"

#ifdef HARMONY_HarmonyVolumeComponent_generated_h
#error "HarmonyVolumeComponent.generated.h already included, missing '#pragma once' in HarmonyVolumeComponent.h"
#endif
#define HARMONY_HarmonyVolumeComponent_generated_h

#include "UObject/ObjectMacros.h"
#include "UObject/ReflectedTypeAccessors.h"
#include "Templates/IsUEnumClass.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

// ********** Begin Class UHarmonyVolumeComponent **************************************************
struct Z_Construct_UClass_UHarmonyVolumeComponent_Statics;
HARMONY_API UClass* Z_Construct_UClass_UHarmonyVolumeComponent(ETypeConstructPhase);

#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h_46_INCLASS_NO_PURE_DECLS \
private: \
	friend struct ::Z_Construct_UClass_UHarmonyVolumeComponent_Statics; \
	friend HARMONY_API UClass* ::Z_Construct_UClass_UHarmonyVolumeComponent(ETypeConstructPhase); \
public: \
	DECLARE_CLASS2(UHarmonyVolumeComponent, USceneComponent, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/Harmony"), Z_Construct_UClass_UHarmonyVolumeComponent) \
	DECLARE_SERIALIZER(UHarmonyVolumeComponent)


#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h_46_ENHANCED_CONSTRUCTORS \
	/** Deleted move- and copy-constructors, should never be used */ \
	UHarmonyVolumeComponent(UHarmonyVolumeComponent&&) = delete; \
	UHarmonyVolumeComponent(const UHarmonyVolumeComponent&) = delete; \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UHarmonyVolumeComponent); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UHarmonyVolumeComponent); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(UHarmonyVolumeComponent) \
	NO_API virtual ~UHarmonyVolumeComponent();


#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h_43_PROLOG
#define FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h_46_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h_46_INCLASS_NO_PURE_DECLS \
	FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h_46_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


class UHarmonyVolumeComponent;

// ********** End Class UHarmonyVolumeComponent ****************************************************

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_peteshand_p4_8000_GsDemo_Unreal_Saved_PrecompiledPluginBuild_Harmony_IOS_20260707014027_Package_HostProject_Plugins_Harmony_Source_Harmony_Public_HarmonyVolumeComponent_h

// ********** Begin Enum EHarmonyVolumeShape *******************************************************
#define FOREACH_ENUM_EHARMONYVOLUMESHAPE(op) \
	op(EHarmonyVolumeShape::Box) \
	op(EHarmonyVolumeShape::Sphere) 

enum class EHarmonyVolumeShape : uint8;
template<> struct TIsUEnumClass<EHarmonyVolumeShape> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVolumeShape>();
// ********** End Enum EHarmonyVolumeShape *********************************************************

// ********** Begin Enum EHarmonyVolumeBoundaryMode ************************************************
#define FOREACH_ENUM_EHARMONYVOLUMEBOUNDARYMODE(op) \
	op(EHarmonyVolumeBoundaryMode::HardCull) \
	op(EHarmonyVolumeBoundaryMode::FeatherFade) 

enum class EHarmonyVolumeBoundaryMode : uint8;
template<> struct TIsUEnumClass<EHarmonyVolumeBoundaryMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVolumeBoundaryMode>();
// ********** End Enum EHarmonyVolumeBoundaryMode **************************************************

// ********** Begin Enum EHarmonyVolumeRuleMode ****************************************************
#define FOREACH_ENUM_EHARMONYVOLUMERULEMODE(op) \
	op(EHarmonyVolumeRuleMode::AlwaysCull) \
	op(EHarmonyVolumeRuleMode::CullIfCameraDistanceLessThan) 

enum class EHarmonyVolumeRuleMode : uint8;
template<> struct TIsUEnumClass<EHarmonyVolumeRuleMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVolumeRuleMode>();
// ********** End Enum EHarmonyVolumeRuleMode ******************************************************

// ********** Begin Enum EHarmonyVolumeEffectMode **************************************************
#define FOREACH_ENUM_EHARMONYVOLUMEEFFECTMODE(op) \
	op(EHarmonyVolumeEffectMode::AlphaCull) \
	op(EHarmonyVolumeEffectMode::DepthOffset) 

enum class EHarmonyVolumeEffectMode : uint8;
template<> struct TIsUEnumClass<EHarmonyVolumeEffectMode> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVolumeEffectMode>();
// ********** End Enum EHarmonyVolumeEffectMode ****************************************************

// ********** Begin Enum EHarmonyVolumeEvaluationStage *********************************************
#define FOREACH_ENUM_EHARMONYVOLUMEEVALUATIONSTAGE(op) \
	op(EHarmonyVolumeEvaluationStage::Fragment) \
	op(EHarmonyVolumeEvaluationStage::Preprocess) 

enum class EHarmonyVolumeEvaluationStage : uint8;
template<> struct TIsUEnumClass<EHarmonyVolumeEvaluationStage> { enum { Value = true }; };
template<> UE_NODEBUG HARMONY_NON_ATTRIBUTED_API UEnum* StaticEnum<EHarmonyVolumeEvaluationStage>();
// ********** End Enum EHarmonyVolumeEvaluationStage ***********************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
