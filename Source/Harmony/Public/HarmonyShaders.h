#pragma once

#include "GlobalShader.h"
#include "PostProcess/PostProcessEyeAdaptation.h"
#include "PostProcess/PostProcessLocalExposure.h"
#include "RenderResource.h"
#include "ScreenPass.h"
#include "SceneTexturesConfig.h"
#include "SceneView.h"

/**
 * This file defines all the resources needed for rendering a Gaussian Splat Layers
 */

// Define compute / VS / PS C++ classes
//----------------------------------------------------------------
BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyPreprocessPassParams, )
    SHADER_PARAMETER(FMatrix44f, WorldToClip)
    SHADER_PARAMETER(FMatrix44f, WorldToView)
    SHADER_PARAMETER(FVector3f, CameraWorldPos)
    SHADER_PARAMETER(FVector3f, ViewForward)
    SHADER_PARAMETER(FVector2f, SplatViewportSize)
    SHADER_PARAMETER(FVector2f, Focal)
    SHADER_PARAMETER(uint32, PreprocessLayerFilter)
    SHADER_PARAMETER(float, PreprocessFrustumPadding)
    SHADER_PARAMETER(uint32, PreprocessUseAdaptiveFrustumPadding)
    SHADER_PARAMETER(float, PreprocessNearFrustumPadding)
    SHADER_PARAMETER(float, PreprocessFarFrustumPadding)
    SHADER_PARAMETER(float, PreprocessNearFrustumDistance)
    SHADER_PARAMETER(float, PreprocessFarFrustumDistance)
    SHADER_PARAMETER(float, PreprocessNearCullDistance)
    SHADER_PARAMETER(uint32, PreprocessUseAdaptiveMinAlpha)
    SHADER_PARAMETER(float, PreprocessMinAlpha)
    SHADER_PARAMETER(float, PreprocessNearMinAlpha)
    SHADER_PARAMETER(float, PreprocessFarMinAlpha)
    SHADER_PARAMETER(float, PreprocessNearMinAlphaDistance)
    SHADER_PARAMETER(float, PreprocessFarMinAlphaDistance)
    SHADER_PARAMETER(float, PreprocessMinScreenRadiusPx)
    SHADER_PARAMETER(float, SplatPixelRadius)
    SHADER_PARAMETER(float, KernelSize)
    SHADER_PARAMETER(uint32, SHDegree)
    SHADER_PARAMETER(uint32, NumSourceSplats)
    SHADER_PARAMETER(uint32, MaxOutputSplats)
    SHADER_PARAMETER(uint32, SourceSplatOffset)
    SHADER_PARAMETER(uint32, NumInstances)
    SHADER_PARAMETER(uint32, NumPreprocessCullVolumes)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatPosRadius)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatOutputIndex)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatInstanceIndex)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatInstanceTransform)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatInstanceSettings)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatInstanceFadeSettings)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, PreprocessCullVolumeData)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatColor)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatCovariance)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatSHCoeffs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<float4>, RWPreprocessedSplats)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortValues)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyPreprocessCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyPreprocessCS);
    using FParameters = FHarmonyPreprocessPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyPreprocessCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyPreprocessCompressedPassParams, )
    SHADER_PARAMETER(FMatrix44f, WorldToClip)
    SHADER_PARAMETER(FMatrix44f, WorldToView)
    SHADER_PARAMETER(FVector3f, CameraWorldPos)
    SHADER_PARAMETER(FVector3f, ViewForward)
    SHADER_PARAMETER(FVector2f, SplatViewportSize)
    SHADER_PARAMETER(FVector2f, Focal)
    SHADER_PARAMETER(uint32, PreprocessLayerFilter)
    SHADER_PARAMETER(float, PreprocessFrustumPadding)
    SHADER_PARAMETER(uint32, PreprocessUseAdaptiveFrustumPadding)
    SHADER_PARAMETER(float, PreprocessNearFrustumPadding)
    SHADER_PARAMETER(float, PreprocessFarFrustumPadding)
    SHADER_PARAMETER(float, PreprocessNearFrustumDistance)
    SHADER_PARAMETER(float, PreprocessFarFrustumDistance)
    SHADER_PARAMETER(float, PreprocessNearCullDistance)
    SHADER_PARAMETER(uint32, PreprocessUseAdaptiveMinAlpha)
    SHADER_PARAMETER(float, PreprocessMinAlpha)
    SHADER_PARAMETER(float, PreprocessNearMinAlpha)
    SHADER_PARAMETER(float, PreprocessFarMinAlpha)
    SHADER_PARAMETER(float, PreprocessNearMinAlphaDistance)
    SHADER_PARAMETER(float, PreprocessFarMinAlphaDistance)
    SHADER_PARAMETER(float, PreprocessMinScreenRadiusPx)
    SHADER_PARAMETER(float, SplatPixelRadius)
    SHADER_PARAMETER(float, KernelSize)
    SHADER_PARAMETER(uint32, SHDegree)
    SHADER_PARAMETER(uint32, NumSourceSplats)
    SHADER_PARAMETER(uint32, MaxOutputSplats)
    SHADER_PARAMETER(uint32, SourceSplatOffset)
    SHADER_PARAMETER(uint32, NumInstances)
    SHADER_PARAMETER(uint32, NumPreprocessCullVolumes)
    SHADER_PARAMETER(int32, CompressedColorDCZeroPoint)
    SHADER_PARAMETER(int32, CompressedColorRestZeroPoint)
    SHADER_PARAMETER(int32, CompressedOpacityZeroPoint)
    SHADER_PARAMETER(int32, CompressedScalingFactorZeroPoint)
    SHADER_PARAMETER(float, CompressedColorDCScale)
    SHADER_PARAMETER(float, CompressedColorRestScale)
    SHADER_PARAMETER(float, CompressedOpacityScale)
    SHADER_PARAMETER(float, CompressedScalingFactorScale)
    SHADER_PARAMETER_SRV(StructuredBuffer<float>, SplatCompressedPosition)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatOutputIndex)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatInstanceIndex)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatInstanceTransform)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatInstanceSettings)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, SplatInstanceFadeSettings)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, PreprocessCullVolumeData)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatCompressedMetadata)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatCompressedCovariance)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SplatCompressedSHData)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<float4>, RWPreprocessedSplats)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortValues)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyPreprocessCompressedCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyPreprocessCompressedCS);
    using FParameters = FHarmonyPreprocessCompressedPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyPreprocessCompressedCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyInitSortPaddingPassParams, )
    SHADER_PARAMETER(uint32, NumSplats)
    SHADER_PARAMETER(uint32, SortCount)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortValues)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyInitSortPaddingCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyInitSortPaddingCS);
    using FParameters = FHarmonyInitSortPaddingPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyInitSortPaddingCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyBitonicSortPassParams, )
    SHADER_PARAMETER(uint32, SortCount)
    SHADER_PARAMETER(uint32, K)
    SHADER_PARAMETER(uint32, J)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortValues)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyBitonicSortCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyBitonicSortCS);
    using FParameters = FHarmonyBitonicSortPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyBitonicSortCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyRadixHistogramPassParams, )
    SHADER_PARAMETER(uint32, SortCount)
    SHADER_PARAMETER(uint32, Shift)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSrcSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWGroupHistogram)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyRadixHistogramCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixHistogramCS);
    using FParameters = FHarmonyRadixHistogramPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixHistogramCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyRadixPrefixPassParams, )
    SHADER_PARAMETER(uint32, GroupCount)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWGroupHistogram)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyRadixPrefixCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixPrefixCS);
    using FParameters = FHarmonyRadixPrefixPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixPrefixCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyRadixBuildDispatchArgsPassParams, )
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWRadixDispatchArgs)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyRadixBuildDispatchArgsCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixBuildDispatchArgsCS);
    using FParameters = FHarmonyRadixBuildDispatchArgsPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixBuildDispatchArgsCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyRadixDigitBasePassParams, )
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWGroupHistogram)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDigitBase)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyRadixDigitBaseCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixDigitBaseCS);
    using FParameters = FHarmonyRadixDigitBasePassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixDigitBaseCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyRadixLoopbackPrefixPassParams, )
    SHADER_PARAMETER(uint32, PartitionStatusInvalid)
    SHADER_PARAMETER(uint32, PartitionStatusReduction)
    SHADER_PARAMETER(uint32, PartitionStatusPrefix)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDigitBase)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWGroupHistogram)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWPartitionState)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyRadixLoopbackPrefixCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixLoopbackPrefixCS);
    using FParameters = FHarmonyRadixLoopbackPrefixPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixLoopbackPrefixCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyRadixScatterPassParams, )
    SHADER_PARAMETER(uint32, SortCount)
    SHADER_PARAMETER(uint32, Shift)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSrcSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSrcSortValues)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWGroupOffsets)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDstSortKeys)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDstSortValues)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyRadixScatterCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixScatterCS);
    using FParameters = FHarmonyRadixScatterPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixScatterCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

class FHarmonyRadixScatterEvenCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixScatterEvenCS);
    using FParameters = FHarmonyRadixScatterPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixScatterEvenCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

class FHarmonyRadixScatterOddCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyRadixScatterOddCS);
    using FParameters = FHarmonyRadixScatterPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyRadixScatterOddCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyLayerPartitionBuildDispatchArgsPassParams, )
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWLayerPartitionDispatchArgs)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyLayerPartitionBuildDispatchArgsCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyLayerPartitionBuildDispatchArgsCS);
    using FParameters = FHarmonyLayerPartitionBuildDispatchArgsPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyLayerPartitionBuildDispatchArgsCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyLayerPartitionCountPassParams, )
    SHADER_PARAMETER(uint32, SortCount)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, PreprocessedSplats)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortedIndices)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWLayerGroupCounts)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyLayerPartitionCountCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyLayerPartitionCountCS);
    using FParameters = FHarmonyLayerPartitionCountPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyLayerPartitionCountCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyLayerPartitionPrefixPassParams, )
    SHADER_PARAMETER(uint32, GroupCount)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWLayerGroupCounts)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWLayerGroupOffsets)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWBackgroundDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWForegroundDrawIndexedArgs)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyLayerPartitionPrefixCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyLayerPartitionPrefixCS);
    using FParameters = FHarmonyLayerPartitionPrefixPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyLayerPartitionPrefixCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyLayerPartitionScatterPassParams, )
    SHADER_PARAMETER(uint32, SortCount)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, PreprocessedSplats)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWSortedIndices)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWDrawIndexedArgs)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWLayerGroupOffsets)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWBackgroundSortedIndices)
    SHADER_PARAMETER_UAV(RWStructuredBuffer<uint>, RWForegroundSortedIndices)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyLayerPartitionScatterCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyLayerPartitionScatterCS);
    using FParameters = FHarmonyLayerPartitionScatterPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyLayerPartitionScatterCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FTrianglePassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
    SHADER_PARAMETER(FMatrix44f, ViewToWorld)
    SHADER_PARAMETER_STRUCT(FEyeAdaptationParameters, EyeAdaptation)
    SHADER_PARAMETER_STRUCT(FLocalExposureParameters, LocalExposure)
    SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, EyeAdaptationBuffer)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSceneInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeBackgroundSplatsInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDepthInput)
    SHADER_PARAMETER(FVector3f, CameraWorldPos)
    SHADER_PARAMETER(uint32, NumSplats)
    SHADER_PARAMETER(uint32, NumCullVolumes)
    SHADER_PARAMETER(uint32, LayerSelect)
    SHADER_PARAMETER(uint32, EnableOpaqueDepthReject)
    SHADER_PARAMETER(uint32, UseBackgroundDepthComposeReject)
    SHADER_PARAMETER(uint32, UseBackgroundAmbiguityMask)
    SHADER_PARAMETER(uint32, UseBackgroundAmbiguityRectReject)
    SHADER_PARAMETER(uint32, UseOpaquePresenceMaskReject)
    SHADER_PARAMETER(uint32, EnableBackgroundAmbiguityResolve)
    SHADER_PARAMETER(uint32, OpaqueDepthRejectMode)
    SHADER_PARAMETER(uint32, OpaquePresenceMaskMode)
    SHADER_PARAMETER(uint32, DirectDrawDebugViewMode)
    SHADER_PARAMETER(uint32, SceneDepthWriteSelectionMode)
    SHADER_PARAMETER(uint32, ApplyInverseTonemap)
    SHADER_PARAMETER(uint32, InverseTonemapMethod)
    SHADER_PARAMETER(uint32, InverseTonemapLocalExposureMode)
    SHADER_PARAMETER(float, OpaqueDepthRejectBias)
    SHADER_PARAMETER(float, OpaqueDepthRejectAdaptiveBand)
    SHADER_PARAMETER(float, OpaqueDepthRejectFeather)
    SHADER_PARAMETER(float, OpaquePresenceDepthDistance)
    SHADER_PARAMETER(float, BackgroundDepthCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontAlphaFloor)
    SHADER_PARAMETER(float, BackgroundAmbiguityMinCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageRatioThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityThresholdFeather)
    SHADER_PARAMETER(float, BackgroundAmbiguityNeighborhoodRadiusPx)
    SHADER_PARAMETER(float, InverseTonemapScale)
    SHADER_PARAMETER(float, InverseTonemapGamma)
    SHADER_PARAMETER(float, InverseTonemapSaturationScale)
    SHADER_PARAMETER(float, FilmSlope)
    SHADER_PARAMETER(float, FilmToe)
    SHADER_PARAMETER(float, FilmShoulder)
    SHADER_PARAMETER(float, FilmBlackClip)
    SHADER_PARAMETER(float, FilmWhiteClip)
    SHADER_PARAMETER(uint32, UseAdaptiveFragmentMinAlpha)
    SHADER_PARAMETER(uint32, ClampFragmentMinAlpha)
    SHADER_PARAMETER(float, FragmentMinAlpha)
    SHADER_PARAMETER(float, FragmentNearMinAlpha)
    SHADER_PARAMETER(float, FragmentFarMinAlpha)
    SHADER_PARAMETER(float, FragmentNearMinAlphaDistance)
    SHADER_PARAMETER(float, FragmentFarMinAlphaDistance)
    SHADER_PARAMETER(float, SplatTailCutoff)
    SHADER_PARAMETER(FVector2f, Focal)
    SHADER_PARAMETER(FVector2f, DrawTargetInvSize)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, PreprocessedSplats)
    SHADER_PARAMETER_SRV(StructuredBuffer<float4>, CullVolumeData)
    SHADER_PARAMETER_SRV(StructuredBuffer<uint>, SortedIndices)
    SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint>, BackgroundAmbiguityRect)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeBackgroundSplatsTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDepthTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture3D, LumBilateralGrid)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BlurredLogLum)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDepthPointClampSampler)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FTriangleVS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FTriangleVS);
    using FParameters = FTrianglePassParams;
    SHADER_USE_PARAMETER_STRUCT(FTriangleVS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
        return true;
    }
};
//----------------------------------------------------------------

