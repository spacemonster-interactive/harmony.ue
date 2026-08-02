#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HarmonyProxyRTSubsystem.generated.h"

class UTextureRenderTarget2D;

USTRUCT()
struct HARMONY_API FHarmonyProxyRenderTargetSet
{
    GENERATED_BODY()

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> BackgroundSplats;

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> BackgroundAverageDepth;

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> BackgroundAmbiguityResolve;

    UPROPERTY(Transient)
    TObjectPtr<UTextureRenderTarget2D> BackgroundDirectCoverage;

    uint64 LastUsedFrame = 0u;
};

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

    FHarmonyProxyRenderTargetSet& GetOrAddProxyRenderTargets(
        uint64 ViewFamilyRenderTargetKey,
        uint64 CurrentFrame,
        uint64 RetentionFrames);

    /** Currently active background-splat target, exposed for runtime diagnostics. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Proxy RT", ToolTip="Live runtime render target for background-splat composition. Updated automatically when Proxy RTs are enabled."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundSplats;

    /** Currently active background average-depth target, exposed for runtime diagnostics. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Average Depth Proxy RT", ToolTip="Live runtime render target storing per-pixel average background splat depth. Updated automatically when Proxy RTs are enabled or when the buffer visualization mode requests it."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundAverageDepth;

    /** Currently active ambiguity-resolve target, exposed for runtime diagnostics. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Ambiguity Resolve Proxy RT", ToolTip="Live runtime render target storing the cached ambiguity-resolve splat contribution. Updated automatically when Proxy RTs are enabled and ambiguity resolve is active."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundAmbiguityResolve;

    /** Currently active direct-coverage target, exposed for runtime diagnostics. */
    UPROPERTY(Transient, VisibleAnywhere, Category="Layers", meta=(DisplayName="Background Direct Coverage RT", ToolTip="Live runtime render target storing visible direct background splat coverage for tonemap masking when Proxy RTs are disabled. Updated automatically when the custom tonemapper needs direct background coverage."))
    TObjectPtr<UTextureRenderTarget2D> BackgroundDirectCoverage;

private:
    UPROPERTY(Transient)
    TMap<uint64, FHarmonyProxyRenderTargetSet> ProxyRenderTargetsByFamily;
};
