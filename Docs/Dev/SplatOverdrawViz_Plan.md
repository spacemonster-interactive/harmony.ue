# Splat Overdraw Visualization — Plan & Handoff

> Living doc. The model doing the work updates the **Status** and checks off steps as it goes,
> so any model can pick up mid-stream. Keep edits to Status/checkboxes; don't rewrite the plan
> without noting why.

## Status
- **Current step:** DONE.
- **Last updated by:** Codex (post-Step 4 heatmap endpoint padding)
- **Notes:** Step 1 shader work is still pending human shader compile verification
  (`recompileshaders changed` or restart editor). Step 4 tuned the heatmap for the first attached
  screenshot where the old linear `Count / 32` mapping saturated most splats white. The view now uses
  fixed log normalization against `HeatmapMax`, so heavily overdrawn regions retain more color
  separation. A follow-up tuning pass after the second screenshot lowered the default max to 512 and
  made the red-to-white tip slightly more eager, so the hottest pockets can reach white again without
  returning to the all-white view. A later ramp-tuning pass after the third screenshot changed the
  color order to deep blue -> blue -> cyan -> green -> yellow -> red -> magenta -> white, with
  non-uniform low-end stops so very low counts stay blue longer and a magenta hot band before white.
  A final endpoint-padding pass after the fourth screenshot stretched the deep-blue/blue low end and
  started the magenta-to-white tip earlier, so thin distant splats should read bluer and the hottest
  regions should retain more white.
  Zero-off-cost gates were rechecked: callback registration, counter RT allocation/draw, and heatmap
  pass are all behind `IsVisualizingSplatOverdraw`. Balance verified after endpoint padding:
  `s0.usf` braces 0 / parens -3 (pre-existing baseline), `HarmonyViewExtension.cpp` 0/0,
  plan doc 0/0.
  What Step 1 added:
  - `FTrianglePS::FWriteOverdraw` permutation (`WRITE_OVERDRAW` define) in `HarmonyShaders.h`,
    restricted in `ShouldCompilePermutation` to compile *only* standalone (all other flags off) → +1
    permutation total.
  - `TrianglePS` in `s0.usf`: under `#if WRITE_OVERDRAW`, after all discards + `Alpha *= StageCoverageWeight`,
    it emits `float4(1,0,0,0)` and returns (skips color pipeline/debug). One unit per surviving fragment.
  - `HarmonyOverdrawHeatmap(float Count, float MaxCount)` helper in `s0.usf` (deep-blue-to-white ramp;
    Count 0 → black), ready for the Step 3 viz pass.
  What Step 2 added:
  - `HarmonyDebug::SplatOverdrawUserSceneTextureName = "HarmonySplatOverdraw"`,
    `GSplatOverdrawVisualizationMode`, and `IsVisualizingSplatOverdraw`.
  - `r.Harmony.Debug.SplatOverdraw.HeatmapMax` CVar (default 512 after post-Step 4 tuning).
  - Per-view render-thread state for the RDG overdraw counter texture/view rect/view key.
  - A post-opaque, early-stage-only accumulation pass that is created only when the splat-overdraw
    buffer visualization is active. It allocates/clears an R32F counter RT, reuses
    `InitializeDirectPassParams`, uses the standalone `FWriteOverdraw` permutation with stage gates
    off, keeps per-splat opaque depth reject governed by `r.Harmony.Feature.Occlusion.DepthReject`,
    and additively blends red (`CW_RED, BO_Add, BF_One, BF_One`) into the counter.
  What Step 3 added:
  - `FHarmonyVisualizeSplatOverdrawPS` / `VisualizeSplatOverdrawPS`, which maps the counter's red
    channel through `HarmonyOverdrawHeatmap(Count, r.Harmony.Debug.SplatOverdraw.HeatmapMax)`.
  - `RenderSplatOverdrawVisualizationPass_RenderThread`, registered at the Tonemap callback only
    when `IsVisualizingSplatOverdraw` is true. It samples the Step 2 counter for the matching view
    or outputs black if no valid counter exists.
  - Generated post-process material + config registration for `"HarmonySplatOverdraw"` at
    `/Harmony/Generated/M_HarmonySplatOverdrawBufferVis`, labelled "Harmony Splat Overdraw".
  What Step 4 added:
  - Changed `HarmonyOverdrawHeatmap` from linear `Count / HeatmapMax` normalization to fixed
    `log2(Count + 1) / log2(HeatmapMax + 1)` normalization.
  - Raised the default `r.Harmony.Debug.SplatOverdraw.HeatmapMax` from 32 to 1024 and expanded the
    CVar help text. Later tuning passes lowered the default to 512, made the white tip more visible,
    and changed the ramp to deep blue -> blue -> cyan -> green -> yellow -> red -> magenta -> white.
    The latest endpoint-padding pass keeps low counts blue longer and gives the hottest white tip more
    range. Lower the CVar to reveal low overdraw and bring white in sooner; raise it to separate
    extreme hotspots.
  - Rechecked off-mode gates and noted that TSR behavior is the intended render-res counter sampled at
    output-res behavior; no new pass runs unless the buffer visualization is selected.

