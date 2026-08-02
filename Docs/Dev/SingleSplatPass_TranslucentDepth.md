# Single Splat Pass via Pre-Translucency Depth

Design notes and research findings from the 2026-07-24 investigation. No code from that session
survives — this document is the deliverable. Engine references are UE 5.8
(`/Users/Shared/Epic Games/UE_5.8`), an **installed (binary) build**.

---

## 1. The problem

Harmony currently renders splats in two stages around UE's translucency:

- **Early** — `FPostOpaqueRenderDelegate`, fires at `RendererModule.RenderPostOpaqueExtensions`,
  before UE's translucency. Handles pixels with no opaque coverage.
- **Late** — `FAfterPassCallbackDelegate` after the custom tonemap. Composites over opaque-covered
  pixels.

Both stages rasterize the full splat set. The goal is to rasterize **once**, which requires knowing
the **nearest translucent surface depth before UE's translucency pass runs**, with no manual tagging
and working for arbitrary visible translucent objects.

Measured cost of the late stage in the test scene: **~3.5–4.5 ms** (25.76 ms with
`r.Harmony.Debug.Splats.StageSelect 1` vs 29–30 ms with `0`).

---

## 2. How UE gathers and renders translucency

### Gathering

Everything routes through `EMeshPass` (`Renderer/Public/MeshPassProcessor.h:56-110`). Translucency
slots: `Distortion`, `TranslucencyStandard(+Modulate)`, `TranslucencyAfterDOF(+Modulate)`,
`TranslucencyAfterMotionBlur`, `TranslucencyHoldout`, `TranslucencyAll`,
`LumenTranslucencyRadianceCacheMark`, `FrontLayerTranslucencyGBuffer`.

- Static meshes: `SceneVisibility.cpp:~1880-1940`, `DrawCommandPacket.AddCommandsForMesh(...)`
- Dynamic meshes: `SceneVisibility.cpp:~2540-2595`, `PassMask.Set(EMeshPass::X)`
- Both land in `FViewInfo::ParallelMeshDrawCommandPasses[EMeshPass::X]`

**`EMeshPass::Type` is a fixed enum with `Num` and `NumBits = 6` static-asserted — a plugin cannot
add a mesh pass.** This is the single most important constraint on the whole problem.

### Rendering

`FDeferredShadingSceneRenderer::Render` (`DeferredShadingRenderer.cpp`, L1822–4404). Order:

| Line | Call |
|---|---|
| 3496 | `RenderFrontLayerTranslucency(...)` ← **produces nearest-translucent depth** |
| 3876 | `RenderTranslucency(... UnderWater ...)` |
| **3924** | **`RendererModule.RenderPostOpaqueExtensions(...)`** ← Harmony's early hook |
| 4011 | `RenderTranslucency(... AboveWater ...)` |
| 4025 | `RenderDistortion(...)` |

`FTranslucencyPassResources` (`Renderer/Internal/TranslucentPassResource.h:27`) has a `DepthTexture`,
but it is the *input* depth the separate-translucency pass renders against — **not** an output of
translucent surface depth. It does not help.

---

## 3. Key finding: `FrontLayerTranslucency` already does this

`Renderer/Private/FrontLayerTranslucency.cpp` is a near-exact match for the requirement.

- `FFrontLayerTranslucencyGBufferMeshProcessor` (L212) / `CreateFrontLayerTranslucencyGBufferPassProcessor`
  (L386) — registered as `EMeshPass::FrontLayerTranslucencyGBuffer`, depth state
  `TStaticDepthStencilState<true, CF_DepthNearOrEqual>` → depth write on, nearest wins.
- `RenderFrontLayerTranslucency` (L649) allocates `Out.SceneDepth` from
  `SceneTextures.Depth.Target->Desc` (L753), pre-fills it with **opaque scene depth** via
  `FFrontLayerTranslucencyClearGBufferPS` (`Shaders/Private/FrontLayerTranslucency.usf:240-255`,
  `OutDepth = SceneDepthTexture.Load(...)`), then draws translucent surfaces on top.
- Result stored per view: `View.FrontLayerTranslucency.SceneDepth` (assigned L828; member declared
  `SceneRendering.h:1637`).

### CRITICAL semantics

**The buffer is pre-filled with opaque scene depth, NOT cleared to the far plane** (unlike
CustomDepth). "Translucency present" therefore means **strictly nearer than the opaque reference**,
never merely "finite". Testing `isfinite()` marks the entire screen as covered.

