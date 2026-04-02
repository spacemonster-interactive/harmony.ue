# Scene Setup

Once a splat file has been imported as a `UHarmonyDataAsset`, place it in a level using a `AHarmonyActor`.

---

## Adding a Harmony Actor

1. In the **Content Browser**, locate the `UHarmonyDataAsset` you imported
2. Either:
   - **Drag it directly into the viewport** — a `AHarmonyActor` is created and the asset assigned automatically, or
   - **Place an empty actor** via *Place Actors → search "Harmony"*, then assign the asset manually in the Details panel
3. The splat appears immediately in the viewport

---

## Assigning the Data Asset

In the `AHarmonyActor` Details panel, set the **Data Asset** property to the `UHarmonyDataAsset` you want to display. Each actor displays one asset.

---

## Adjusting the Transform

Gaussian Splat captures are rarely centred at the world origin. After placing the actor, use the standard Unreal transform tools (move, rotate, scale) in the viewport or Details panel to position the splat correctly in the scene.

Getting the transform right is usually the first thing to do after placing an actor.

---

## Current Limitations

The `AHarmonyActor` Details panel is minimal at this stage of development. The main controls are:

- **Data Asset** — which splat to display
- **Transform** — position, rotation, scale in the world

> **Note:** Additional per-actor settings are expected to be added as development progresses. Many rendering options that currently live in the plugin's Project Settings will be moved to the actor level over time, allowing per-instance control over quality, layering, and other properties. See [Settings](04_Settings.md).

---

## Previewing

The splat renders in:

- **Editor viewport** — visible while authoring
- **Play in Editor (PIE)** — full runtime rendering in the editor
- **Cooked / packaged builds** — works in shipped Windows and macOS builds
