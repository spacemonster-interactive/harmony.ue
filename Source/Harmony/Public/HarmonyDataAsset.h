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

    UPROPERTY()
    TArray<FVector3f> CompressedPosition;

    UPROPERTY()
    TArray<uint32> CompressedMetadata;

    UPROPERTY()
    TArray<uint32> CompressedCovariance;

    UPROPERTY()
    TArray<uint32> CompressedSHData;

    void UpdateDebugStats()
    {
        uint64 Bytes = 0;
        if (Encoding == EHarmonyAssetEncoding::Compressed)
        {
            Bytes =
                static_cast<uint64>(CompressedPosition.Num()) * sizeof(FVector3f) +
                static_cast<uint64>(CompressedMetadata.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedCovariance.Num()) * sizeof(uint32) +
                static_cast<uint64>(CompressedSHData.Num()) * sizeof(uint32);
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
        UnitScale = 100.0f;
        PosRadius.Reset();
        Color.Reset();
        Covariance.Reset();
        SHCoeffs.Reset();
        CompressedGeometryCount = 0;
        CompressedFeatureCount = 0;
        CompressedQuantization = FHarmonyCompressedQuantization{};
        CompressedPosition.Reset();
        CompressedMetadata.Reset();
        CompressedCovariance.Reset();
        CompressedSHData.Reset();
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
