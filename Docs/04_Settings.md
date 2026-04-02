# Settings

> **Pre-release notice:** The settings described here are among the most actively changing parts of Harmony. Options will be added, removed, renamed, and reorganised between versions. Treat this page as a snapshot of the current state, not a stable reference.

---

## Project Settings

Harmony's global settings are found in:

*Edit → Project Settings → Plugins → Harmony*

These settings apply across the entire project and affect all `AHarmonyActor` instances. They cover areas including:

- **Rendering quality** — splat resolution, alpha thresholds, kernel size
- **Render pipeline** — pass ordering, tonemapper integration, background/foreground compositing
- **Occlusion** — depth-based rejection of splats behind opaque geometry

---

## Expected Changes

The current settings are intentionally kept at the project level while the plugin is in early development. As the plugin matures, settings that make sense on a per-splat basis will move into the `AHarmonyActor` Details panel, giving per-instance control. Settings that are genuinely global will remain in Project Settings.

Because this reorganisation is still in progress, avoid building automated tooling or pipelines around specific setting names or locations.

---

## Console Variables

Many settings are also exposed as console variables (`r.Harmony.*`) for runtime experimentation. These are primarily intended for development and debugging and are not guaranteed to be stable.

---

## Next Steps

- [Scene Setup](03_SceneSetup.md) — placing and configuring actors
- [Asset Reference](06_AssetReference.md) — data asset properties