---

## Feature

Add a Harmony buffer-visualization mode — **"Harmony Splat Overdraw"** — that shows a **heatmap of
per-pixel splat fragment count** (how many splat fragments blended into each pixel), analogous to
Unreal's standard shader-complexity/overdraw view but for Gaussian splats. Splat overdraw (lots of
overlapping low-alpha fragments) is a primary GPU cost, so this heatmap directly guides the
optimization work.

## Hard constraints (do not violate)
1. **Zero performance cost when the viz is not selected.** All new work (counter RT, accumulation
   draw, heatmap pass) must be gated behind "is this viz active", exactly like the existing
   scene-coverage-mask viz. The normal render path must be untouched.
2. **Don't complicate the core composite.** This is a self-contained, bolt-on debug path. Do NOT
   modify the two-stage early/late composite. One new shader permutation of the splat draw is the
   only allowed intrusion into shared code.
3. **Code-changes-only workflow.** The human builds/tests via Xcode; the model does not build. Make
   edits anchored on exact content; verify brace/paren balance after shader edits.

## Locked design decisions (agreed before planning)
- **Count metric:** post-discard fragments — i.e. fragments that survive the same discards as the
  real draw (min-alpha, depth reject, stage gate) and would actually blend. This reflects real cost.
- **Accumulation:** a single dedicated overdraw draw of **all splats once** (not stage-accurate).
  Stage-accurate accumulation (counting in both early and late passes at their real resolutions) is
  explicitly **deferred** — a single representative pass is enough for a heatmap.
- **Blend:** additive (`CW_RED, BO_Add, BF_One, BF_One`) into a single-channel float counter RT;
  the overdraw PS emits `1.0` per surviving fragment.
- **Resolution:** render resolution for the counter RT (accumulation runs pre-TSR where the splats
  are prepared). The heatmap display samples it at output res — fine for a debug view.
- **Heatmap normalization:** fixed max via a CVar (predictable), not auto-normalize. Step 4 switched
  display normalization to a fixed log curve so extreme scenes do not immediately saturate to white.
- **Trigger:** selecting the buffer-vis mode is the trigger (like the coverage-mask viz). No feature
  CVar needed to activate; the only CVar is the heatmap max.
- **Maintainability mandate (important — avoids a forked path):** zero-off-cost is achieved via a
  *gated pass*, NOT a duplicated code path. The overdraw draw MUST be a **permutation of the existing
  `TrianglePS`** (`FWriteOverdraw`) so all VS/discard/cull logic is shared (compiler-enforced, no
  divergence), and the C++ accumulation pass MUST **reuse the existing splat param-setup**
  (`InitializeDirectPassParams`) rather than hand-copying the draw binding. Only three things may
  differ from the real draw: the PS output line (shader), the additive blend state, and the counter
  RT. Do NOT fork `TrianglePS` into a separate shader, and do NOT duplicate the param binding.
  (Rejected alternative: an extra MRT on the real draw — shares more code but adds a bound target/clear
  to the hot path even when off, violating zero-off-cost.)
- **Heatmap ramp (default, tunable in `HarmonyOverdrawHeatmap`):** cool→hot ramp with a magenta/white
  tip — deep blue → blue → cyan → green → yellow → red → magenta → white, log-normalized by
  `HeatmapMax`; **count 0 → black** (so splat-free pixels read as background and hotspots pop).
  Background is pure black (not a dimmed scene) for a clean debug read. Both the ramp stops and
  black-vs-dimmed are trivial to change later.

