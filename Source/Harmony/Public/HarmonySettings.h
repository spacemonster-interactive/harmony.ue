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
    Harmony UMETA(DisplayName="Harmony"),
    SplatPreInverse UMETA(DisplayName="Splat Pre-Inverse"),
    None UMETA(DisplayName="None")
};

UENUM(BlueprintType)
enum class EHarmonyBackgroundForegroundSplitMode : uint8
{
    WorldOrigin UMETA(DisplayName="World Origin"),
    Camera UMETA(DisplayName="Camera")
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

    UPROPERTY(Config, EditAnywhere, Category="Proxy", meta=(DisplayName="Use Proxy Render Targets", DisplayPriority="0", ToolTip="Render the background and foreground splat layers into offscreen runtime render targets, then composite those textures back into the scene. Disable to draw splats directly into scene color instead."))
    bool bUseProxyRT = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw|General", meta=(DisplayName="Splat Pixel Radius", DisplayPriority="0", ClampMin="0.0", UIMin="0.0", ClampMax="2.0", UIMax="2.0", ToolTip="Global pixel-radius scale applied before rasterization. Higher values make splats appear larger on screen."))
    float PreviewSplatPixelRadius = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(DisplayName="Reuse Static View Preprocess", DisplayPriority="1", ToolTip="Reuse preprocess and sort results when the camera view and relevant preprocess inputs are unchanged. Disable to force recompute every frame."))
    bool bEnableStaticViewPreprocessCache = true;

    UPROPERTY(Config, EditAnywhere, Category="Proxy", meta=(ConsoleVariable="r.Harmony.Feature.Compose.ThroughTransparency.AmbiguityResolve", DisplayName="Resolve Proxy Depth Ambiguity", DisplayPriority="1", EditCondition="bUseProxyRT && (!bEnableBackgroundForegroundSplit || bDrawBackgroundLayer)", EditConditionHides, ToolTip="Enable the extra ambiguity-resolve path for proxy background composition. This improves mixed edge pixels where average proxy depth is ambiguous, but adds an extra resolve pass and can reduce performance."))
    bool bEnableBackgroundAmbiguityResolve = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(DisplayName="Tonemapping Mode", DisplayPriority="0", ToolTip="Choose how Harmony interacts with tonemapping. Harmony replaces the tonemapper. Splat Pre-Inverse keeps UE's tonemapper and pre-expands splat color before it enters scene color. None leaves UE tonemapping untouched."))
    EHarmonyTonemappingMode TonemappingMode = EHarmonyTonemappingMode::Harmony;

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

    UPROPERTY(Config, EditAnywhere, Category="Draw|Layers", meta=(ConsoleVariable="r.Harmony.Feature.Layers.Split", DisplayName="Foreground / Background Split", DisplayPriority="0", ToolTip="Split splats into background and foreground layers. Disable to render all splats in the background layer and skip the foreground layer entirely."))
    bool bEnableBackgroundForegroundSplit = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Layers", meta=(ConsoleVariable="r.Harmony.Feature.Layers.SplitMode", DisplayName="Split Mode", DisplayPriority="1", EditCondition="bEnableBackgroundForegroundSplit", EditConditionHides, ToolTip="Choose whether the background/foreground split threshold is measured relative to the world origin or the current camera."))
    EHarmonyBackgroundForegroundSplitMode BackgroundForegroundSplitMode = EHarmonyBackgroundForegroundSplitMode::WorldOrigin;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Layers", meta=(DisplayName="World Origin Split Offset", DisplayPriority="2", EditCondition="bEnableBackgroundForegroundSplit && BackgroundForegroundSplitMode == EHarmonyBackgroundForegroundSplitMode::WorldOrigin", EditConditionHides, UIMin="-10000.0", UIMax="10000.0", ToolTip="Global offset added to each component's background / foreground split threshold when Split Mode is World Origin."))
    float WorldOriginBackgroundForegroundSplitOffset = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Layers", meta=(DisplayName="Camera Split Offset", DisplayPriority="3", EditCondition="bEnableBackgroundForegroundSplit && BackgroundForegroundSplitMode == EHarmonyBackgroundForegroundSplitMode::Camera", EditConditionHides, UIMin="-10000.0", UIMax="10000.0", ToolTip="Global offset added to each component's background / foreground split threshold when Split Mode is Camera."))
    float CameraBackgroundForegroundSplitOffset = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Layers", meta=(ConsoleVariable="r.Harmony.Feature.Draw.Background", DisplayName="Draw Background Splats", DisplayPriority="4", EditCondition="bEnableBackgroundForegroundSplit", EditConditionHides, ToolTip="Render background splats in the fixed post-opaque background pass. Disable to skip background splat rendering entirely."))
    bool bDrawBackgroundLayer = true;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Layers", meta=(ConsoleVariable="r.Harmony.Feature.Draw.Foreground", DisplayName="Draw Foreground Splats", DisplayPriority="5", EditCondition="bEnableBackgroundForegroundSplit", EditConditionHides, ToolTip="Render foreground splats in the fixed tonemap-stage foreground pass. Disable to skip foreground splat rendering entirely."))
    bool bDrawForegroundLayer = true;

    UPROPERTY(Config, EditAnywhere, Category="Draw|Depth", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.Background.SceneDepthCoverageThreshold", DisplayName="Depth Write Coverage Threshold", DisplayPriority="1", ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum accumulated background splat coverage required before a pixel writes into SceneDepth for components with Write Depth To Scene enabled. Higher values reduce low-alpha tail contamination in depth-driven effects."))
    float BackgroundSceneDepthCoverageThreshold = 0.1f;

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

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.CompositeForeground", DisplayName="Composite Foreground Splats", DisplayPriority="1", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony && bEnableBackgroundForegroundSplit", EditConditionHides, ToolTip="Composite the foreground splat layer at the end of the plugin tonemap pass."))
    bool bTonemapCompositeForeground = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.MaskMode", DisplayName="Tonemap Mask Mode", DisplayPriority="2", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ToolTip="Controls which scene-coverage channel the plugin tonemap pass uses for its selective tonemap mask."))
    EHarmonyTonemapMaskMode TonemapMaskMode = EHarmonyTonemapMaskMode::OpaqueMinusTranslucency;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.BloomStrength", EditCondition="false", EditConditionHides, HideEditConditionToggle))
    float TonemapBloomStrength = 1.0f;

    // Keep the replacement tonemap neutral by default so opaque UE geometry tracks the stock
    // tonemapper as closely as this plugin-visible path allows. Artistic offsets remain opt-in.
    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ExposureCompensationStops", DisplayName="Exposure Compensation", DisplayPriority="3", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, UIMin="-8.0", UIMax="8.0", ToolTip="Optional exposure offset, in stops, applied in the plugin tonemap pass. Keep this at 0 for the closest match to the stock UE tonemapper."))
    float TonemapExposureCompensationStops = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLift", DisplayPriority="4", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="0.25", ToolTip="Optional shadow lift applied before tonemapping. Keep this at 0 for the closest match to the stock UE tonemapper."))
    float TonemapShadowLift = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLiftPivot", DisplayPriority="5", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Compressed luminance pivot where shadow lift starts fading out. Only used when Shadow Lift is above 0."))
    float TonemapShadowLiftPivot = 0.15f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLiftSoftness", DisplayPriority="6", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Soft transition width around the shadow-lift pivot in compressed luminance space. Only used when Shadow Lift is above 0."))
    float TonemapShadowLiftSoftness = 0.25f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapScale", DisplayName="Splat Pre-Inverse Scale", DisplayPriority="7", EditCondition="TonemappingMode == EHarmonyTonemappingMode::SplatPreInverse", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="2.0", ToolTip="Overall multiplier applied after the Splat Pre-Inverse expansion. Lower values reduce the brightness of splats before the stock UE tonemapper sees them."))
    float SplatPreInverseScale = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapGamma", DisplayName="Splat Pre-Inverse Gamma", DisplayPriority="8", EditCondition="TonemappingMode == EHarmonyTonemappingMode::SplatPreInverse", EditConditionHides, ClampMin="0.25", UIMin="0.25", UIMax="2.5", ToolTip="Power curve applied after inverse tonemap expansion. Values above 1 compress the lifted result and usually reduce the washed-out midtone look."))
    float SplatPreInverseGamma = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Background.InverseTonemapSaturationScale", DisplayName="Splat Pre-Inverse Saturation", DisplayPriority="9", EditCondition="TonemappingMode == EHarmonyTonemappingMode::SplatPreInverse", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="2.0", ToolTip="Post-inverse saturation trim for Splat Pre-Inverse. Lower values pull the expanded splat color back toward luma before UE's stock tonemapper runs."))
    float SplatPreInverseSaturationScale = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.DepthDistance", DisplayName="Opaque Depth Distance", DisplayPriority="0", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ClampMin="0.001", UIMin="0.001", UIMax="100000.0", ToolTip="Depth threshold used when building the scene-coverage mask for selective tonemapping."))
    float ComposeMaskDepthDistance = 100000.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.TransparentPow", DisplayName="Translucency Suppression Power", DisplayPriority="1", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="32.0", ToolTip="Exponent applied to separate translucency background visibility when building the scene-coverage mask."))
    float ComposeMaskTransparentPow = 15.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Mask.CrossBlur", DisplayName="Feather Mask Edges", DisplayPriority="2", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ToolTip="Enable edge feathering during scene-coverage mask generation."))
    bool bComposeMaskCrossBlur = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.CrossBlurPx", DisplayName="Feather Radius (px)", DisplayPriority="3", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony && bComposeMaskCrossBlur", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="8.0", ToolTip="Edge feather radius in pixels for scene-coverage mask generation."))
    float ComposeMaskCrossBlurPx = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Mask.SceneCoverageHistory.Enable", DisplayName="Use Mask History", DisplayPriority="4", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony", EditConditionHides, ToolTip="Enable temporal reprojection history for the scene-coverage mask."))
    bool bComposeMaskSceneCoverageHistory = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.SceneCoverageHistory.Weight", DisplayName="Mask History Weight", DisplayPriority="5", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony && bComposeMaskSceneCoverageHistory", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Previous-frame blend weight for scene-coverage history on edge pixels."))
    float ComposeMaskSceneCoverageHistoryWeight = 0.9f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping|Masking", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.SceneCoverageHistory.VelocityFalloff", DisplayName="Mask History Velocity Falloff", DisplayPriority="6", EditCondition="TonemappingMode == EHarmonyTonemappingMode::Harmony && bComposeMaskSceneCoverageHistory", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="16.0", ToolTip="Velocity falloff distance in pixels for scene-coverage history. History blend weight decays exponentially as pixel motion increases (exp(-velocity / falloff)). Lower values suppress history more aggressively during camera movement. 0 disables velocity attenuation."))
    float ComposeMaskSceneCoverageHistoryVelocityFalloff = 2.0f;

};
