#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "HarmonyDataAsset.generated.h"

UENUM()
enum class EHarmonyAssetEncoding : uint8
{
    Expanded,
    Compressed
};

UENUM()
enum class EHarmonyCompressionPreset : uint8
{
    Balanced UMETA(DisplayName="Balanced"),
    Smallest UMETA(DisplayName="Smallest")
};

inline constexpr int32 GHarmonyMaxSHCoefficients = 16;
inline constexpr int32 GHarmonyFullCompressedSHWordsPerFeature = (GHarmonyMaxSHCoefficients * 3) / 4;
inline constexpr int32 GHarmonyCompressedPositionBucketSize = 256;
inline constexpr int32 GHarmonyCompressionFormatVersion = 2;

FORCEINLINE int32 GetHarmonyStoredSHCoefficientCount(const int32 SHDegree)
{
    switch (SHDegree)
    {
        case 0: return 1;
        case 1: return 4;
        case 2: return 9;
        case 3: return GHarmonyMaxSHCoefficients;
        default: return SHDegree > 3 ? GHarmonyMaxSHCoefficients : 0;
    }
}

FORCEINLINE int32 GetHarmonyCompressedSHWordCount(const int32 SHDegree)
{
    const int32 StoredCoefficientCount = GetHarmonyStoredSHCoefficientCount(SHDegree);
    return StoredCoefficientCount > 0 ? ((StoredCoefficientCount * 3 + 3) / 4) : 0;
}

FORCEINLINE int32 GetHarmonyStoredRestSHCoefficientCount(const int32 SHDegree)
{
    return FMath::Max(GetHarmonyStoredSHCoefficientCount(SHDegree) - 1, 0);
}

FORCEINLINE int32 GetHarmonyCompressedRestSHByteCount(const int32 SHDegree)
{
    return GetHarmonyStoredRestSHCoefficientCount(SHDegree) * 3;
}

FORCEINLINE int32 GetHarmonyCompressedPositionBucketCount(const int32 NumPoints)
{
    return NumPoints > 0
        ? ((NumPoints + GHarmonyCompressedPositionBucketSize - 1) / GHarmonyCompressedPositionBucketSize)
        : 0;
}

USTRUCT()
struct FHarmonyCompressedQuantizationChannel
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 ZeroPoint = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    float Scale = 1.0f;
};

USTRUCT()
struct FHarmonyCompressedQuantization
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel ColorDC;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel ColorRest;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel Opacity;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantizationChannel ScalingFactor;
};

USTRUCT()
struct FHarmonyCompressionFormat
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 Version = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    EHarmonyCompressionPreset Preset = EHarmonyCompressionPreset::Balanced;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesMortonOrdering = false;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesScaleCodebook = false;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesColorDCCodebook = false;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    bool bUsesSHPalette = false;
};