## Mechanism (how it fits together)
1. When the "Harmony Splat Overdraw" buffer-vis mode is active (`IsVisualizingSplatOverdraw`), a
   **gated accumulation pass** at the post-opaque hook draws all prepared splats with a
   `WRITE_OVERDRAW` permutation of the splat PS — same VS + same discard logic, but additive `1.0`
   into an R32F/R16F **counter RT** (render res). Uses the already-prepared splat buffers
   (`PreprocessedSplats`, sorted indices, `DrawIndexedArgs`) from `EnsureDirectDrawPreparedForView`.
2. A **visualization SVE pass** (`RenderSplatOverdrawVisualizationPass_RenderThread`) — registered
   in `SubscribeToPostProcessingPass` only when the mode is active — reads the counter RT, maps
   count → heatmap color (fixed max from CVar), and outputs it.
3. A **generated buffer-vis material + registration** (mirror `HarmonySceneDepth`) puts
   "Harmony Splat Overdraw" in the Buffer Visualization menu.

## Reference: existing symbols to mirror (already in the codebase)
- Debug name: `HarmonyDebug::SceneCoverageMaskUserSceneTextureName = "HarmonySceneCoverageMask"`
  (`Source/Harmony/Public/HarmonyDebug.h`).
- Viz-active check: `IsVisualizingSceneCoverageMask(const FSceneViewFamily&)`
  (`HarmonyViewExtension.cpp` ~L1219) + `GSceneCoverageMaskVisualizationMode` (~L1197).
- Viz pass: `RenderSceneCoverageMaskVisualizationPass_RenderThread` (~L2469) and its registration in
  `SubscribeToPostProcessingPass` (~L2371–2375).
- Buffer-vis material: `FHarmonyBufferVisualization::EnsureSceneDepthVisualizationMaterial()` /
  `RegisterSceneDepthBufferVisualizationMaterial()`, key `SceneDepthVisualizationKey = "HarmonySceneDepth"`,
  package `/Harmony/Generated/M_HarmonySceneDepthBufferVis` (`HarmonyBufferVisualization.cpp` L32, L230, L364).
- Splat draw to reuse: `TriangleVS`/`TrianglePS` in `Shaders/Private/s0.usf`; the prepared draw
  buffers + `EnsureDirectDrawPreparedForView_RenderThread` in `HarmonyViewExtension.cpp`.
- Proposed new names (keep parallel): `HarmonySplatOverdraw` (debug name + viz key +
  `/Harmony/Generated/M_HarmonySplatOverdrawBufferVis`), `IsVisualizingSplatOverdraw`,
  `RenderSplatOverdrawVisualizationPass_RenderThread`, CVar `r.Harmony.Debug.SplatOverdraw.HeatmapMax`.

---

## Steps

Dependencies are linear: 1 → 2 → 3 → 4. Each step should leave the tree compiling. The feature is
end-to-end visible only after Step 3.

### Step 1 — Shader: overdraw permutation + heatmap helper
**Files:** `Shaders/Private/s0.usf`, `Source/Harmony/Public/HarmonyShaders.h`
- Add a `WRITE_OVERDRAW` permutation (or a dedicated overdraw PS reusing `TriangleVS`) that runs the
  **existing** discard/gate logic in `TrianglePS` (min-alpha, depth reject, `NO_OPAQUE_ONLY`/
  `HYBRID_OPAQUE_ONLY` — for the single-pass overdraw draw, draw with the gate OFF so it counts all
  splats once) and outputs `float4(1,0,0,0)` (additive count) instead of premultiplied color.
- Add a heatmap mapping function `float3 HarmonyOverdrawHeatmap(float count, float maxCount)` (blue→
  green→yellow→red ramp) for use by the viz pass in Step 3.
- Register the permutation/PS in `HarmonyShaders.h` (mirror an existing `FTrianglePS` permutation).
**Test:** compiles; `recompileshaders changed` succeeds; no behavior change (permutation unused yet).
**Done when:** [x] DONE — `FWriteOverdraw` permutation + `HarmonyOverdrawHeatmap` helper + `#if WRITE_OVERDRAW`
output branch in `TrianglePS`. Standalone-only via `ShouldCompilePermutation`. No behavior change until Step 2.

### Step 2 — C++: counter RT + gated accumulation pass
**Files:** `Source/Harmony/Private/HarmonyViewExtension.cpp`, `Source/Harmony/Public/HarmonyViewExtension.h`,
`Source/Harmony/Public/HarmonyDebug.h`
- Add `HarmonyDebug::SplatOverdrawUserSceneTextureName = "HarmonySplatOverdraw"`.
- Add `IsVisualizingSplatOverdraw(const FSceneViewFamily&)` + `GSplatOverdrawVisualizationMode`
  (mirror `IsVisualizingSceneCoverageMask`).