Use `View.FrontLayerTranslucency.RelativeDepthThreshold` (from `r.FrontLayerTranslucency.DepthThreshold`,
default 1024 ULP) rather than inventing an epsilon.

### Coverage

`ShouldRenderInFrontLayerTranslucencyGBufferPass` (L80-86) requires
`FMaterial::IsTranslucencyWritingFrontLayerTransparency()` — `MaterialShared.cpp:2178-2185`:

```
IsTranslucentBlendMode(BlendMode)
  && (TranslucencyLightingMode == TLM_Surface || TLM_SurfacePerPixelLighting)
  && bAllowFrontLayerTranslucency        // defaults TRUE (Material.cpp:1194)
```

So **no opt-in checkbox** — but `TranslucencyLightingMode` defaults to
**`TLM_VolumetricNonDirectional`** (`Material.cpp:1156`). Therefore:

- Glass / water / surface-lit translucency: **included automatically**
- Particles / smoke / volumetric-lit: **excluded**
- After-Motion-Blur translucency: excluded (L85)
- Nanite translucency: unsupported (`// TODO: Implement Nanite support`, L525/L656)

Arguably correct — a single "nearest depth" is meaningless for a smoke card. **Verified empirically
in this project: all hero-vehicle glass and headlight lenses are marked; nothing else needed it.**

### Gating

`ShouldRenderFrontLayerTranslucency` (L104-111) requires ONE of:
1. `ShouldRenderLumenReflections(View) && Lumen::UseFrontLayerTranslucencyReflections(View)`
2. `RayTracedTranslucency::IsEnabled(View)`
3. `MegaLights::UseFrontLayerTranslucencyDirectLighting(View)` (`MegaLights.cpp:610`) — **does not
   require Lumen GI**; a lighter gate worth trying
4. `IsVSMTranslucentHighQualityEnabled()` — unreliable, only fires when history is invalid

Shaders are gated on `DoesPlatformSupportLumenGI` (L128/164/184). **This is satisfied on all three
targets** — `Config/Mac/DataDrivenPlatformInfo.ini:36,77` and `Config/IOS/DataDrivenPlatformInfo.ini:89`
(`METAL_SM6_IOS`) all set `bSupportsLumenGI=true`.

### Existence is contingent

`IsLumenFrontLayerTranslucencyEnabled` (L627) requires `View.bTranslucentSurfaceLighting`, which
relevance only sets if a visible primitive uses translucent *surface* lighting. **With no qualifying
translucency in view, the pass is skipped entirely and `SceneDepth` is null — not empty.** Verified:
an empty map produced no buffer; loading the vehicle produced one.

Any production consumer needs an explicit "no buffer" state. It means "no translucency", so all
splats belong on the early side — but it must not be a silent black-dummy read.

---

## 4. Config required

The project was on raw engine CVar defaults — `r.DynamicGlobalIlluminationMethod = 0 (None)`,
`r.ReflectionMethod = 2 (SSR)` (`IndirectLightRendering.cpp:93,104`), with no
`[/Script/Engine.RendererSettings]` section at all.

```ini
[/Script/Engine.RendererSettings]
r.DynamicGlobalIlluminationMethod=1
r.ReflectionMethod=1
r.GenerateMeshDistanceFields=True

[SystemSettings]
r.Lumen.Reflections.Allow=1
r.Lumen.TranslucencyReflections.FrontLayer.Allow=1
r.Lumen.TranslucencyReflections.FrontLayer.Enable=1
```

Notes:
- All three RendererSettings are needed: `ShouldRenderLumenReflections` also requires
  `ShouldRenderLumenDiffuseGI`, and with no HWRT on Metal `IsLumenFeatureAllowedForView` falls
  through to `IsSoftwareRayTracingSupported()` = `DoesProjectSupportDistanceFields()`
  (`Lumen/Lumen.cpp:245-248`). Distance fields require an editor restart + DDC rebuild.
- `[SystemSettings]` is used because these are `ECVF_Scalability`. `BaseScalability.ini` sets
  `r.Lumen.Reflections.Allow=0` at ReflectionQuality **0 and 1**, and
  `FrontLayer.Allow=0` at quality **2**. SystemSettingsIni priority (0x05000000) outranks
  Scalability (0x02000000), verified live via the CVar HISTORY dump.
