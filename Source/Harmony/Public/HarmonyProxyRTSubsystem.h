#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HarmonyProxyRTSubsystem.generated.h"

class UTextureRenderTarget2D;

/**
 * Owns the transient proxy render targets used by the Harmony view extension.
 *
 * Keeping these here (rather than on the UHarmonySettings CDO) ensures they are
 * created after the engine's "Disregard for GC" window has closed, which prevents
 * the GC assumption-violation crash that occurs in packaged builds when a CDO
 * (always in the root set) references a UObject that is not.
 */
UCLASS()
class HARMONY_API UHarmonyProxyRTSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    /** Live runtime render target used for background-splat composition. Managed by FHarmonyViewExtension. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Proxy RT", ToolTip="Live runtime render target for background-splat composition. Updated automatically when Proxy RTs are enabled."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundSplats;

    /** Live runtime render target used for background average-depth visualization/blending. Managed by FHarmonyViewExtension. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Average Depth Proxy RT", ToolTip="Live runtime render target storing per-pixel average background splat depth. Updated automatically when Proxy RTs are enabled or when the buffer visualization mode requests it."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundAverageDepth;

    /** Live runtime render target used for cached ambiguity-resolve splat composition. Managed by FHarmonyViewExtension. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Ambiguity Resolve Proxy RT", ToolTip="Live runtime render target storing the cached ambiguity-resolve splat contribution. Updated automatically when Proxy RTs are enabled and ambiguity resolve is active."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundAmbiguityResolve;

    /** Live runtime render target storing visible direct background splat coverage for the custom tonemap mask. Managed by FHarmonyViewExtension. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Direct Coverage RT", ToolTip="Live runtime render target storing visible direct background splat coverage for tonemap masking when Proxy RTs are disabled. Updated automatically when the custom tonemapper needs direct background coverage."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundDirectCoverage;

    /** Live runtime render target used for foreground-splat composition. Managed by FHarmonyViewExtension. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Foreground Proxy RT", ToolTip="Live runtime render target for foreground-splat composition. Updated automatically when Proxy RTs are enabled."))
    TObjectPtr<UTextureRenderTarget2D> ForegroundSplats;
};
