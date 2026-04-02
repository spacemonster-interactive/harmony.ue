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

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(DisplayName="Use Proxy RTs", DisplayPriority="0", ToolTip="Enable offscreen proxy render targets for Harmony layer composition. When enabled, Harmony renders the background and foreground splat layers into live runtime render targets and composites those textures back into the scene instead of drawing those layers directly into scene color."))
    bool bUseProxyRT = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(DisplayName="Splat Pixel Radius", DisplayPriority="11", ClampMin="0.0", UIMin="0.0", ClampMax="64.0", UIMax="64.0", ToolTip="Global pixel-radius scale applied to Harmony splats before rasterization. Higher values make splats appear larger on screen."))
    float PreviewSplatPixelRadius = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(DisplayName="Enable Static View Preprocess Cache", DisplayPriority="1", ToolTip="Reuse preprocess and sort results when the camera view and relevant preprocess inputs are unchanged. Disable to force recompute every frame."))
    bool bEnableStaticViewPreprocessCache = true;

    UPROPERTY(Config, EditAnywhere, Category="Performance", meta=(ConsoleVariable="r.Harmony.Feature.Compose.ThroughTransparency.AmbiguityResolve", DisplayName="Enable Ambiguity Resolve", DisplayPriority="2", EditCondition="bUseProxyRT && bDrawBackgroundLayer", EditConditionHides, ToolTip="Enable the extra ambiguity-resolve path for proxy background composition. This improves mixed edge pixels where average proxy depth is ambiguous, but adds an extra resolve pass and can reduce performance."))
    bool bEnableBackgroundAmbiguityResolve = true;

    UPROPERTY(Config, EditAnywhere, Category="Native Compose", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.Enable", DisplayName="Replace Tonemapper", ToolTip="Enable Harmony tonemap replacement. When enabled, Harmony runs its custom tonemap pass before DOF and also registers a pass-through callback at Replacing Tonemapper so UE's default tonemapper is suppressed."))
    bool bEnableTonemap = true;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Feature.Preprocess.AdaptiveFrustumPadding", DisplayName="Use Distance-Based Frustum Padding", DisplayPriority="1", ToolTip="Use distance-based frustum padding in preprocess culling. Near and far padding values are blended between the near and far distance thresholds."))
    bool bAdaptivePreprocessFrustumPadding = false;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FrustumPadding", DisplayName="Frustum Padding", DisplayPriority="2", EditCondition="!bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.1", UIMin="0.1", UIMax="2.0", ToolTip="Fallback clip-space frustum padding used during preprocess culling when distance-based padding is disabled. Lower values cull more aggressively."))
    float PreprocessFrustumPadding = 1.2f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearFrustumPadding", DisplayName="Near Padding", DisplayPriority="3", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.1", UIMin="0.1", UIMax="2.0", ToolTip="Frustum padding used for splats at or nearer than Near Distance when distance-based padding is enabled."))
    float PreprocessNearFrustumPadding = 1.4f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarFrustumPadding", DisplayName="Far Padding", DisplayPriority="4", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.1", UIMin="0.1", UIMax="2.0", ToolTip="Frustum padding used for splats at or beyond Far Distance when distance-based padding is enabled."))
    float PreprocessFarFrustumPadding = 1.2f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearFrustumDistance", DisplayName="Near Distance", DisplayPriority="5", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10000.0", ToolTip="View-space distance where distance-based frustum padding starts at Near Padding."))
    float PreprocessNearFrustumDistance = 500.0f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarFrustumDistance", DisplayName="Far Distance", DisplayPriority="6", EditCondition="bAdaptivePreprocessFrustumPadding", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="50000.0", ToolTip="View-space distance where distance-based frustum padding finishes transitioning to Far Padding."))
    float PreprocessFarFrustumDistance = 5000.0f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearCullDistance", DisplayName="Near Cull Distance", DisplayPriority="7", ClampMin="0.0", UIMin="0.0", UIMax="1000.0", ToolTip="Minimum positive view-space Z required to keep a splat during preprocess. Higher values cull more splats near the camera."))
    float PreprocessNearCullDistance = 0.2f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Feature.Preprocess.AdaptiveMinAlpha", DisplayName="Use Distance-Based Min Alpha", DisplayPriority="8", ToolTip="Use distance-based alpha culling in preprocess. Near and far min alpha values are blended between the near and far distance thresholds."))
    bool bAdaptivePreprocessMinAlpha = false;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.MinAlpha", DisplayName="Min Alpha", DisplayPriority="9", EditCondition="!bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum post-opacity alpha required to keep a splat during preprocess when distance-based alpha culling is disabled."))
    float PreprocessMinAlpha = 0.06f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearMinAlpha", DisplayName="Near Min Alpha", DisplayPriority="10", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum alpha used for splats at or nearer than Near Min Alpha Distance when distance-based alpha culling is enabled."))
    float PreprocessNearMinAlpha = 0.06f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarMinAlpha", DisplayName="Far Min Alpha", DisplayPriority="11", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum alpha used for splats at or beyond Far Min Alpha Distance when distance-based alpha culling is enabled."))
    float PreprocessFarMinAlpha = 0.2f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.NearMinAlphaDistance", DisplayName="Near Min Alpha Distance", DisplayPriority="12", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10000.0", ToolTip="View-space distance where distance-based alpha culling starts at Near Min Alpha."))
    float PreprocessNearMinAlphaDistance = 500.0f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.FarMinAlphaDistance", DisplayName="Far Min Alpha Distance", DisplayPriority="13", EditCondition="bAdaptivePreprocessMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="50000.0", ToolTip="View-space distance where distance-based alpha culling finishes transitioning to Far Min Alpha."))
    float PreprocessFarMinAlphaDistance = 800.0f;

    UPROPERTY(Config, EditAnywhere, Category="Preprocess", meta=(ConsoleVariable="r.Harmony.Tuning.Preprocess.MinScreenRadiusPx", DisplayPriority="14", ClampMin="0.0", UIMin="0.0", UIMax="16.0", ToolTip="Cull splats in preprocess when the projected max radius in pixels is below this threshold."))
    float PreprocessMinScreenRadiusPx = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Feature.Draw.Background", DisplayName="Draw Background Splats", DisplayPriority="0", ToolTip="Render background splats in Harmony's fixed post-opaque background pass. Disable to skip background splat rendering entirely."))
    bool bDrawBackgroundLayer = true;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Feature.Draw.Foreground", DisplayName="Draw Foreground Splats", DisplayPriority="1", ToolTip="Render foreground splats in Harmony's fixed tonemap-stage foreground pass. Disable to skip foreground splat rendering entirely."))
    bool bDrawForegroundLayer = true;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Feature.Draw.Background.WriteSceneDepth", DisplayName="Write Background Depth To Scene", DisplayPriority="2", ToolTip="Write the background splat layer's weighted depth estimate into SceneDepth. This primarily affects depth-driven post effects such as DOF, and is only useful when the background splats pass runs before those effects."))
    bool bWriteBackgroundDepthToScene = true;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.Background.SceneDepthCoverageThreshold", DisplayName="Background Scene Depth Coverage Threshold", DisplayPriority="3", EditCondition="bWriteBackgroundDepthToScene", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Minimum accumulated background splat coverage required before Harmony writes a pixel into SceneDepth. Higher values reduce low-alpha tail contamination in depth-driven effects."))
    float BackgroundSceneDepthCoverageThreshold = 0.1f;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Feature.Draw.AdaptiveFragmentMinAlpha", DisplayName="Use Distance-Based Fragment Min Alpha", DisplayPriority="4", ToolTip="Use distance-based fragment alpha gating in the pixel shader. Near and far min alpha values are blended between the near and far distance thresholds."))
    bool bAdaptiveFragmentMinAlpha = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Feature.Draw.ClampFragmentMinAlpha", DisplayName="Clamp Low-Alpha Fragments", DisplayPriority="5", ToolTip="Experimental. When a fragment falls below the effective fragment min alpha threshold, clamp its alpha up to the threshold instead of discarding it. Disabled by default so the standard discard behavior remains unchanged."))
    bool bClampFragmentMinAlpha = false;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentMinAlpha", DisplayName="Fragment Min Alpha", DisplayPriority="6", EditCondition="!bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="0.25", ToolTip="Minimum final alpha used by the fragment gate. Fragments below this threshold are discarded by default, or clamped up to this threshold when Clamp Low-Alpha Fragments is enabled. Higher values reduce tail overdraw but can make splats look thinner or harsher."))
    float FragmentMinAlpha = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentNearMinAlpha", DisplayName="Fragment Near Min Alpha", DisplayPriority="7", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="0.25", ToolTip="Fragment min alpha used for splats at or nearer than Fragment Near Min Alpha Distance when distance-based fragment alpha gating is enabled. This threshold is either a discard gate or a clamp floor depending on Clamp Low-Alpha Fragments."))
    float FragmentNearMinAlpha = 0.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentFarMinAlpha", DisplayName="Fragment Far Min Alpha", DisplayPriority="8", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="0.25", ToolTip="Fragment min alpha used for splats at or beyond Fragment Far Min Alpha Distance when distance-based fragment alpha gating is enabled. This threshold is either a discard gate or a clamp floor depending on Clamp Low-Alpha Fragments."))
    float FragmentFarMinAlpha = 0.03f;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentNearMinAlphaDistance", DisplayName="Fragment Near Min Alpha Distance", DisplayPriority="9", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="10000.0", ToolTip="View-space distance where distance-based fragment alpha gating starts at Fragment Near Min Alpha."))
    float FragmentNearMinAlphaDistance = 500.0f;

    UPROPERTY(Config, EditAnywhere, Category="Draw", meta=(ConsoleVariable="r.Harmony.Tuning.Draw.FragmentFarMinAlphaDistance", DisplayName="Fragment Far Min Alpha Distance", DisplayPriority="10", EditCondition="bAdaptiveFragmentMinAlpha", EditConditionHides, ClampMin="0.0", UIMin="0.0", UIMax="50000.0", ToolTip="View-space distance where distance-based fragment alpha gating finishes transitioning to Fragment Far Min Alpha."))
    float FragmentFarMinAlphaDistance = 800.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.EnableBloom", ToolTip="Include CombinedBloom input in the plugin tonemap pass."))
    bool bTonemapEnableBloom = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.EnableSceneTint", ToolTip="Apply SceneColorTint from the view's post-process settings in the plugin tonemap pass."))
    bool bTonemapEnableSceneTint = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.CompositeForeground", ToolTip="Composite the foreground splat layer at the end of the plugin tonemap pass."))
    bool bTonemapCompositeForeground = true;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Tonemap.MaskMode", ToolTip="Controls which scene-coverage channel the plugin tonemap pass uses for its selective tonemap mask."))
    EHarmonyTonemapMaskMode TonemapMaskMode = EHarmonyTonemapMaskMode::OpaqueMinusTranslucency;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.BloomStrength", ClampMin="0.0", UIMin="0.0", UIMax="4.0", ToolTip="Bloom multiplier for the plugin tonemap pass."))
    float TonemapBloomStrength = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ExposureCompensationStops", UIMin="-8.0", UIMax="8.0", ToolTip="Additional exposure compensation, in stops, applied in the plugin tonemap pass."))
    float TonemapExposureCompensationStops = 0.5f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLift", ClampMin="0.0", UIMin="0.0", UIMax="0.25", ToolTip="Lift dark regions before tonemapping while leaving mids and highlights mostly unchanged."))
    float TonemapShadowLift = 0.02f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLiftPivot", ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Compressed luminance pivot where shadow lift starts fading out."))
    float TonemapShadowLiftPivot = 0.15f;

    UPROPERTY(Config, EditAnywhere, Category="Tonemapping", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Tonemap.ShadowLiftSoftness", ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Soft transition width around the shadow-lift pivot in compressed luminance space."))
    float TonemapShadowLiftSoftness = 0.25f;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.DepthDistance", ClampMin="0.001", UIMin="0.001", UIMax="100000.0", ToolTip="Depth distance divisor used by the compose scene-coverage mask."))
    float ComposeMaskDepthDistance = 100000.0f;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.TransparentPow", ClampMin="0.0", UIMin="0.0", UIMax="32.0", ToolTip="Exponent applied to separate translucency background visibility when building the compose scene-coverage mask."))
    float ComposeMaskTransparentPow = 15.0f;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Mask.CrossBlur", ToolTip="Enable edge feathering during compose scene-coverage mask generation."))
    bool bComposeMaskCrossBlur = true;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.CrossBlurPx", ClampMin="0.0", UIMin="0.0", UIMax="8.0", ToolTip="Edge feather radius in pixels for compose scene-coverage mask generation."))
    float ComposeMaskCrossBlurPx = 1.0f;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Feature.Compose.Mask.SceneCoverageHistory.Enable", ToolTip="Enable temporal reprojection history for the compose scene-coverage mask."))
    bool bComposeMaskSceneCoverageHistory = true;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.SceneCoverageHistory.Weight", ClampMin="0.0", UIMin="0.0", ClampMax="1.0", UIMax="1.0", ToolTip="Previous-frame blend weight for compose scene-coverage history on edge pixels."))
    float ComposeMaskSceneCoverageHistoryWeight = 0.9f;

    UPROPERTY(Config, EditAnywhere, Category="Masking|Compose", meta=(ConsoleVariable="r.Harmony.Tuning.Compose.Mask.SceneCoverageHistory.VelocityFalloff", ClampMin="0.0", UIMin="0.0", UIMax="16.0", ToolTip="Velocity falloff distance in pixels for compose scene-coverage history. History blend weight decays exponentially as pixel motion increases (exp(-velocity / falloff)). Lower values suppress history more aggressively during camera movement. 0 disables velocity attenuation."))
    float ComposeMaskSceneCoverageHistoryVelocityFalloff = 2.0f;

};