class FTrianglePS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FTrianglePS);
    using FParameters = FTrianglePassParams;
    class FWriteSplatAverageDepth : SHADER_PERMUTATION_BOOL("WRITE_SPLAT_AVERAGE_DEPTH");
    class FWriteSplatCoverage : SHADER_PERMUTATION_BOOL("WRITE_SPLAT_COVERAGE");
    using FPermutationDomain = TShaderPermutationDomain<FWriteSplatAverageDepth, FWriteSplatCoverage>;
    SHADER_USE_PARAMETER_STRUCT(FTrianglePS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        const FPermutationDomain PermutationVector(Parameters.PermutationId);
        return !(PermutationVector.Get<FWriteSplatAverageDepth>() && PermutationVector.Get<FWriteSplatCoverage>());
    }

    static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        const FPermutationDomain PermutationVector(Parameters.PermutationId);
        OutEnvironment.SetDefine(TEXT("WRITE_SPLAT_AVERAGE_DEPTH"), PermutationVector.Get<FWriteSplatAverageDepth>() ? 1 : 0);
        OutEnvironment.SetDefine(TEXT("WRITE_SPLAT_COVERAGE"), PermutationVector.Get<FWriteSplatCoverage>() ? 1 : 0);
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyComposeAfterDOFPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
    SHADER_PARAMETER_STRUCT(FEyeAdaptationParameters, EyeAdaptation)
    SHADER_PARAMETER_STRUCT(FLocalExposureParameters, LocalExposure)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSceneInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeBackgroundSplatsInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDepthInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeOutput)
    SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, EyeAdaptationBuffer)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSceneColorTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeBackgroundSplatsTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDepthTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture3D, LumBilateralGrid)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BlurredLogLum)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposePointClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDepthPointClampSampler)
    SHADER_PARAMETER(float, ComposeOpaqueDepthDistance)
    SHADER_PARAMETER(float, BackgroundDepthCoverageThreshold)
    SHADER_PARAMETER(float, ComposeBackgroundDepthBias)
    SHADER_PARAMETER(float, ComposeBackgroundDepthFeather)
    SHADER_PARAMETER(uint32, EnableBackgroundAmbiguityResolve)
    SHADER_PARAMETER(float, BackgroundAmbiguityMinCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageRatioThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityThresholdFeather)
    SHADER_PARAMETER(float, BackgroundAmbiguityNeighborhoodRadiusPx)
    SHADER_PARAMETER(uint32, ApplyInverseTonemap)
    SHADER_PARAMETER(uint32, InverseTonemapMethod)
    SHADER_PARAMETER(uint32, InverseTonemapLocalExposureMode)
    SHADER_PARAMETER(float, InverseTonemapScale)
    SHADER_PARAMETER(float, InverseTonemapGamma)
    SHADER_PARAMETER(float, InverseTonemapSaturationScale)
    SHADER_PARAMETER(float, FilmSlope)
    SHADER_PARAMETER(float, FilmToe)
    SHADER_PARAMETER(float, FilmShoulder)
    SHADER_PARAMETER(float, FilmBlackClip)
    SHADER_PARAMETER(float, FilmWhiteClip)
    SHADER_PARAMETER(uint32, ComposeUseBackgroundSplatsTexture)
    SHADER_PARAMETER(uint32, ComposeUseBackgroundDepthTexture)
    SHADER_PARAMETER(uint32, ComposePointSampleBackgroundProxy)
    SHADER_PARAMETER(uint32, ComposeDebugViewMode)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyComposeAfterDOFPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyComposeAfterDOFPS);
    using FParameters = FHarmonyComposeAfterDOFPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyComposeAfterDOFPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyDownsampleSceneColorPassParams, )
    SHADER_PARAMETER(FVector2f, DownsampleInputUVViewportBilinearMin)
    SHADER_PARAMETER(FVector2f, DownsampleInputUVViewportBilinearMax)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, DownsampleSceneColorTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, DownsampleLinearClampSampler)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyDownsampleSceneColorPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyDownsampleSceneColorPS);
    using FParameters = FHarmonyDownsampleSceneColorPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyDownsampleSceneColorPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyNormalizeBackgroundAverageDepthPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, NormalizeInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, NormalizeOutput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, NormalizeAccumTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, NormalizePointClampSampler)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyNormalizeBackgroundAverageDepthPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyNormalizeBackgroundAverageDepthPS);
    using FParameters = FHarmonyNormalizeBackgroundAverageDepthPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyNormalizeBackgroundAverageDepthPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyComposeProxyTexturePassParams, )
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeProxyInput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeProxyTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposePointClampSampler)
    SHADER_PARAMETER(uint32, ComposePointSampleProxyTexture)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyComposeProxyTexturePS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyComposeProxyTexturePS);
    using FParameters = FHarmonyComposeProxyTexturePassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyComposeProxyTexturePS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyWriteBackgroundSceneDepthPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDepthInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundSceneDepthInput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDepthTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundSceneDepthTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDepthPointClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundSceneDepthPointClampSampler)
    SHADER_PARAMETER(float, BackgroundDepthCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundOpaqueDepthDistance)
    SHADER_PARAMETER(float, BackgroundDepthBias)
    SHADER_PARAMETER(float, BackgroundDepthFeather)
    SHADER_PARAMETER(uint32, BackgroundUseOpaqueSceneMask)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyWriteBackgroundSceneDepthPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyWriteBackgroundSceneDepthPS);
    using FParameters = FHarmonyWriteBackgroundSceneDepthPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyWriteBackgroundSceneDepthPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyWriteBackgroundAmbiguityStencilPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSceneInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeBackgroundSplatsInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeOutput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDepthInput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeBackgroundSplatsTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDepthTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDepthPointClampSampler)
    SHADER_PARAMETER(float, ComposeOpaqueDepthDistance)
    SHADER_PARAMETER(float, BackgroundDepthCoverageThreshold)
    SHADER_PARAMETER(float, ComposeBackgroundDepthBias)
    SHADER_PARAMETER(float, ComposeBackgroundDepthFeather)
    SHADER_PARAMETER(uint32, EnableBackgroundAmbiguityResolve)
    SHADER_PARAMETER(float, BackgroundAmbiguityMinCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageRatioThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityThresholdFeather)
    SHADER_PARAMETER(float, BackgroundAmbiguityNeighborhoodRadiusPx)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyWriteBackgroundAmbiguityStencilPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyWriteBackgroundAmbiguityStencilPS);
    using FParameters = FHarmonyWriteBackgroundAmbiguityStencilPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyWriteBackgroundAmbiguityStencilPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyInitBackgroundAmbiguityRectPassParams, )
    SHADER_PARAMETER(uint32, OutputViewportWidth)
    SHADER_PARAMETER(uint32, OutputViewportHeight)
    SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWBackgroundAmbiguityRect)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyInitBackgroundAmbiguityRectCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyInitBackgroundAmbiguityRectCS);
    using FParameters = FHarmonyInitBackgroundAmbiguityRectPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyInitBackgroundAmbiguityRectCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyBuildBackgroundAmbiguityRectPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSceneInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeBackgroundSplatsInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDepthInput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeBackgroundSplatsTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDepthTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDepthPointClampSampler)
    SHADER_PARAMETER(uint32, OutputViewportWidth)
    SHADER_PARAMETER(uint32, OutputViewportHeight)
    SHADER_PARAMETER(uint32, EnableBackgroundAmbiguityResolve)
    SHADER_PARAMETER(float, ComposeOpaqueDepthDistance)
    SHADER_PARAMETER(float, BackgroundDepthCoverageThreshold)
    SHADER_PARAMETER(float, ComposeBackgroundDepthBias)
    SHADER_PARAMETER(float, ComposeBackgroundDepthFeather)
    SHADER_PARAMETER(float, BackgroundAmbiguityMinCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityFrontCoverageRatioThreshold)
    SHADER_PARAMETER(float, BackgroundAmbiguityThresholdFeather)
    SHADER_PARAMETER(float, BackgroundAmbiguityNeighborhoodRadiusPx)
    SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint>, RWBackgroundAmbiguityRect)
