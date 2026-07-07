#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphFwd.h"
#include "RHIResources.h"
#include "ScreenPass.h"
#include "SceneViewExtension.h"
#include "UObject/StrongObjectPtr.h"

class FPostOpaqueRenderParameters;

// The two stages of the splat composite. Early runs at post-opaque (before translucency) and handles
// pixels without opaque coverage; Late runs after Harmony's custom tonemap (both hook BeforeDOF, so
// splats still receive DOF) and composites the splat contribution over opaque-covered pixels.
enum class EHarmonySplatStage : uint8
{
	Early,
	Late
};

class HARMONY_API FHarmonyViewExtension : public FSceneViewExtensionBase
{
public:
    FHarmonyViewExtension(const FAutoRegister& AutoRegister);
    virtual ~FHarmonyViewExtension() override;

	void ReleaseSplatRenderResources_GameThread();

	//~ Begin FSceneViewExtensionBase Interface
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& View) override;
	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;
	virtual void SubscribeToPostProcessingPass(EPostProcessingPass Pass, const FSceneView& InView, FPostProcessingPassDelegateArray& InOutPassCallbacks, bool bIsPassEnabled) override;
	//~ End FSceneViewExtensionBase Interface

private:
	bool IsDirectDrawPreparedForView_RenderThread(const FSceneView& View) const;
	void EnsureDirectDrawPreparedForView_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View);
	void PrepareDirectDraw_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View);
	// Early splat stage: hooked at post-opaque; draws splats where the scene has no opaque coverage.
	void RenderEarlySplatsPass_RenderThread(FPostOpaqueRenderParameters& Parameters);
	FScreenPassTexture RenderSplatsCommon_RenderThread(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		FScreenPassTexture SceneColor,
		FScreenPassRenderTarget Output,
		TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTexturesUniform,
		EHarmonySplatStage Stage = EHarmonySplatStage::Early,
		// When true, the draw target is a dedicated offscreen splat buffer (not scene color), so the
		// draw writes premultiplied color AND coverage to alpha (CW_RGBA) instead of preserving scene
		// alpha — required so the later upscale/composite can occlude the scene with the coverage.
		bool bRenderIntoDedicatedSplatBuffer = false);
	FScreenPassTexture RenderSceneCoverageMaskVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderSplatOverdrawVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderSeparateTranslucencyVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderCustomTonemapPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderTonemapReplacementPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	// Late splat stage: composites the splat contribution over opaque-covered pixels after the early
	// custom tonemap, so the tonemapper runs on a splat-free scene. Handles direct draw and proxy.
	FScreenPassTexture RenderLateSplatsPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	void ReleaseSplatRenderResources_RenderThread();

	struct FRenderThreadState
	{
		struct FCompressedDispatchSegment
		{
			uint32 SourceSplatOffset = 0u;
			uint32 NumSplats = 0u;
			FIntVector4 QuantizationZeroPoints = FIntVector4(0, 0, 0, 0);
			FVector4f QuantizationScales = FVector4f(1.0f, 1.0f, 1.0f, 1.0f);
		};

		bool bEnabled = false;
		FTextureRHIRef BackgroundSplatsTargetRHI;
		EPixelFormat BackgroundSplatsRequestedFormat = PF_Unknown;
		bool bBackgroundSplatsRequestedSRGB = false;
		FIntPoint BackgroundSplatsRequestedExtent = FIntPoint::ZeroValue;
		FTextureRHIRef BackgroundAverageDepthTargetRHI;
		EPixelFormat BackgroundAverageDepthRequestedFormat = PF_Unknown;
		bool bBackgroundAverageDepthRequestedSRGB = false;
		FIntPoint BackgroundAverageDepthRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundAmbiguityResolveTargetRHI;
			EPixelFormat BackgroundAmbiguityResolveRequestedFormat = PF_Unknown;
			bool bBackgroundAmbiguityResolveRequestedSRGB = false;
			FIntPoint BackgroundAmbiguityResolveRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef BackgroundDirectCoverageTargetRHI;
			EPixelFormat BackgroundDirectCoverageRequestedFormat = PF_Unknown;
			bool bBackgroundDirectCoverageRequestedSRGB = false;
			FIntPoint BackgroundDirectCoverageRequestedExtent = FIntPoint::ZeroValue;
			bool bBackgroundDirectCoverageTargetValid = false;
			FIntRect BackgroundDirectCoverageViewRect = FIntRect(0, 0, 0, 0);
			FTextureRHIRef AutoExposureMeterMaskTargetRHI;
			FIntPoint AutoExposureMeterMaskRequestedExtent = FIntPoint::ZeroValue;
			bool bAutoExposureMeterMaskActive = false;
		FTextureRHIRef SceneCoverageHistoryTextures[2];
		FIntPoint SceneCoverageHistoryExtent = FIntPoint::ZeroValue;
		FIntRect SceneCoverageHistoryViewRect = FIntRect(0, 0, 0, 0);
		EPixelFormat SceneCoverageHistoryFormat = PF_Unknown;
		int32 SceneCoverageHistoryReadIndex = 0;
		bool bSceneCoverageHistoryValid = false;
		FRDGTextureRef OpaqueSceneDepthTexture = nullptr;
		FIntRect OpaqueSceneDepthViewRect = FIntRect(0, 0, 0, 0);
		UPTRINT OpaqueSceneDepthViewKey = 0u;
		FRDGTextureRef SplatOverdrawCounterTexture = nullptr;
		FIntRect SplatOverdrawCounterViewRect = FIntRect(0, 0, 0, 0);
		UPTRINT SplatOverdrawCounterViewKey = 0u;
		// Render-resolution viewport/extent captured by the early (pre-TSR) splat stage, so the late
		// (post-TSR) stage can rasterize its splats at the same resolution into an offscreen buffer and
		// upscale — otherwise the render-res early splats seam against native-output-res late splats
		// under TSR upscaling. Captured every frame the early stage runs.
		FIntRect SplatRenderViewRect = FIntRect(0, 0, 0, 0);
		FIntPoint SplatRenderExtent = FIntPoint::ZeroValue;
		// Per-frame opaque tile classification (bit0=tile has opaque, bit1=tile has non-opaque),
		// built at post-opaque from pristine scene depth; consumed by the direct background draws
		// for pixel-exact splat culling. RDG resource: only valid within the frame's graph.
		FRDGBufferSRVRef OpaqueTileMaskSRV = nullptr;
		FIntPoint OpaqueTileMaskTiles = FIntPoint::ZeroValue;
		UPTRINT OpaqueTileMaskViewKey = 0u;
		float SplatPixelRadius = 1.0f;
		uint32 SHDegree = 3;
		uint32 NumSplats = 0;
		uint32 NumExpandedSplats = 0;
		uint32 NumCompressedSplats = 0;
		uint32 NumInstances = 0;
		uint32 NumCullVolumes = 0;
		uint32 NumPreprocessCullVolumes = 0;
		uint32 SortCount = 0;
		uint32 PreparedSortedValueBufferIndex = 0;
		bool bPreparedForDirectDraw = false;
		uint64 PreparedDirectDrawFrameCounter = 0u;
		UPTRINT PreparedDirectDrawViewKey = 0u;
		bool bEnableDirectDrawViewCaching = true;
		bool bDirectDrawViewCacheValid = false;
		FIntRect DirectDrawViewCacheViewRect = FIntRect(0, 0, 0, 0);
		FIntRect DirectDrawViewCacheUnconstrainedViewRect = FIntRect(0, 0, 0, 0);
		FIntRect DirectDrawViewCacheFamilyViewRect = FIntRect(0, 0, 0, 0);
		FMatrix44f DirectDrawViewCacheWorldToClip = FMatrix44f::Identity;
		FMatrix44f DirectDrawViewCacheWorldToView = FMatrix44f::Identity;
		uint32 DirectDrawViewCacheConfigHash = 0u;
		bool bDirectDrawViewCacheVisualizeBuffer = false;
		FName DirectDrawViewCacheBufferVisualizationMode = NAME_None;
		bool bBackgroundSplatsTargetValid = false;
		bool bReuseBackgroundSplatsTargetThisFrame = false;
		uint32 BackgroundSplatsTargetDrawConfigHash = 0u;
		bool bBackgroundAverageDepthTargetValid = false;
		bool bReuseBackgroundAverageDepthTargetThisFrame = false;
		uint32 BackgroundAverageDepthTargetDrawConfigHash = 0u;
		bool bBackgroundAmbiguityResolveTargetValid = false;
		bool bReuseBackgroundAmbiguityResolveTargetThisFrame = false;
		uint32 BackgroundAmbiguityResolveTargetDrawConfigHash = 0u;
		bool bAnyComponentWritesDepthToScene = true;
		bool bAnyComponentSkipsDepthToScene = false;
		FBufferRHIRef SplatPosRadiusBufferRHI;
		FShaderResourceViewRHIRef SplatPosRadiusSRV;
		FBufferRHIRef SplatOutputIndexBufferRHI;
		FShaderResourceViewRHIRef SplatOutputIndexSRV;
		FBufferRHIRef SplatInstanceIndexBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceIndexSRV;
		FBufferRHIRef SplatInstanceTransformBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceTransformSRV;
		FBufferRHIRef SplatInstanceSettingsBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceSettingsSRV;
		FBufferRHIRef SplatInstanceFadeSettingsBufferRHI;
		FShaderResourceViewRHIRef SplatInstanceFadeSettingsSRV;
		FBufferRHIRef CullVolumeDataBufferRHI;
		FShaderResourceViewRHIRef CullVolumeDataSRV;
		FBufferRHIRef PreprocessCullVolumeDataBufferRHI;
		FShaderResourceViewRHIRef PreprocessCullVolumeDataSRV;
		FBufferRHIRef SplatColorBufferRHI;
		FShaderResourceViewRHIRef SplatColorSRV;
		FBufferRHIRef SplatCovarianceBufferRHI;
		FShaderResourceViewRHIRef SplatCovarianceSRV;
		FBufferRHIRef SplatSHCoeffsBufferRHI;
		FShaderResourceViewRHIRef SplatSHCoeffsSRV;
		FBufferRHIRef CompressedPositionBufferRHI;
		FShaderResourceViewRHIRef CompressedPositionSRV;
		FBufferRHIRef CompressedOutputIndexBufferRHI;
		FShaderResourceViewRHIRef CompressedOutputIndexSRV;
		FBufferRHIRef CompressedInstanceIndexBufferRHI;
		FShaderResourceViewRHIRef CompressedInstanceIndexSRV;
		FBufferRHIRef CompressedMetadataBufferRHI;
		FShaderResourceViewRHIRef CompressedMetadataSRV;
		FBufferRHIRef CompressedCovarianceBufferRHI;
		FShaderResourceViewRHIRef CompressedCovarianceSRV;
		FBufferRHIRef CompressedSHDataBufferRHI;
		FShaderResourceViewRHIRef CompressedSHDataSRV;
		FBufferRHIRef PreprocessedSplatBufferRHI;
		FShaderResourceViewRHIRef PreprocessedSplatSRV;
		FUnorderedAccessViewRHIRef PreprocessedSplatUAV;
		FBufferRHIRef SortKeyBuffersRHI[2];
		FShaderResourceViewRHIRef SortKeySRVs[2];
		FUnorderedAccessViewRHIRef SortKeyUAVs[2];
		FBufferRHIRef SortValueBuffersRHI[2];
		FShaderResourceViewRHIRef SortValueSRVs[2];
		FUnorderedAccessViewRHIRef SortValueUAVs[2];
		FBufferRHIRef RadixGroupHistogramBufferRHI;
		FShaderResourceViewRHIRef RadixGroupHistogramSRV;
		FUnorderedAccessViewRHIRef RadixGroupHistogramUAV;
		FBufferRHIRef RadixPartitionStateBufferRHI;
		FShaderResourceViewRHIRef RadixPartitionStateSRV;
		FUnorderedAccessViewRHIRef RadixPartitionStateUAV;
		FBufferRHIRef RadixDigitBaseBufferRHI;
		FShaderResourceViewRHIRef RadixDigitBaseSRV;
		FUnorderedAccessViewRHIRef RadixDigitBaseUAV;
		FBufferRHIRef RadixDispatchArgsBufferRHI;
		FShaderResourceViewRHIRef RadixDispatchArgsSRV;
		FUnorderedAccessViewRHIRef RadixDispatchArgsUAV;
		FBufferRHIRef DrawIndexedArgsBufferRHI;
		FUnorderedAccessViewRHIRef DrawIndexedArgsUAV;
		TArray<FCompressedDispatchSegment> CompressedDispatchSegments;
	};

	void InvalidateDirectDrawState_RenderThread();
	FRenderThreadState RenderThreadState;
	FDelegateHandle PostOpaqueRenderDelegateHandle;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundSplatsFallbackTarget_GameThread;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundAverageDepthFallbackTarget_GameThread;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundAmbiguityResolveFallbackTarget_GameThread;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundDirectCoverageFallbackTarget_GameThread;
	TStrongObjectPtr<UTextureRenderTarget2D> AutoExposureMeterMaskTarget_GameThread;
	uint32 CachedSceneTopologyHash_GameThread = 0u;
	uint32 CachedSceneTransformHash_GameThread = 0u;
};