- Add CVar `r.Harmony.Debug.SplatOverdraw.HeatmapMax` (float, default e.g. 32; post-Step 4 tuning set this to 512).
- Allocate an R32F/R16F **counter RT** (render res), clear to 0, and draw all prepared splats with
  the `WRITE_OVERDRAW` permutation + additive blend (`CW_RED, BO_Add, BF_One, BF_One`) into it —
  **only when `IsVisualizingSplatOverdraw`**. Hook at the post-opaque path
  (`RenderEarlySplatsPass_RenderThread`) where splats are already prepared. Stash the counter RT
  (RenderThreadState field, e.g. `SplatOverdrawCounterTexture` + ViewRect/ViewKey, mirroring
  `OpaqueSceneDepthTexture`) for Step 3.
- **Draw config (per locked decisions):** `FWriteOverdraw=true`; stage gates OFF
  (`FNoOpaqueOnly=false`, `FHybridOpaqueOnly=false`) so all splats count once; keep depth reject ON
  (`EnableOpaqueDepthReject`) and min-alpha ON so only fragments that really blend are counted;
  coverage/avg-depth permutations OFF.
- **Maintainability mandate:** build the pass's `FTrianglePassParams` by **reusing the existing
  `InitializeDirectPassParams` helper**, then only override the PS permutation + blend + render target.
  Do not hand-copy the buffer/param binding — that's the alignment burden we're avoiding.
**Test:** with the viz mode active (after Step 3) or a temporary debug dump; no cost when inactive
(the whole block is behind the gate). Verify balance.
**Done when:** [x] DONE — gated counter RT + accumulation draw added in `RenderSplatsCommon_RenderThread`,
state stashed for Step 3, all work behind `IsVisualizingSplatOverdraw`, and balance checks verified.

### Step 3 — C++: visualization pass + buffer-vis registration (end-to-end visible)
**Files:** `Source/Harmony/Private/HarmonyViewExtension.cpp/.h`,
`Source/Harmony/Private/HarmonyBufferVisualization.cpp/.h`
- Add `RenderSplatOverdrawVisualizationPass_RenderThread` (mirror
  `RenderSceneCoverageMaskVisualizationPass_RenderThread`): when the mode is active, sample the
  counter RT, apply `HarmonyOverdrawHeatmap` with the CVar max, output the heatmap.
- Register it in `SubscribeToPostProcessingPass` (mirror the coverage-mask registration ~L2371).
- Add `EnsureSplatOverdrawVisualizationMaterial()` + `RegisterSplatOverdrawBufferVisualizationMaterial()`
  (mirror the SceneDepth pair), key `"HarmonySplatOverdraw"`, package
  `/Harmony/Generated/M_HarmonySplatOverdrawBufferVis`; call them from Startup/Initialize.
**Test:** Buffer Visualization menu shows "Harmony Splat Overdraw"; selecting it shows a heatmap over
splats (hot where splats overlap heavily). Toggle off → normal scene, no cost.
**Done when:** [x] DONE — heatmap shader/pass added, Tonemap callback registered behind
`IsVisualizingSplatOverdraw`, generated buffer-vis material/config registered, and balance checks verified.

### Step 4 — Polish & verify
**Files:** as needed.
- Verify **zero cost when off** (accumulation/counter/heatmap all skipped unless mode active).
- Tune `HeatmapMax` default; add tooltips/legend note; make sure it behaves under TSR upscaling
  (counter is render res, sampled at output res — confirm no obvious scaling artifacts; a heatmap
  tolerates soft upscaling).
- Update this doc's Status to DONE and note any deferred items (stage-accurate accumulation).
**Done when:** [x] DONE — heatmap uses fixed log normalization with default `HeatmapMax=512`,
CVar help text explains the range tradeoff and color order, zero-off-cost gates were rechecked, and
TSR behavior remains the planned render-res counter sampled at output res. Deferred: stage-accurate
accumulation and runtime overdraw-driven culling experiments.

## Deferred / out of scope
- Stage-accurate overdraw (accumulate separately in early + late at their real resolutions).
- Auto-normalized heatmap range.
- Counting VS/rasterization cost of discarded fragments (only post-discard fragments counted).