END_SHADER_PARAMETER_STRUCT()

class FHarmonyBuildBackgroundAmbiguityRectCS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyBuildBackgroundAmbiguityRectCS);
    using FParameters = FHarmonyBuildBackgroundAmbiguityRectPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyBuildBackgroundAmbiguityRectCS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyComposeSceneCoverageMaskPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSceneInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSeparateTranslucencyInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeBackgroundSplatsInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDepthInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundDirectCoverageInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundAmbiguityResolveInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeOutput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSeparateTranslucencyTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSeparateTranslucencyModulateTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSceneCoverageHistoryTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeBackgroundSplatsTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDepthTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundDirectCoverageTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundAmbiguityResolveTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeSceneCoverageHistorySampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDepthPointClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundDirectCoveragePointClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, BackgroundAmbiguityResolvePointClampSampler)
    SHADER_PARAMETER(float, ComposeOpaqueDepthDistance)
    SHADER_PARAMETER(float, ComposeTransparentPow)
    SHADER_PARAMETER(float, BackgroundDepthCoverageThreshold)
    SHADER_PARAMETER(float, ComposeBackgroundDepthBias)
    SHADER_PARAMETER(float, ComposeBackgroundDepthFeather)
    SHADER_PARAMETER(uint32, ComposeMaskCrossBlur)
    SHADER_PARAMETER(float, ComposeMaskCrossBlurPx)
    SHADER_PARAMETER(uint32, ComposeEnableSceneCoverageTemporalHistory)
    SHADER_PARAMETER(uint32, ComposeSceneCoverageHistoryCameraCut)
    SHADER_PARAMETER(float, ComposeSceneCoverageHistoryWeight)
    SHADER_PARAMETER(float, ComposeSceneCoverageHistoryVelocityFalloff)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyComposeSceneCoverageMaskPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyComposeSceneCoverageMaskPS);
    using FParameters = FHarmonyComposeSceneCoverageMaskPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyComposeSceneCoverageMaskPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyBuildAutoExposureMeterMaskPassParams, )
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, Output)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, BackgroundSplats)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ForegroundSplats)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, BackgroundSplatsTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ForegroundSplatsTexture)
    SHADER_PARAMETER_SAMPLER(SamplerState, LinearClampSampler)
    SHADER_PARAMETER(uint32, BackgroundCoverageInRed)
    SHADER_PARAMETER(uint32, ForegroundCoverageInRed)
    SHADER_PARAMETER(uint32, HasForegroundSplats)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyBuildAutoExposureMeterMaskPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyBuildAutoExposureMeterMaskPS);
    using FParameters = FHarmonyBuildAutoExposureMeterMaskPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyBuildAutoExposureMeterMaskPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FHarmonyComposeExtendedTonemapperPassParams, )
    SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
    SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTexturesStruct)
    SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, ComposeEyeAdaptationBuffer)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSceneInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeBloomInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeSeparateTranslucencyInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeForegroundSplatsInput)
    SHADER_PARAMETER_STRUCT(FScreenPassTextureViewportParameters, ComposeOutput)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSceneColorTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposePreserveSceneColorTexture)
    SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeColorGradingLUTTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeBloomTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSeparateTranslucencyTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeSceneCoverageMaskTexture)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, ComposeForegroundSplatsTexture)
	SHADER_PARAMETER_SAMPLER(SamplerState, ComposeLinearClampSampler)
    SHADER_PARAMETER_SAMPLER(SamplerState, ComposeColorGradingLUTSampler)
	SHADER_PARAMETER(FVector3f, ComposeSceneColorTint)
    SHADER_PARAMETER(FVector3f, ComposeTonemapperInverseGamma)
	SHADER_PARAMETER(float, ComposeTransparentPow)
	SHADER_PARAMETER(uint32, ComposeGeoMaskSource)
	SHADER_PARAMETER(uint32, ComposeTonemapMaskMode)
	SHADER_PARAMETER(float, ComposeBloomStrength)
	SHADER_PARAMETER(float, ComposeSaturation)
    SHADER_PARAMETER(FVector3f, ComposeColorContrast)
    SHADER_PARAMETER(FVector3f, ComposeColorGamma)
    SHADER_PARAMETER(FVector3f, ComposeColorGain)
    SHADER_PARAMETER(FVector3f, ComposeColorOffset)
    SHADER_PARAMETER(float, ComposeBlueCorrection)
    SHADER_PARAMETER(float, ComposeExpandGamut)
    SHADER_PARAMETER(float, ComposeToneCurveAmount)
    SHADER_PARAMETER(float, ComposeFilmSlope)
    SHADER_PARAMETER(float, ComposeFilmToe)
    SHADER_PARAMETER(float, ComposeFilmShoulder)
    SHADER_PARAMETER(float, ComposeFilmBlackClip)
    SHADER_PARAMETER(float, ComposeFilmWhiteClip)
    SHADER_PARAMETER(float, ComposePostToneSaturationScale)
    SHADER_PARAMETER(float, ComposeViewColorGradingStrength)
    SHADER_PARAMETER(float, ComposeExposureCompensationScale)
    SHADER_PARAMETER(float, ComposeShadowLift)
    SHADER_PARAMETER(float, ComposeShadowLiftPivot)
    SHADER_PARAMETER(float, ComposeShadowLiftSoftness)
    SHADER_PARAMETER(uint32, ComposeUseViewColorGrading)
    SHADER_PARAMETER(uint32, ComposeEnableBloom)
    SHADER_PARAMETER(uint32, ComposeEnableSceneTint)
    SHADER_PARAMETER(uint32, ComposeEnablePreExposure)
    SHADER_PARAMETER(uint32, ComposeUseForegroundSplatsTexture)
    SHADER_PARAMETER(uint32, ComposeUseNativeColorGradingLUT)
    SHADER_PARAMETER(uint32, ComposeTonemapperOutputDevice)
    SHADER_PARAMETER(uint32, ComposeTonemapperOutputGamut)
    SHADER_PARAMETER(float, ComposeTonemapperOutputMaxLuminance)
    SHADER_PARAMETER(float, ComposeLUTSize)
    SHADER_PARAMETER(float, ComposeInvLUTSize)
    SHADER_PARAMETER(float, ComposeLUTScale)
    SHADER_PARAMETER(float, ComposeLUTOffset)
    SHADER_PARAMETER(FVector4f, ComposeViewOverlayColor)
    SHADER_PARAMETER(uint32, ComposeApplyViewOverlay)
    SHADER_PARAMETER(uint32, ComposeApplyTonemapToWholeScene)
    SHADER_PARAMETER(uint32, ComposeBloomOnly)
    SHADER_PARAMETER(uint32, ComposeDebugViewMode)
    RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FHarmonyComposeExtendedTonemapperPS : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FHarmonyComposeExtendedTonemapperPS);
    using FParameters = FHarmonyComposeExtendedTonemapperPassParams;
    SHADER_USE_PARAMETER_STRUCT(FHarmonyComposeExtendedTonemapperPS, FGlobalShader);

    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return true;
    }
};

//----------------------------------------------------------------
// Render resources - buffers/textures/geometry etc.
struct FHelloVertex
{
    FVector2f Position;
    FVector2f UV;
};

class FTriangleVertexBuffer : public FVertexBuffer
{
public:
    void InitRHI(FRHICommandListBase& RHICmdList);
};

class FTriangleIndexBuffer : public FIndexBuffer
{
public:
    void InitRHI(FRHICommandListBase& RHICmdList);
};

class FTrianlgeVertexBufferElementDesc : public FRenderResource
{
public:
    FVertexDeclarationRHIRef VertexDeclarationRHI;
    virtual ~FTrianlgeVertexBufferElementDesc() {}

    virtual void InitRHI(FRHICommandListBase& RHICmdList);
    virtual void ReleaseRHI();
};

extern HARMONY_API TGlobalResource<FTriangleVertexBuffer> GTriangleVertexBuf;
extern HARMONY_API TGlobalResource<FTrianlgeVertexBufferElementDesc> GTriangleVertexBufElementDesc;
extern HARMONY_API TGlobalResource<FTriangleIndexBuffer> GTriangleIndexBuf;
