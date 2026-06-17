# Settings

> **Pre-release notice:** The settings described here are among the most actively changing parts of the plugin. Options will be added, removed, renamed, and reorganised between versions. Treat this page as a snapshot of the current state, not a stable reference.

---

## Project Settings

There are currently two project-level settings sections under **Edit -> Project Settings -> Plugins**:

- **Harmony**: runtime and rendering behavior
- **Harmony Import**: import-time DataAsset behavior

### `Plugins -> Harmony`

These settings apply across the entire project and affect all `AHarmonyActor` instances. They cover areas including:

- **Rendering quality** — splat resolution, alpha thresholds, kernel size
- **Render pipeline** — pass ordering, tonemapper integration, background/foreground compositing
- **Occlusion** — depth-based rejection of splats behind opaque geometry

### `Plugins -> Harmony Import`

This section controls how supported source files are turned into `UHarmonyDataAsset`s at import time.

Current option:

- **Import Compressed**: when enabled, formats that first import as Expanded data (`.ply`, `.splat`, `.spz`, `.ksplat`, `.sog`) are automatically compressed before the asset is saved

Notes:

- This setting defaults to `true`
- It affects future imports only; it does not rewrite existing assets
- `.npz` imports are already compressed at source and are not affected by this setting

---

## Expected Changes

The current settings are intentionally kept at the project level while the plugin is in early development. As the plugin matures, settings that make sense on a per-splat basis may move into the `AHarmonyActor` Details panel, giving per-instance control. Settings that are genuinely global will remain in Project Settings.

Because this reorganisation is still in progress, avoid building automated tooling or pipelines around specific setting names or locations.

---

## Console Variables

Many runtime settings are also exposed as console variables (`r.Harmony.*`) for experimentation. These are primarily intended for development and debugging and are not guaranteed to be stable.

Import settings are editor settings, not runtime console variables.

---

## Next Steps

- [Importing Splats](02_Importing.md) — supported formats and import workflow
- [Scene Setup](03_SceneSetup.md) — placing and configuring actors
- [Asset Reference](06_AssetReference.md) — data asset properties
