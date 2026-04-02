# Harmony

Gaussian Splat rendering plugin for Unreal Engine 5.

Harmony imports 3D Gaussian Splat captures from common authoring tools, stores them as `UHarmonyDataAsset` assets, and renders them in the editor and at runtime using the UE5 RHI.


---

> **Pre-release — Alpha**
>
> Harmony is in active development and has not yet reached a stable release. APIs, asset formats, actor properties, and project settings are all subject to change — some significantly — between versions. Do not build production pipelines against the current interface without expecting to update them as the plugin evolves.

---

## Requirements

| | |
|---|---|
| **Unreal Engine** | 5.7 or later |
| **Platforms** | Windows, macOS |
| **Rendering** | SM6 / Metal |

---

## Showcase

| | |
|---|---|
| **Live Demo — Windows** | [Download demo](https://spacemonster.s3.us-east-1.amazonaws.com/harmony/demo/index.html) |
| **Live Demo — macOS** | [Download demo](https://spacemonster.s3.us-east-1.amazonaws.com/harmony/demo/index.html) |
| **Video Overview** | [Watch on YouTube](https://youtu.be/ZiBRiuQxpFk) |
| **Sample Project** | *(coming soon)* |

The live demos and sample project include Gaussian Splat assets that are not part of the plugin repository. They are provided separately as a way to showcase the plugin with real-world content.

---

## Quick Start

1. Copy the **Harmony** folder into your project's `Plugins/` directory
2. Open your project — compile when prompted
3. Enable the plugin via *Edit → Plugins → Rendering → Harmony* if not already active
4. Drag a supported splat file into the **Content Browser** to import it as a `UHarmonyDataAsset`
5. Add a `AHarmonyActor` to the level and assign the data asset in its Details panel
6. Press **Play** or use the viewport to preview

---

## Documentation

| | |
|---|---|
| [01 — Installation](Docs/01_Installation.md) | Requirements, installing the plugin, first launch |
| [02 — Importing](Docs/02_Importing.md) | Supported file formats and import workflow |
| [03 — Scene Setup](Docs/03_SceneSetup.md) | Placing splats in a level using `AHarmonyActor` |
| [04 — Settings](Docs/04_Settings.md) | Project-wide quality and rendering settings |
| [05 — Compression](Docs/05_Compression.md) | Expanded vs Compressed assets, converting between them |
| [06 — Asset Reference](Docs/06_AssetReference.md) | `UHarmonyDataAsset` properties and debug stats |

---

## Supported Formats

| Extension | Format | Imports As |
|---|---|---|
| `.ply` | Gaussian Splat PLY | Expanded |
| `.splat` | Packed binary splat | Expanded |
| `.spz` | Niantic SPZ (gzip) | Expanded |
| `.npz` | NumPy archive | Compressed |

See [Importing](Docs/02_Importing.md) for full details on each format.
