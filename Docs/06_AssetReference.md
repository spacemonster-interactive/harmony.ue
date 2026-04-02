# Asset Reference

## UHarmonyDataAsset

The core data asset type for all Gaussian Splat content in Harmony. Created automatically on import; appears in the Content Browser.

---

## Properties

### General

| Property | Type | Description |
|---|---|---|
| `NumPoints` | `int32` | Total number of Gaussian splat points in this asset |
| `Encoding` | `EHarmonyAssetEncoding` | Whether the asset is `Expanded` or `Compressed` |

### Expanded Data

Present when `Encoding == Expanded`. All arrays have `NumPoints` entries.

| Property | Type | Description |
|---|---|---|
| `PosRadius` | `TArray<FVector4f>` | Per-point XYZ world position and radius |
| `Color` | `TArray<FVector4f>` | Per-point RGBA colour (alpha in `.W`) |
| `Covariance` | `TArray<FVector4f>` | Per-point 3D covariance, two `FVector4f` per point |
| `SHCoeffs` | `TArray<FVector4f>` | Spherical harmonic coefficients, 16 `FVector4f` per point |

### Compressed Data

Present when `Encoding == Compressed`.

| Property | Type | Description |
|---|---|---|
| `CompressedPosition` | `TArray<FVector3f>` | Per-point XYZ world position |
| `CompressedMetadata` | `TArray<uint32>` | 3 words per point: packed rotation, opacity, scale |
| `CompressedCovariance` | `TArray<uint32>` | 3 words per point: float16 pairs normalized by trace |
| `CompressedSHData` | `TArray<uint32>` | 12 words per point: int8-quantized SH coefficients |
| `CompressedGeometryCount` | `int32` | Unique geometry entries (may be < `NumPoints` for sparse NPZ assets) |
| `CompressedFeatureCount` | `int32` | Unique feature entries (may be < `NumPoints` for sparse NPZ assets) |

### Quantization Parameters

Stored with Compressed assets to allow dequantization.

| Property | Description |
|---|---|
| `Q.Position` | Scale and zero-point for position quantization |
| `Q.Opacity` | Scale and zero-point for opacity (ZeroPoint = -128) |
| `Q.ColorDC` | Scale for SH DC band dequantization |
| `Q.ColorRest` | Scale for SH higher-band dequantization (ZeroPoint = 0) |

---

## Debug Stats Tooltip

Hovering over a `UHarmonyDataAsset` in the Content Browser shows a tooltip with:

| Field | Description |
|---|---|
| **Disk Size** | Estimated serialized size in MB |
| **Points** | Number of splat points (`NumPoints`) |
| **Encoding** | `Expanded` or `Compressed` |

> The Disk Size reflects the actual size of the asset as saved. Assets created via **Convert to Compressed** or **Convert to Expanded** report the correct size for their encoding immediately, they are saved fresh and do not inherit stale stats from their source.

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

Right-clicking a `UHarmonyDataAsset` in the Content Browser exposes conversion actions based on the asset's current encoding:

| Action | Available when | Result |
|---|---|---|
| **Convert to Compressed** | `Encoding == Expanded` | Creates `AssetName_Compressed` as a new Compressed asset |
| **Convert to Expanded** | `Encoding == Compressed` (non-sparse) | Creates `AssetName_Expanded` as a new Expanded asset (lossy) |

See [Compression](Compression.md) for full details.
