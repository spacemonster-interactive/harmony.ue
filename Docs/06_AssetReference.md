# Asset Reference

## UHarmonyDataAsset

The core data asset type used for Gaussian Splat content. Created automatically on import and shown in the Content Browser.

---

## Properties

### General

| Property | Type | Description |
|---|---|---|
| `NumPoints` | `int32` | Total number of Gaussian splat points in this asset |
| `SHDegree` | `int32` | Highest spherical harmonic degree stored in the asset |
| `Encoding` | `EHarmonyAssetEncoding` | Whether the asset is `Expanded` or `Compressed` |
| `BoundsMin` / `BoundsMax` | `FVector3f` | Imported bounds for the splat set |
| `EstimatedSerializedSizeMB` | `float` | Estimated serialized payload size used for Content Browser stats |
| `UnitScale` | `float` | Import-space to Unreal-space scale factor recorded with the asset |

### Expanded Data

Present when `Encoding == Expanded`.

| Property | Type | Description |
|---|---|---|
| `PosRadius` | `TArray<FVector4f>` | Per-point XYZ world position and radius |
| `Color` | `TArray<FVector4f>` | Per-point RGBA colour (alpha in `.W`) |
| `Covariance` | `TArray<FVector4f>` | Per-point 3D covariance, stored as two `FVector4f` values per point |
| `SHCoeffs` | `TArray<FVector4f>` | Spherical harmonic coefficients. New assets store only the coefficients required by `SHDegree` |

Notes:

- `PosRadius` and `Color` have `NumPoints` entries
- `Covariance` has `2 * NumPoints` entries
- `SHCoeffs` is compact by SH degree on newly written assets:
  - degree 0: 1 coefficient per point
  - degree 1: 4 coefficients per point
  - degree 2: 9 coefficients per point
  - degree 3: 16 coefficients per point
- Older Expanded assets may still carry a full 16-coefficient stride for compatibility

### Compressed Data

Present when `Encoding == Compressed`.

| Property | Type | Description |
|---|---|---|
| `CompressedGeometryCount` | `int32` | Unique geometry entries (may be < `NumPoints` for sparse NPZ assets) |
| `CompressedFeatureCount` | `int32` | Unique feature entries (may be < `NumPoints` for sparse NPZ assets) |
| `CompressedQuantization` | `FHarmonyCompressedQuantization` | Quantization parameters used to reconstruct compressed channels |
| `CompressionFormat` | `FHarmonyCompressionFormat` | Format version and compact-layout flags used for this asset |

Compressed payload fields are intentionally low-level. Not every compressed asset uses every array.

#### Position Storage

| Property | Type | Description |
|---|---|---|
| `CompressedPosition` | `TArray<FVector3f>` | Legacy float32 compressed position storage |
| `CompressedPositionQuantized` | `TArray<uint16>` | Quantized local XYZ offsets for bucketed position storage |
| `CompressedPositionBucketCenter` | `TArray<FVector3f>` | Per-bucket position centres |
| `CompressedPositionBucketExtent` | `TArray<FVector3f>` | Per-bucket half-extents used to decode quantized positions |

#### Metadata Storage

| Property | Type | Description |
|---|---|---|
| `CompressedMetadata` | `TArray<uint32>` | Full packed metadata words |
| `CompressedMetadataCompact` | `TArray<uint16>` | Compact packed metadata used by current dense compressed assets |
| `CompressedMetadataCodebookLabels` | `TArray<uint8>` | Optional labels into a compact metadata codebook |
| `CompressedMetadataCodebook` | `TArray<uint16>` | Optional metadata codebook table |

#### Geometry and SH Storage

| Property | Type | Description |
|---|---|---|
| `CompressedCovariance` | `TArray<uint32>` | Packed covariance representation |
| `CompressedSHData` | `TArray<uint32>` | Word-packed SH storage |
| `CompressedSHDataBytes` | `TArray<uint8>` | Compact byte-packed SH rest coefficients used by newer assets |
| `CompressedColorDCCodebookLabels` | `TArray<uint8>` | Legacy compatibility field for codebook-compressed DC colour |
| `CompressedColorDCCodebook` | `TArray<uint32>` | Legacy compatibility field for codebook-compressed DC colour |

### Quantization Parameters

Stored with Compressed assets to allow dequantization.

| Property | Description |
|---|---|
| `ColorDC` | SH DC-band quantization parameters |
| `ColorRest` | Higher-band SH quantization parameters |
| `Opacity` | Opacity quantization parameters |
| `ScalingFactor` | Covariance/scaling-factor reconstruction parameters |

### Compression Format Metadata

Stored with Compressed assets for compatibility and debugging.

| Property | Description |
|---|---|
| `Version` | Compressed-format version written into the asset |
| `Preset` | Historical compression preset enum kept for compatibility/debugging |
| `bUsesMortonOrdering` | Whether the asset was reordered spatially before compression |
| `bUsesScaleCodebook` | Whether compact metadata codebook storage is present |
| `bUsesColorDCCodebook` | Legacy compatibility flag for codebook-compressed DC colour |
| `bUsesSHPalette` | Reserved flag for alternate SH storage modes |

---

## Debug Stats Tooltip

Hovering over a `UHarmonyDataAsset` in the Content Browser shows a tooltip with:

| Field | Description |
|---|---|
| **Disk Size** | Estimated serialized size in MB |
| **Points** | Number of splat points (`NumPoints`) |
| **Encoding** | `Expanded` or `Compressed` |

> The Disk Size reflects the asset payload as currently stored. Imported assets and assets created via **Convert to Compressed** report updated stats as soon as they are saved.

---

## EHarmonyAssetEncoding

```cpp
enum class EHarmonyAssetEncoding : uint8
{
    Expanded,    // Full float32 arrays
    Compressed,  // Quantized GPU-ready packing
};
```

---

## Context Menu Actions

Right-clicking a `UHarmonyDataAsset` in the Content Browser currently exposes:

| Action | Available when | Result |
|---|---|---|
| **Convert to Compressed** | `Encoding == Expanded` | Creates `AssetName_Compressed` as a new Compressed asset |

Expanded-source formats can also be auto-compressed during import through **Edit -> Project Settings -> Plugins -> Harmony Import -> Import Compressed**.

See [Compression](05_Compression.md) for full details.
