# Importing Gaussian Splats

Harmony supports four Gaussian Splat file formats. All formats import into the same asset type/ `UHarmonyDataAsset` | but differ in what data they carry and how it is stored internally.

---

## How to Import

1. Open the **Content Browser** in Unreal Editor
2. Drag your splat file directly onto the Content Browser, or use **Import** from the toolbar
3. Harmony automatically detects the format from the file extension
4. A `UHarmonyDataAsset` is created in the current folder

No import dialog or settings are required. The asset is ready to use immediately after import.

---

## Supported Formats

### `.ply` | Gaussian Splat PLY

The most common authoring export format. Standard PLY binary with per-point properties for position, scale, rotation, opacity, and spherical harmonic colour coefficients.

- Imports as **Expanded** encoding (full float32 arrays)
- Supports all SH degrees (DC only through degree 3)
- Scale is expected as log-encoded values in the PLY; Harmony decodes these automatically

### `.splat` | Packed Binary

A compact binary format used by several web-based Gaussian Splat viewers. Each record is 32 bytes containing packed position, scale, rotation, and colour.

- Imports as **Expanded** encoding
- Colour is stored as a pre-activated value (no sigmoid needed); Harmony converts to SH DC internally
- No higher-order SH coefficients, DC colour only

### `.spz` | Niantic SPZ

A gzip-compressed binary format developed by Niantic. Contains position, rotation, scale, and optional SH coefficients in a compact quantized layout that Harmony decompresses on import.

- Imports as **Expanded** encoding
- Supports optional higher-order SH up to degree 3

### `.npz` | NumPy Archive

A NumPy `.npz` file produced by training pipelines that output Harmony-compatible compressed arrays directly. Unlike the other formats, NPZ files arrive pre-quantized.

- Imports as **Compressed** encoding, no conversion step required
- Supports **sparse indexing**: geometry and feature arrays may be shared across fewer unique entries than total points (`CompressedGeometryCount < NumPoints` or `CompressedFeatureCount < NumPoints`)
- Sparse assets cannot be decompressed back to Expanded format in-place; re-import from the source `.ply` or `.spz` if an Expanded version is needed

---

## After Import

Once imported, the asset appears in the Content Browser with a thumbnail preview. You can:

- Drag it into the level to place it as an `AHarmonyActor`
- Right-click it to **Convert to Compressed** (if it imported as Expanded) see [Compression](Compression.md)
- Hover over it in the Content Browser to see the **Disk Size** and point count in the tooltip

---

## Adding New Formats

Each format is handled by a dedicated importer class in `Source/HarmonyEditor/Private/Import/`:

| File | Format |
|---|---|
| `HarmonyPlyImporter.cpp` | `.ply` |
| `HarmonySplatImporter.cpp` | `.splat` |
| `HarmonySpzImporter.cpp` | `.spz` |
| `HarmonyNpzImporter.cpp` | `.npz` |

Each importer exposes a single public function in the `Harmony::Editor` namespace and fills a `FHarmonyRawData` struct. `UHarmonyPlyFactory` dispatches to the correct importer based on file extension.
