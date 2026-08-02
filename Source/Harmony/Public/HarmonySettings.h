#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "HarmonySettings.generated.h"

UENUM(BlueprintType)
enum class EHarmonyTonemapMaskMode : uint8
{
    OpaqueAndTranslucency UMETA(DisplayName="Opaque + Translucency"),
    OpaqueOnly UMETA(DisplayName="Opaque Only"),
    OpaqueMinusTranslucency UMETA(DisplayName="Opaque Minus Translucency")
};

UENUM(BlueprintType)
enum class EHarmonyTonemappingMode : uint8
{
    EarlyMasked UMETA(DisplayName="Early Masked"),
    TonemapCompensation UMETA(DisplayName="Tonemap Compensation"),
    StockTonemapping UMETA(DisplayName="Stock Tonemapping")
};

UENUM(BlueprintType)
enum class EHarmonyTonemapCompensationMethod : uint8
{
    Invalid = 0 UMETA(Hidden),
    FixedACES = 2 UMETA(DisplayName="Fixed ACES"),
    UEFilmCurve = 3 UMETA(DisplayName="UE Film Curve")
};

UCLASS(Config=Engine, DefaultConfig, meta=(DisplayName="Harmony"))
class HARMONY_API UHarmonySettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    virtual void PostInitProperties() override;
    virtual void PostReloadConfig(FProperty* PropertyThatWasLoaded) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual FName GetCategoryName() const override;
    virtual FText GetSectionText() const override;
    virtual FText GetSectionDescription() const override;
