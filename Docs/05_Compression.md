# Compression

Harmony stores Gaussian Splat data in one of two internal encodings: **Expanded** and **Compressed**. You can convert between them from the Content Browser using the right-click context menu.

---

## Expanded vs Compressed

### Expanded

Full-precision float32 arrays, one entry per splat point. This is the natural output of all importers except NPZ.

| Array | Layout | Description |
|---|---|---|
| `PosRadius` | `FVector4f × N` | XYZ position + radius |
| `Color` | `FVector4f × N` | RGBA (alpha in `.W`) |
| `Covariance` | `FVector4f × 2N` | 3D covariance (two vec4 per point) |
| `SHCoeffs` | `FVector4f × 16N` | Spherical harmonic coefficients (up to degree 3) |

Expanded assets are larger on disk but retain full precision and are the authoritative source format for re-compression.

### Compressed

Quantized, GPU-ready packing. Each point is stored in a compact integer representation designed for efficient upload and rendering.

| Array | Layout | Description |
|---|---|---|
| `CompressedPosition` | `FVector3f × N` | World-space XYZ |
| `CompressedMetadata` | `uint32 × 3N` | Packed rotation, opacity, scale |
| `CompressedCovariance` | `uint32 × 3N` | float16 pairs, normalized by covariance trace |
| `CompressedSHData` | `uint32 × 12N` | int8-quantized SH coefficients |

Compressed assets are significantly smaller on disk and faster to upload to the GPU.

---

## Converting Assets

Both conversion actions are available by right-clicking any `UHarmonyDataAsset` in the Content Browser.

### Convert to Compressed

Right-click an **Expanded** asset → **Convert to Compressed**

- Creates a new asset with a `_Compressed` suffix alongside the original
- The original asset is left untouched
- The new asset's **Disk Size** tooltip reflects the compressed size immediately

### Convert to Expanded

Right-click a **Compressed** asset → **Convert to Expanded**

> ⚠️ **Lossy operation.** Decompression reconstructs float32 values from quantized integers. The result is not bit-for-bit identical to the original Expanded asset.

- A warning dialog explains the precision loss before proceeding
- Creates a new asset with an `_Expanded` suffix alongside the original
- The original Compressed asset is left untouched

---

## Limitations

### Sparse NPZ assets cannot be decompressed

NPZ-imported assets may use **sparse indexing**, where geometry or feature arrays are shared across fewer unique entries than total splat points (`CompressedGeometryCount < NumPoints` or `CompressedFeatureCount < NumPoints`).

These assets are flagged as non-decompressible. The **Convert to Expanded** option will report this and direct you to re-import from the original source file (`.ply`, `.spz`, etc.) if an Expanded version is needed.

### Precision loss on round-trip

Compress → Decompress is lossy:

- **SH coefficients** are quantized to int8 (symmetric, ZeroPoint = 0 for higher bands, ZeroPoint = -128 for opacity)
- **Covariance** is stored as float16 pairs normalized by the covariance trace; the trace is reconstructed from the packed scale factor on decompress
- Fine details in colour and shape may be visibly degraded after a round-trip

For archival or re-export purposes, always keep the original Expanded asset.

---

## Choosing a Format

| Situation | Recommended format |
|---|---|
| Authoring / iterating | **Expanded** full precision, re-compressible |
| Shipping / runtime | **Compressed** smaller, faster GPU upload |
| NPZ pipeline output | **Compressed** already quantized at source |