USTRUCT()
struct FHarmonyRawData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FString SourceFilename;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    int32 NumPoints = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    int32 SHDegree = 3;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FVector3f BoundsMin = FVector3f::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FVector3f BoundsMax = FVector3f::ZeroVector;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    float EstimatedSerializedSizeMB = 0.0f;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    EHarmonyAssetEncoding Encoding = EHarmonyAssetEncoding::Expanded;

    // Path of the asset this was decimated from, empty if not decimated.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    FString SourceAssetPath;

    // 1.0 = not decimated; otherwise the target keep-ratio passed to the decimate action.
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Debug")
    float DecimationTargetRatio = 1.0f;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Packed")
    float UnitScale = 100.0f;

    // Bulk packed data (direct upload layout) is intentionally hidden from details/blueprint panels.
    UPROPERTY()
    TArray<FVector4f> PosRadius;

    UPROPERTY()
    TArray<FVector4f> Color;

    UPROPERTY()
    TArray<FVector4f> Covariance;

    UPROPERTY()
    TArray<FVector4f> SHCoeffs;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 CompressedGeometryCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    int32 CompressedFeatureCount = 0;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressedQuantization CompressedQuantization;

    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat|Compressed")
    FHarmonyCompressionFormat CompressionFormat;

    UPROPERTY()
    TArray<FVector3f> CompressedPosition;

    UPROPERTY()
    TArray<uint16> CompressedPositionQuantized;

    UPROPERTY()
    TArray<FVector3f> CompressedPositionBucketCenter;

    UPROPERTY()
    TArray<FVector3f> CompressedPositionBucketExtent;

    UPROPERTY()
    TArray<uint32> CompressedMetadata;

    UPROPERTY()
    TArray<uint16> CompressedMetadataCompact;

    UPROPERTY()
    TArray<uint8> CompressedMetadataCodebookLabels;

    UPROPERTY()
    TArray<uint16> CompressedMetadataCodebook;

    UPROPERTY()
    TArray<uint32> CompressedCovariance;

    UPROPERTY()
    TArray<uint32> CompressedSHData;

    UPROPERTY()
    TArray<uint8> CompressedSHDataBytes;

    UPROPERTY()
    TArray<uint8> CompressedColorDCCodebookLabels;

    UPROPERTY()
    TArray<uint32> CompressedColorDCCodebook;

    int32 GetExpandedSHStorageStride() const
    {
        const int32 CompactCoefficientCount = GetHarmonyStoredSHCoefficientCount(SHDegree);
        if (CompactCoefficientCount <= 0)
        {
            return 0;
        }

        if (NumPoints > 0 && SHCoeffs.Num() >= (NumPoints * GHarmonyMaxSHCoefficients))
        {
            return GHarmonyMaxSHCoefficients;
        }

        return CompactCoefficientCount;
    }

    int32 GetCompactExpandedSHStorageCount() const
    {
        const int32 CompactCoefficientCount = GetHarmonyStoredSHCoefficientCount(SHDegree);
        return (NumPoints > 0 && CompactCoefficientCount > 0)
            ? (NumPoints * CompactCoefficientCount)
            : 0;
    }

    int32 GetCompressedSHStorageStrideWords() const
    {
        const int32 CompactWordCount = GetHarmonyCompressedSHWordCount(SHDegree);
        if (CompactWordCount <= 0)
        {
            return 0;
        }

        if (CompressedFeatureCount <= 0)
        {
            return 0;
        }

        if (CompressedFeatureCount > 0 &&
            CompressedSHData.Num() >= (CompressedFeatureCount * GHarmonyFullCompressedSHWordsPerFeature))
        {
            return GHarmonyFullCompressedSHWordsPerFeature;
        }

        return CompressedSHData.Num() >= (CompressedFeatureCount * CompactWordCount)
            ? CompactWordCount
            : 0;
    }

    int32 GetCompressedSHStorageStrideBytes() const
    {
        if (!HasCompressedColorDCCodebook())
        {
            return 0;
        }

        const int32 RestByteCount = GetHarmonyCompressedRestSHByteCount(SHDegree);
        if (RestByteCount == 0)
        {
            return 0;
        }

        return (CompressedFeatureCount > 0 && CompressedSHDataBytes.Num() >= (CompressedFeatureCount * RestByteCount))
            ? RestByteCount
            : 0;
    }

    bool HasBucketedCompressedPositions() const
    {
        const int32 BucketCount = GetHarmonyCompressedPositionBucketCount(NumPoints);
        return BucketCount > 0 &&
            CompressedPositionQuantized.Num() >= (NumPoints * 3) &&
            CompressedPositionBucketCenter.Num() >= BucketCount &&
            CompressedPositionBucketExtent.Num() >= BucketCount;
    }

    bool HasLegacyCompressedPositions() const
    {
        return NumPoints > 0 && CompressedPosition.Num() >= NumPoints;
    }

    bool HasCompactCompressedMetadata() const
    {
        return NumPoints > 0 &&
            CompressedGeometryCount == NumPoints &&
            CompressedFeatureCount == NumPoints &&
            CompressedMetadataCompact.Num() >= NumPoints;
    }

    bool HasCompressedMetadataCodebook() const
    {
        return NumPoints > 0 &&
            CompressionFormat.bUsesScaleCodebook &&
            CompressedGeometryCount == NumPoints &&
            CompressedFeatureCount == NumPoints &&
            CompressedMetadataCodebookLabels.Num() >= NumPoints &&
            CompressedMetadataCodebook.Num() > 0 &&
            CompressedMetadataCodebook.Num() <= 256;
    }

    bool HasCompressedColorDCCodebook() const
    {
        return CompressedFeatureCount > 0 &&
            CompressionFormat.bUsesColorDCCodebook &&
            CompressedColorDCCodebookLabels.Num() >= CompressedFeatureCount &&
            CompressedColorDCCodebook.Num() > 0 &&
            CompressedColorDCCodebook.Num() <= 256;
    }

    int32 GetCompressedMetadataStrideWords() const
    {
        if (NumPoints <= 0)
        {
            return 0;
        }

        if (CompressedMetadata.Num() >= (NumPoints * 3))
        {
            return 3;
        }

        return ((CompressedGeometryCount == NumPoints && CompressedFeatureCount == NumPoints && CompressedMetadata.Num() >= NumPoints) ||
                HasCompactCompressedMetadata() ||
                HasCompressedMetadataCodebook())
            ? 1
            : 0;
    }

    void UpdateDebugStats()
    {
        uint64 Bytes = 0;
        if (Encoding == EHarmonyAssetEncoding::Compressed)
        {
            Bytes =
                static_cast<uint64>(CompressedPosition.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedPositionQuantized.Num()) * sizeof(uint16) +
                static_cast<uint64>(CompressedPositionBucketCenter.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedPositionBucketExtent.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedMetadata.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedMetadataCompact.Num()) * sizeof(uint16) +
                static_cast<uint64>(CompressedMetadataCodebookLabels.Num()) * sizeof(uint8) +
                static_cast<uint64>(CompressedMetadataCodebook.Num()) * sizeof(uint16) +
                static_cast<uint64>(CompressedCovariance.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedSHData.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedSHDataBytes.Num()) * sizeof(uint8) +
                static_cast<uint64>(CompressedColorDCCodebookLabels.Num()) * sizeof(uint8) +
                static_cast<uint64>(CompressedColorDCCodebook.Num()) * sizeof(uint32);
        }
        else
        {
            Bytes =
                static_cast<uint64>(PosRadius.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(Color.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(Covariance.Num()) * sizeof(FVector4f) +
                static_cast<uint64>(SHCoeffs.Num()) * sizeof(FVector4f);
        }

        EstimatedSerializedSizeMB = static_cast<float>(Bytes) / (1024.0f * 1024.0f);
    }

    void Reset()
    {
        SourceFilename.Empty();
        NumPoints = 0;
        SHDegree = 3;
        BoundsMin = FVector3f::ZeroVector;
        BoundsMax = FVector3f::ZeroVector;
        EstimatedSerializedSizeMB = 0.0f;
        Encoding = EHarmonyAssetEncoding::Expanded;
        SourceAssetPath.Empty();
        DecimationTargetRatio = 1.0f;
        UnitScale = 100.0f;
        PosRadius.Reset();
        Color.Reset();
        Covariance.Reset();
        SHCoeffs.Reset();
        CompressedGeometryCount = 0;
        CompressedFeatureCount = 0;
        CompressedQuantization = FHarmonyCompressedQuantization{};
        CompressedPosition.Reset();
        CompressedPositionQuantized.Reset();
        CompressedPositionBucketCenter.Reset();
        CompressedPositionBucketExtent.Reset();
        CompressedMetadata.Reset();
        CompressedMetadataCompact.Reset();
        CompressedMetadataCodebookLabels.Reset();
        CompressedMetadataCodebook.Reset();
        CompressedCovariance.Reset();
        CompressedSHData.Reset();
        CompressedSHDataBytes.Reset();
        CompressedColorDCCodebookLabels.Reset();
        CompressedColorDCCodebook.Reset();
        CompressionFormat = FHarmonyCompressionFormat{};
    }
};

UCLASS()
class HARMONY_API UHarmonyDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, Category = "Gaussian Splat")
    FHarmonyRawData SplatData;

    //~ UObject interface
    // Exposes key splat metadata (point count, encoding, data size) as Asset Registry tags so they
    // appear immediately in the Content Browser hover tooltip for all assets — including ones created
    // in-session via duplication, which have not yet been scanned from disk by the Asset Registry.
    virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
    //~ End UObject interface
};