- **Ship caveat**: forcing `Allow=1` overrides a scalability decision. Correct for a spike, wrong
  for shipping — especially iOS. Set project scalability defaults or gate per-platform instead.

---

## 5. Measurements (test scene, Mac M4, 1019x580 → 2038x1159 TSR)

| Scope | ms |
|---|---|
| `FrontLayerTranslucency` (total) | **0.0821** |
| ├ `ClearTranslucencyGBuffer` | 0.0479 |
| ├ `TranslucencyGBuffer` (actual mesh draws) | **0.0096** |
| └ `TileClassificationMark` | 0.0241 |
| `LumenScreenProbeGather` | 1.0202 |
| `LumenSceneLighting` | 0.3188 |
| `LumenFrontLayerTranslucencyReflections` | 0.1795 |
| **Lumen gate total** | **~1.6** |
| `CustomDepth` (current split's depth source) | **5.6900** |
| `Translucency(BeforeDistortion Parallel)` | 0.1709 |
| Late splat stage | ~3.5–4.5 |

**The buffer is essentially free (0.08 ms). The Lumen gate is ~1.6 ms, not the 4–6 ms first
estimated.** Note `TranslucencyGBuffer` at 0.0096 ms is 18× cheaper than UE's own translucency draw
— consistent with a small qualifying set.

**Open question worth chasing: `CustomDepth` costs 5.69 ms.** If `TranslucentCustomDepthSplit` is
its only consumer, replacing the depth source lets it be switched off — a bigger win than the
single-pass work itself. Not verified what else in the project uses CustomDepth.

---

## 6. Options considered

| Option | Verdict |
|---|---|
| **A. Ride `FrontLayerTranslucency`** | Cheapest. ~0.08 ms, no new shaders, no engine change. Costs a Lumen dependency. **Chosen for the prototype.** |
| **B. Clone the pass into the plugin** | ~3–6 weeks. Cannot add an `EMeshPass`, so no relevance integration, no cached draw commands, no GPU-Scene culling — re-gather every frame. Also needs new material shader types (`TDepthOnlyVS`/`FDepthOnlyPS` refuse translucent materials unless `bIsTranslucencyWritingCustomDepth`, `DepthRendering.h:89-92,129-132`), invalidating all material DDC. **Not recommended.** |
| **C. Hijack CustomDepth** | Already what ships. `IsTranslucencyWritingCustomDepth` is a compile-time material property, so it cannot be forced at runtime — the tagging is unavoidable. |
| **D. Custom Render Pass / SceneCapture** | Full extra scene render, still needs ShowOnly tagging. Worse than status quo. |
| **E. Engine fork** | Add `EMeshPass::HarmonyTranslucentDepth` (room exists: ~43 of 64) and copy `FrontLayerTranslucency.cpp` with the material filter relaxed and the Lumen gate removed. Cleanest result, all platforms, no Lumen dependency. Requires moving off the installed build. **Recommended if the prototype proves out and the Lumen dependency is unwanted.** |

---

## 7. The single-pass design

One rasterization, MRT with two colour targets, per-fragment routing.

`Zo` = opaque scene depth, `Zt` = nearest translucent depth (valid only where `Zt < Zo`),
`Zs` = splat fragment depth.

| Pixel state | Fragment | Target |
|---|---|---|
| no opaque, no translucent | any | **Early** |
| translucent only | behind (`Zs > Zt`) | **Early** |
| translucent only | in front (`Zs < Zt`) | **Late** |
| opaque only | in front (`Zs < Zo`) | **Late** |
| opaque only | behind (`Zs > Zo`) | culled |
| translucent + opaque | `Zt < Zs < Zo` | **Early** |
| translucent + opaque | `Zs < Zt` | **Late** |
| translucent + opaque | `Zs > Zo` | culled |

- **Early** target composites into scene colour at post-opaque → UE's translucency blends over it.
- **Late** target composites after UE's translucency → covers translucent surfaces.

### Implementation gotchas discovered the hard way

1. **Do not `discard` for routing.** Both targets are written in one draw. A fragment routed to one
   must write **zero** to the other; zero under premultiplied `over` is a no-op. A discard drops the
   fragment from *both* targets.

2. **Opaque depth must come from a snapshot taken at post-opaque**, not live SceneDepth. By the time
   the late target composites, `Harmony.WriteBackgroundSceneDepthDeferred` has stamped splat depth
   into SceneDepth, so live depth is not a valid opaque reference.

3. **The opaque snapshot must NOT be conditional on the front-layer buffer existing.** Coupling them
   meant a scene with no qualifying translucency lost its opaque reference too, `bHasOpaque` went
   false everywhere, nothing was culled, and splats painted over all solid geometry.

4. **Use the OUTPUT (scene colour) viewport for the raster**, not `BackgroundDrawTargetViewport` —
   that one is built from the *proxy* texture with its own capped raster rect
   (`HarmonyViewExtension.cpp:~4976`), a different coordinate space. Using it misaligns everything.

5. **Do not fork the colour pipeline.** Inverse tonemap depends on a long chain
   (`DirectDrawApplyPixelColorPipeline` → `DirectDrawApplyInverseTonemapPipeline` →
   `DirectDrawInverseFilmicCurve`, exposure, local exposure, saturation). A dedicated single-pass
   shader should `#include` `s0.usf` and call `DirectDrawApplyPixelColorPipeline` /
   `DirectDrawCanPrecomputeColorPipeline` exactly as `TrianglePS` does. Forking it produced a
   blown-out image immediately.

6. **Compose passes must use `AddHarmonyDrawScreenPass`**, which pairs `FHarmonyScreenPassVS`
   (TEXCOORD0 = `float4 UVAndScreenPos`). `FPixelShaderUtils::AddFullscreenPass` uses a different VS
   whose TEXCOORD0 is `float2` — Metal rejects the PSO and
   `MetalPipeline.cpp:320` asserts fatally. With that VS, `UVAndScreenPos.xy` is *already* the input
   viewport UV; do not remap it manually.

7. The single-pass PS can reuse `FTriangleVS` and `FTrianglePassParams` — a UE pass binds one
   parameter struct, so a separate struct would force duplicating the splat quad expansion.

### Existing code the design builds on

Harmony already ships `r.Harmony.Feature.Splats.TranslucentCustomDepthSplit`, which performs this
split using CustomDepth:

- Depth read is a **single choke point**: `GetParticipatingTranslucentCustomDepth()` in `s0.usf`.
  All consumers go through it, so swapping the depth source is a one-function change.
- Split mode is applied in `TrianglePS`; **mode 1 (early discard) is currently dead code** — only
  `bProxyHybridLateTranslucentCustomDepthSplitActive` → **mode 2** on the proxy-hybrid late stage is
  live. Testing the split requires `StageSelect 0`.
- Shader params: `TranslucentCustomDepthSplitMode` / `StencilValue` / `StencilMask` / `Bias`.
- A debug view already exists: `ComposeDebugViewMode == 5u`, "Closest translucent depth". Note it
  assumes far-plane-clear semantics and must be adapted for the front-layer buffer.
- **Important**: this split routes fragments between two passes; it does **not** reduce
  rasterization. Both stages still rasterize the full splat set.

---

## 8. Status when the session was reverted

Everything below was built and compiling, then reverted. Nothing survives.

| Piece | State |
|---|---|
| Buffer visualization (`Harmony Front Layer Translucent Depth`, 3 modes) | Worked. Confirmed coverage on vehicle glass. |
| `r.Harmony.Feature.Splats.TranslucentDepthSource` (0=CustomDepth, 1=FrontLayer) | Built. A/B looked identical — but possibly a false positive if nothing in the scene was CustomDepth-tagged. **Unverified.** |
| Single-pass mode + `s2.usf` | Built and ran; misaligned/blown out. Bugs 2–5 above were identified and fixed but **the fixed version was never visually confirmed**. |

### If picking this up again

1. Restore the config block (§4) and confirm `FrontLayerTranslucencyGBuffer` appears in `stat GPU`
   with non-zero drawcalls (`ProfileGPU`, not `stat GPU`, shows draw counts).
2. Re-verify coverage across the full content set — other camera angles, configurator states,
   tail lights, interior glass, any VFX.
3. Check whether anything besides the translucent split consumes CustomDepth. If not, §5's 5.69 ms
   is the real prize.
4. Build single-pass with gotchas 1–7 already applied.
5. A `.profViz` decoder was written for this investigation; the format is documented in
   `Engine/Source/Developer/ProfileVisualizer/Private/VisualizerEvents.cpp`
   (`Save/LoadVisualizerEventRecursively`) — little-endian, no header, one recursive root event:
   `double Start, double Duration, double DurationMs, int32 Category, FString EventName,
   bool IsSelected (32-bit), uint32 NumChildren, children...`