#endif

    UPROPERTY(Config, EditAnywhere, Category="General")
    bool bEnabled = true;

    UPROPERTY(Config, EditAnywhere, Category="Draw|General", meta=(DisplayName="Splat Pixel Radius", DisplayPriority="0", ClampMin="0.0", UIMin="0.0", ClampMax="2.0", UIMax="2.0", ToolTip="Global pixel-radius scale applied before rasterization. Higher values make splats appear larger on screen."))
    float PreviewSplatPixelRadius = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(DisplayName="Reuse Static View Preprocess", DisplayPriority="1", ToolTip="Reuse preprocess, sort, and the complete proxy splat raster when the camera view and relevant splat inputs are unchanged. Disable to force the same proxy work to be recomputed every frame without changing rendering quality or translucent ordering policy."))
    bool bEnableStaticViewPreprocessCache = true;

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(DisplayName="Enable Splat Temporal Super Resolution", DisplayPriority="2", ToolTip="Allow Unreal's temporal jitter to be applied to splat rasterization so TAA or TSR can reconstruct additional subpixel detail. It activates only when the current render screen percentage is at or below the configured maximum. While active it requires rerasterizing the early splat proxy every frame, although preprocessing and sorting may still be reused."))
    bool bSplatTemporalSuperResolution = true;

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(DisplayName="Splat Temporal Super Resolution Maximum Screen Percentage", DisplayPriority="3", EditCondition="bSplatTemporalSuperResolution", ClampMin="1.0", UIMin="25.0", ClampMax="100.0", UIMax="100.0", Units="Percent", ToolTip="Apply Splat Temporal Super Resolution only when the current render screen percentage is at or below this value. Above the threshold Harmony uses a stable zero-jitter proxy that can be fully cached. A value of 100 enables the feature at every normal screen percentage."))
    float SplatTemporalSuperResolutionMaxScreenPercentage = 50.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(DisplayName="Tonemapping Mode", DisplayPriority="0", ToolTip="Choose how Harmony interacts with tonemapping. Early Masked runs Harmony's masked tonemap pass earlier in the pipeline and excludes splat pixels. Tonemap Compensation keeps UE's tonemapper and pre-expands splat color before it enters scene color. Stock Tonemapping leaves UE tonemapping unchanged."))
    EHarmonyTonemappingMode TonemappingMode = EHarmonyTonemappingMode::EarlyMasked;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Feature.Preprocess.AdaptiveFrustumPadding", DisplayName="Use Distance-Based Frustum Padding", DisplayPriority="0", ToolTip="Use distance-based frustum padding in culling. Near and far padding values are blended between the near and far distance thresholds."))
    bool bAdaptivePreprocessFrustumPadding = false;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FrustumPadding", DisplayName="Base Frustum Padding", DisplayPriority="1", EditCondition="!bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.1", UIMin="0.1", UIMax="2.0", ToolTip="Fallback clip-space frustum padding used during culling when distance-based padding is disabled. Lower values cull more aggressively."))
    float PreprocessFrustumPadding = 1.2f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearFrustumPadding", DisplayName="Near Padding", DisplayPriority="2", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.1", UIMin="0.1", UIMax="2.0", ToolTip="Frustum padding used for splats at or nearer than Near Distance when distance-based padding is enabled."))
    float PreprocessNearFrustumPadding = 1.4f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarFrustumPadding", DisplayName="Far Padding", DisplayPriority="3", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.1", UIMin="0.1", UIMax="2.0", ToolTip="Frustum padding used for splats at or beyond Far Distance when distance-based padding is enabled."))
    float PreprocessFarFrustumPadding = 1.2f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearFrustumDistance", DisplayName="Near Distance", DisplayPriority="4", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10000.0", ToolTip="View-space distance where distance-based frustum padding starts at Near Padding."))
    float PreprocessNearFrustumDistance = 500.0f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarFrustumDistance", DisplayName="Far Distance", DisplayPriority="5", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="50000.0", ToolTip="View-space distance where distance-based frustum padding finishes transitioning to Far Padding."))
    float PreprocessFarFrustumDistance = 5000.0f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Frustum", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearCullDistance", DisplayName="Near Cull Distance", DisplayPriority="6", ClampMin="0.0", UIMin="0.0", UIMax="1000.0", ToolTip="Minimum positive view-space Z required to keep a splat during culling. Higher values cull more splats near the camera."))
    float PreprocessNearCullDistance = 0.2f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Alpha", meta=(ConsoleVariable="r.Harmony.Feature.Preprocess.AdaptiveMinAlpha", DisplayName="Use Distance-Based Splat Min Alpha", DisplayPriority="0", ToolTip="Use distance-based alpha culling before rasterization. Near and far splat min alpha values are blended between the near and far distance thresholds."))
    bool bAdaptivePreprocessMinAlpha = false;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.MinAlpha", DisplayName="Splat Min Alpha", DisplayPriority="1", EditCondition="!bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum post-opacity alpha required to keep a whole splat during culling when distance-based alpha culling is disabled."))
    float PreprocessMinAlpha = 0.06f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearMinAlpha", DisplayName="Near Splat Min Alpha", DisplayPriority="2", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum alpha used for splats at or nearer than Near Alpha Distance when distance-based alpha culling is enabled."))
    float PreprocessNearMinAlpha = 0.06f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarMinAlpha", DisplayName="Far Splat Min Alpha", DisplayPriority="3", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum alpha used for splats at or beyond Far Alpha Distance when distance-based alpha culling is enabled."))
    float PreprocessFarMinAlpha = 0.2f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearMinAlphaDistance", DisplayName="Near Alpha Distance", DisplayPriority="4", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10000.0", ToolTip="View-space distance where distance-based alpha culling starts at Near Splat Min Alpha."))
    float PreprocessNearMinAlphaDistance = 500.0f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarMinAlphaDistance", DisplayName="Far Alpha Distance", DisplayPriority="5", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="50000.0", ToolTip="View-space distance where distance-based alpha culling finishes transitioning to Far Splat Min Alpha."))
    float PreprocessFarMinAlphaDistance = 800.0f;

    UPROPERTY(Config, EditAnywhere, Category="Culling|Screen Size", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.MinScreenRadiusPx", DisplayName="Min Screen Radius (px)", DisplayPriority="0", ClampMin="0.0", UIMin="0.0", UIMax="16.0", ToolTip="Cull splats when the projected max radius in pixels is below this threshold."))
    float PreprocessMinScreenRadiusPx = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Depth", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.Background.SceneDepthCoverageThreshold", DisplayName="Depth Write Coverage Threshold", DisplayPriority="1", ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum accumulated background splat coverage required before a pixel writes into SceneDepth for components with Write Depth To Scene enabled. The write is committed after standard translucency so translucent shadow/modulate geometry is not rejected by synthetic splat depth, while DOF and TSR still receive it. Higher values reduce low-alpha tail contamination in depth-driven effects."))
    float BackgroundSceneDepthCoverageThreshold = 0.1f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Background", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Background.OpaqueEnvironment", DisplayName="Treat Early Background Splats as Opaque", DisplayPriority="0", ToolTip="Treat every non-empty pixel in the cached early/background splat layer as opaque when it is composed. Intended for enclosing environment captures viewed from the inside, where partial splat coverage would otherwise reveal the scene behind the environment. Late/foreground splats are unchanged."))
    bool bOpaqueEnvironmentBackground = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Feature.Draw.AdaptiveFragmentMinAlpha", DisplayName="Use Distance-Based Fragment Alpha Cutoff", DisplayPriority="0", ToolTip="Use distance-based fragment alpha gating in the pixel shader. Near and far fragment alpha thresholds are blended between the near and far distance thresholds."))
    bool bAdaptiveFragmentMinAlpha = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Feature.Draw.ClampFragmentMinAlpha", DisplayName="Clamp Instead Of Discard", DisplayPriority="1", ToolTip="When a fragment falls below the effective fragment alpha cutoff, clamp its alpha up to the cutoff instead of discarding it. Disabled by default so the standard discard behavior remains unchanged."))
    bool bClampFragmentMinAlpha = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentMinAlpha", DisplayName="Fragment Min Alpha", DisplayPriority="2", EditCondition="!bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="0.25", ToolTip="Minimum final alpha used by the fragment gate. Fragments below this threshold are discarded by default, or clamped up to this threshold when Clamp Instead Of Discard is enabled. Higher values reduce tail overdraw but can make splats look thinner or harsher."))
    float FragmentMinAlpha = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentNearMinAlpha", DisplayName="Near Min Alpha", DisplayPriority="2", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="0.25", ToolTip="Fragment min alpha used for splats at or nearer than Near Distance when distance-based fragment alpha gating is enabled. This threshold is either a discard gate or a clamp floor depending on Clamp Instead Of Discard."))
    float FragmentNearMinAlpha = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentFarMinAlpha", DisplayName="Far Min Alpha", DisplayPriority="3", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="0.25", ToolTip="Fragment min alpha used for splats at or beyond Far Distance when distance-based fragment alpha gating is enabled. This threshold is either a discard gate or a clamp floor depending on Clamp Instead Of Discard."))
    float FragmentFarMinAlpha = 0.03f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentNearMinAlphaDistance", DisplayName="Near Distance", DisplayPriority="4", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10000.0", ToolTip="View-space distance where distance-based fragment alpha gating starts at Near Min Alpha."))
    float FragmentNearMinAlphaDistance = 500.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentFarMinAlphaDistance", DisplayName="Far Distance", DisplayPriority="5", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="50000.0", ToolTip="View-space distance where distance-based fragment alpha gating finishes transitioning to Far Min Alpha."))
    float FragmentFarMinAlphaDistance = 800.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Fragment Alpha", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.SplatTailCutoff", DisplayName="Splat Tail Cutoff", DisplayPriority="6", ClampMin="0.1", UIMin="0.1", UIMax="6.0", ToolTip="Finite support cutoff for the Gaussian splat tail. Higher values extend the splat footprint and soften the outer edge, but increase overdraw."))
    float SplatTailCutoff = 1.8f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.EnableBloom", EditCondition="false", EditConditionHides, HideEditConditionToggle))
    bool bTonemapEnableBloom = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.EnableSceneTint", EditCondition="false", EditConditionHides, HideEditConditionToggle))
    bool bTonemapEnableSceneTint = true;

    // Serialized for backward compatibility. The splat-negative mask is now authoritative:
    // native UE pixels are tonemapped by default and verified splats are preserved unless
    // covering translucency requires the already-combined pixel to be tonemapped.
    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.MaskMode", DisplayName="Tonemap Mask Mode (Legacy)", EditCondition="false", EditConditionHides, HideEditConditionToggle))
    EHarmonyTonemapMaskMode TonemapMaskMode = EHarmonyTonemapMaskMode::OpaqueMinusTranslucency;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.BloomStrength", EditCondition="false", EditConditionHides, HideEditConditionToggle))
    float TonemapBloomStrength = 1.0f;

    // Keep the replacement tonemap neutral by default so opaque UE geometry tracks the stock
    // tonemapper as closely as this plugin-visible path allows. Artistic offsets remain opt-in.
    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ExposureCompensationStops", DisplayName="Exposure Compensation", DisplayPriority="3", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, UIMin="-8.0", UIMax="8.0", ToolTip="Optional exposure offset, in stops, applied in the plugin tonemap pass. Keep this at 0 for the closest match to the stock UE tonemapper."))
    float TonemapExposureCompensationStops = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLift", DisplayPriority="4", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="0.25", ToolTip="Optional shadow lift applied before tonemapping. Keep this at 0 for the closest match to the stock UE tonemapper."))
    float TonemapShadowLift = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLiftPivot", DisplayPriority="5", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Compressed luminance pivot where shadow lift starts fading out. Only used when Shadow Lift is above 0."))
    float TonemapShadowLiftPivot = 0.15f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLiftSoftness", DisplayPriority="6", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Soft transition width around the shadow-lift pivot in compressed luminance space. Only used when Shadow Lift is above 0."))
    float TonemapShadowLiftSoftness = 0.25f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapMethod", DisplayName="Compensation Curve", DisplayPriority="7", EditCondition="TonemappingMode == EHarmonyTonemappingMode::TonemapCompensation", EditConditionHides, ToolTip="UE Film Curve follows the active UE film controls and uses a bloom-safe highlight shoulder. Fixed ACES uses a stable scene-independent approximation."))
    EHarmonyTonemapCompensationMethod TonemapCompensationMethod = EHarmonyTonemapCompensationMethod::UEFilmCurve;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapScale", DisplayName="Tonemap Compensation Scale", DisplayPriority="8", EditCondition="TonemappingMode == EHarmonyTonemappingMode::TonemapCompensation", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="2.0", ToolTip="Overall multiplier applied after the Tonemap Compensation expansion. Lower values reduce the brightness of splats before the stock UE tonemapper sees them."))
    float SplatPreInverseScale = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapGamma", DisplayName="Tonemap Compensation Gamma", DisplayPriority="9", EditCondition="TonemappingMode == EHarmonyTonemappingMode::TonemapCompensation", EditConditionHides, ClampMin="0.25", UIMin="0.25", UIMax="2.5", ToolTip="Display-gamma decode applied before the selected inverse film curve. The default 2.2 is the stable reference value."))
    float SplatPreInverseGamma = 2.2f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapSaturationScale", DisplayName="Tonemap Compensation Saturation", DisplayPriority="10", EditCondition="TonemappingMode == EHarmonyTonemappingMode::TonemapCompensation", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="2.0", ToolTip="Post-inverse saturation trim for Tonemap Compensation. Lower values pull the expanded splat color back toward luma before UE's stock tonemapper runs."))
    float SplatPreInverseSaturationScale = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.DepthDistance", DisplayName="Opaque Depth Distance", DisplayPriority="0", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.001", UIMin="100000.0", UIMax="1000000000.0", ToolTip="Maximum scene depth treated as opaque coverage when building the selective tonemap mask. Keep this high enough to cover distant world geometry; lowering it can leave far geometry outside Early Masked tonemapping."))
    float ComposeMaskDepthDistance = 1000000000.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.TransparentPow", DisplayName="Translucency Suppression Power", DisplayPriority="1", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="32.0", ToolTip="Exponent applied to separate translucency background visibility when building the scene-coverage mask."))
    float ComposeMaskTransparentPow = 15.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.PostEarlySceneColorDifferenceThreshold", DisplayName="Direct Translucency Detection Threshold", DisplayPriority="2", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.000001", UIMin="0.0001", UIMax="0.02", ToolTip="Relative SceneColor difference used to detect Unreal content rendered directly over early splats between post-opaque and BeforeDOF. Lower values detect subtler glass modulation."))
    float ComposeMaskPostEarlySceneColorDifferenceThreshold = 0.001f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.PostEarlySceneColorModulationResidualThreshold", DisplayName="Modulation-Only Residual Threshold", DisplayPriority="2", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="0.05", ToolTip="Maximum relative residual after fitting a direct SceneColor change as pure attenuation. Qualifying shadow/modulation pixels preserve their splat background without tonemapping. Lower values are more conservative and reduce the chance of exempting tinted glass."))
    float ComposeMaskPostEarlySceneColorModulationResidualThreshold = 0.03f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Mask.CrossBlur", DisplayName="Feather Mask Edges", DisplayPriority="3", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ToolTip="Enable edge feathering during scene-coverage mask generation."))
    bool bComposeMaskCrossBlur = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.CrossBlurPx", DisplayName="Feather Radius (px)", DisplayPriority="3", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked && bComposeMaskCrossBlur", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="8.0", ToolTip="Edge feather radius in pixels for scene-coverage mask generation."))
    float ComposeMaskCrossBlurPx = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.ExpandPx", DisplayName="Mask Expansion (px)", DisplayPriority="4", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ClampMin="-2.0", UIMin="-2.0", ClampMax="2.0", UIMax="2.0", ToolTip="Signed morphology radius for the selective tonemap mask. Positive values expand the geometry region receiving tonemapping; negative values contract it; zero disables morphology. Fractional values retain partial source coverage for smoother antialiased boundaries."))
    float ComposeMaskExpandPx = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Mask.SceneCoverageHistory.Enable", DisplayName="Use Mask History", DisplayPriority="5", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked", EditConditionHides, ToolTip="Enable temporal reprojection history for the scene-coverage mask."))
    bool bComposeMaskSceneCoverageHistory = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.SceneCoverageHistory.Weight", DisplayName="Mask History Weight", DisplayPriority="5", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked && bComposeMaskSceneCoverageHistory", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Previous-frame blend weight for scene-coverage history on edge pixels."))
    float ComposeMaskSceneCoverageHistoryWeight = 0.9f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.SceneCoverageHistory.VelocityFalloff", DisplayName="Mask History Velocity Falloff", DisplayPriority="6", EditCondition="TonemappingMode == EHarmonyTonemappingMode::EarlyMasked && bComposeMaskSceneCoverageHistory", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="16.0", ToolTip="Velocity falloff distance in pixels for scene-coverage history. History blend weight decays exponentially as pixel motion increases (exp(-velocity / falloff)). Lower values suppress history more aggressively during camera movement. 0 disables velocity attenuation."))
    float ComposeMaskSceneCoverageHistoryVelocityFalloff = 2.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Translucency Depth", meta=(ConsoleVariable="r.Harmony.Feature.Splats.TranslucentCustomDepthSplit", DisplayName="Split Splats At Translucent Custom Depth", DisplayPriority="0", ToolTip="Use participating translucent CustomDepth in every tonemapping mode to reserve splats nearer than the surface for the post-translucency late pass. Proxy rendering keeps a complete geometry-independent layer for cacheability and rerasterizes the depth-selected foreground contribution late. The translucent material must Allow Custom Depth Writes and the mesh component must Render CustomDepth Pass."))
    bool bTranslucentCustomDepthSplit = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Translucency Depth", meta=(ConsoleVariable="r.Harmony.Tuning.Splats.TranslucentCustomDepthStencilValue", DisplayName="Custom Stencil Value", DisplayPriority="1", EditCondition="bTranslucentCustomDepthSplit", EditConditionHides, ClampMin="0", UIMin="0", ClampMax="255", UIMax="255", ToolTip="Optional Custom Stencil identifier for participating translucent meshes. 0 accepts every valid CustomDepth writer; 1-255 restricts the split to the masked stencil value. Use a reserved nonzero value when the project has unrelated CustomDepth writers."))
    int32 TranslucentCustomDepthStencilValue = 0;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Translucency Depth", meta=(ConsoleVariable="r.Harmony.Tuning.Splats.TranslucentCustomDepthStencilMask", DisplayName="Custom Stencil Mask", DisplayPriority="2", EditCondition="bTranslucentCustomDepthSplit && TranslucentCustomDepthStencilValue != 0", EditConditionHides, ClampMin="0", UIMin="0", ClampMax="255", UIMax="255", ToolTip="Bit mask used when comparing the Custom Stencil value."))
    int32 TranslucentCustomDepthStencilMask = 255;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Translucency Depth", meta=(ConsoleVariable="r.Harmony.Tuning.Splats.TranslucentCustomDepthBias", DisplayName="Depth Split Bias", DisplayPriority="3", EditCondition="bTranslucentCustomDepthSplit", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10.0", ToolTip="View-space bias in Unreal units. A splat must be at least this far in front of the translucent CustomDepth surface to move to the late pass."))
    float TranslucentCustomDepthBias = 5.0f;

};
