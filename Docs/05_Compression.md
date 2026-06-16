# Compression

Gaussian Splat data is stored in one of two internal encodings: **Expanded** and **Compressed**.

Compression is separate from source file format support. A `.ply`, `.spz`, or `.sog` can all end up as either an Expanded or a Compressed `UHarmonyDataAsset`, depending on how it is imported.

---

## How Compressed Assets Are Created

Compressed assets are currently created in three ways:

- Import an expanded-source format with **Edit -> Project Settings -> Plugins -> Harmony Import -> Import Compressed** enabled
- Import a `.npz`, which already arrives as compressed plugin-compatible data
- Right-click an Expanded `UHarmonyDataAsset` in the Content Browser and choose **Convert to Compressed**

Only **Convert to Compressed** is currently exposed in the Content Browser. If you need an Expanded asset from an expanded-source format such as `.ply`, `.spz`, `.sog`, `.splat`, or `.ksplat`, import from the original source file with `Import Compressed` disabled. `.npz` remains a compressed import path.

---

## Expanded vs Compressed

### Expanded

Expanded assets store float data in a straightforward per-splat layout.

| Array | Layout | Description |
|---|---|---|
| `PosRadius` | `FVector4f × N` | XYZ position + radius |
| `Color` | `FVector4f × N` | RGBA (alpha in `.W`) |
| `Covariance` | `FVector4f × 2N` | Full covariance, two vectors per splat |
| `SHCoeffs` | Compact by SH degree | Stores only the coefficients required by the asset's `SHDegree` on newly written assets |

Expanded assets are the easiest format to inspect, debug, and recompress. They are also the best choice if you want to preserve a high-precision editable copy inside the project.

### Compressed

Compressed assets use a compact on-disk layout. Newly written compressed assets are built with one built-in compression recipe; there are currently no user-facing compression presets.

The current compressed layout includes:

- **Morton reordering** before packing, so nearby splats tend to sit near each other in memory
- **Bucketed positions**: positions are stored as `uint16` local offsets inside fixed-size buckets, with per-bucket centre and extent tables
- **Compact metadata**: opacity and covariance scale-factor data are packed into a single compact word per splat
- **Optional exact metadata-word dictionary**: if the compact opacity/scale-factor word contains at most `256` exact values and the dictionary is smaller on disk, labels plus that dictionary are stored instead of the direct array. This is separate from the older DC-colour codebook path, which is no longer written by the current compressor
- **Packed covariance**: covariance is normalized by trace and stored as packed half-float pairs
- **Compact SH storage**: SH DC and higher-order bands are quantized separately, and only the coefficients required by the asset's `SHDegree` are stored

Typical storage characteristics for newly written compressed assets:

| Channel | Storage |
|---|---|
| Position | Bucketed `uint16` XYZ offsets + per-bucket float bounds |
| Opacity / scale factor | Packed compact metadata word or metadata codebook |
| Covariance | `3` packed `uint32` words per splat |
| SH | Int8-quantized, compact by SH degree |

Compressed assets are much smaller on disk than Expanded assets and are the recommended format for normal runtime use.

---

## SH Storage By Degree

SH data is stored by the asset's declared `SHDegree`, not always at a fixed degree-3 width.

| SH Degree | Stored coefficients per splat | Compressed SH words per splat |
|---|---:|---:|
| `0` | `1` | `1` |
| `1` | `4` | `3` |
| `2` | `9` | `7` |
| `3` | `16` | `12` |

This reduces disk size significantly for DC-only and lower-degree assets.

---

## Runtime Upload

The compressed **disk** layout is not identical to the compressed **runtime upload** layout.

At upload time, some compact disk-side representations may be reconstructed into the buffers expected by the render path. In particular:

- bucketed positions are decoded back to float positions before upload
- metadata codebooks are expanded back to direct metadata words before upload

Covariance and SH quantization remain part of the compressed runtime path.

This means `.uasset` size can drop without every disk-side optimization automatically producing the same reduction in VRAM usage.

---

## Converting Assets

### Convert to Compressed

Right-click an **Expanded** asset → **Convert to Compressed**

- Creates a new asset with a `_Compressed` suffix alongside the original
- Leaves the original asset untouched
- Writes the asset in the current compressed layout

If you want newly imported assets to stay Expanded, disable **Import Compressed** in **Project Settings** before importing.

---

## Limitations

### Compression Is Lossy

Compression is intended for runtime efficiency, not as a bit-exact archival format.

Current compression introduces loss in several places:

- positions are quantized into bucketed `uint16` offsets
- opacity is quantized
- covariance trace scale factor is quantized
- covariance components are stored as packed half-floats after normalization
- SH coefficients are quantized to int8

Keep the original source file, or an Expanded asset, if you need a high-precision reference.

### Only Expanded SH Degree 0-3 Assets Can Be Compressed

The current in-editor compression path only accepts Expanded assets with:

- valid full Expanded payloads
- `SHDegree` in the range `0..3`

Assets outside that range remain importable as Expanded data, but they cannot be passed through the current compression path.

### Sparse `.npz` Assets Are Special-Case Compressed Inputs

`.npz` files may contain sparse geometry or feature indexing, where the compressed geometry or feature arrays have fewer unique entries than total splat points.

These are valid compressed assets, but they behave more like native runtime data than like a straightforward Expanded source import.

### Some Source Formats Are Already Lossy

`.spz`, `.sog`, and `.ksplat` are already quantized source formats. Even if you import them as Expanded assets, they may not match a reference `.ply` bit-for-bit.

### Compressed Point Order Differs From The Source

Splats are currently Morton-reordered before new compressed assets are written. This improves locality, but it means the raw splat order in a compressed asset should not be expected to match the original source file.

---

## Choosing a Format

| Situation | Recommended format |
|---|---|
| Authoring / inspection / debugging | **Expanded** |
| Shipping / runtime use | **Compressed** |
| Pipeline already outputs compressed `.npz` data | `.npz` |

See [Importing](02_Importing.md) for supported source formats and import workflow.
