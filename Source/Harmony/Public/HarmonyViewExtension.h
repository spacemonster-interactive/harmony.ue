#pragma once

#include "CoreMinimal.h"
#include "RenderGraphFwd.h"
#include "RHIResources.h"
#include "ScreenPass.h"
#include "SceneViewExtension.h"
#include "UObject/StrongObjectPtr.h"

class UTextureRenderTarget2D;
class FPostOpaqueRenderParameters;

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
	void PublishUserSceneTextures_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View);
	void RenderBackgroundSplatsPostOpaque_RenderThread(FPostOpaqueRenderParameters& Parameters);
	FScreenPassTexture RenderBackgroundSplatsCommon_RenderThread(
		FRDGBuilder& GraphBuilder,
		const FSceneView& View,
		FScreenPassTexture SceneColor,
		FScreenPassRenderTarget Output,
		TRDGUniformBufferRef<FSceneTextureUniformParameters> SceneTexturesUniform);
	FScreenPassTexture RenderSceneCoverageMaskVisualizationPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderCustomTonemapPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderTonemapReplacementPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
	FScreenPassTexture RenderForegroundSplatsPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessMaterialInputs& Inputs);
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
			FTextureRHIRef ForegroundSplatsTargetRHI;
			EPixelFormat ForegroundSplatsRequestedFormat = PF_Unknown;
			bool bForegroundSplatsRequestedSRGB = false;
			FIntPoint ForegroundSplatsRequestedExtent = FIntPoint::ZeroValue;
			FTextureRHIRef AutoExposureMeterMaskTargetRHI;
			FIntPoint AutoExposureMeterMaskRequestedExtent = FIntPoint::ZeroValue;
			bool bAutoExposureMeterMaskActive = false;
		FTextureRHIRef SceneCoverageHistoryTextures[2];
		FIntPoint SceneCoverageHistoryExtent = FIntPoint::ZeroValue;
		FIntRect SceneCoverageHistoryViewRect = FIntRect(0, 0, 0, 0);
		EPixelFormat SceneCoverageHistoryFormat = PF_Unknown;
		int32 SceneCoverageHistoryReadIndex = 0;
		bool bSceneCoverageHistoryValid = false;
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
		bool bUsedLayerPartitionForDirectDraw = false;
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
		bool bForegroundSplatsTargetValid = false;
		bool bReuseForegroundSplatsTargetThisFrame = false;
		uint32 ForegroundSplatsTargetDrawConfigHash = 0u;
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
		FBufferRHIRef LayerGroupCountBufferRHI;
		FUnorderedAccessViewRHIRef LayerGroupCountUAV;
		FBufferRHIRef LayerPartitionDispatchArgsBufferRHI;
		FUnorderedAccessViewRHIRef LayerPartitionDispatchArgsUAV;
		FBufferRHIRef LayerGroupOffsetBufferRHI;
		FUnorderedAccessViewRHIRef LayerGroupOffsetUAV;
		FBufferRHIRef BackgroundSortedIndicesBufferRHI;
		FShaderResourceViewRHIRef BackgroundSortedIndicesSRV;
		FUnorderedAccessViewRHIRef BackgroundSortedIndicesUAV;
		FBufferRHIRef ForegroundSortedIndicesBufferRHI;
		FShaderResourceViewRHIRef ForegroundSortedIndicesSRV;
		FUnorderedAccessViewRHIRef ForegroundSortedIndicesUAV;
		FBufferRHIRef BackgroundDrawIndexedArgsBufferRHI;
		FUnorderedAccessViewRHIRef BackgroundDrawIndexedArgsUAV;
		FBufferRHIRef ForegroundDrawIndexedArgsBufferRHI;
		FUnorderedAccessViewRHIRef ForegroundDrawIndexedArgsUAV;
		TArray<FCompressedDispatchSegment> CompressedDispatchSegments;
	};

	void InvalidateDirectDrawState_RenderThread();
	FRenderThreadState RenderThreadState;
	FDelegateHandle PostOpaqueRenderDelegateHandle;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundSplatsFallbackTarget_GameThread;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundAverageDepthFallbackTarget_GameThread;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundAmbiguityResolveFallbackTarget_GameThread;
		TStrongObjectPtr<UTextureRenderTarget2D> BackgroundDirectCoverageFallbackTarget_GameThread;
	TStrongObjectPtr<UTextureRenderTarget2D> ForegroundSplatsFallbackTarget_GameThread;
	TStrongObjectPtr<UTextureRenderTarget2D> AutoExposureMeterMaskTarget_GameThread;
	uint32 CachedSceneTopologyHash_GameThread = 0u;
	uint32 CachedSceneTransformHash_GameThread = 0u;
};
