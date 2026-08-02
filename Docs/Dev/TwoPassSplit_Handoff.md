# Harmony Splat Rendering — Two-Pass Split: Implementation Handoff

> Self-contained brief for an agent picking up cold (no prior chat context).
> Supersedes `R1_ProxyHybrid_Design.md` as the active plan. Read this fully before editing.

---

## 0. TL;DR

We need to render Gaussian splats so they (a) **blend softly over opaque UE geometry** (no hard edge) and (b) **don't break transparent UE geometry** (glass). The fix is to split splat compositing into **two passes keyed on "is there opaque geometry in front of this pixel?":**

- **EARLY pass** — before translucency, pre-tonemap, **masked**: draw splats only where there is **no opaque** in front. Transparency then works because UE translucency draws over these splats normally.
- **LATE pass** — after the custom tonemap (deferred / display space): draw splats only where there **is opaque**, using the per-splat feathered depth reject. This is the soft-edge fix, confined to the region that needed it.

Most of the machinery for both passes already exists in changelist **449**; this is largely a **re-routing + restoring the mask**, not a rewrite.

---

## 1. What Harmony is

- UE5 plugin (`Plugins/Harmony`) that renders 3D Gaussian splats as a "fixed post-opaque background plate."
- Implemented as an `FSceneViewExtension` (`FHarmonyViewExtension`). It preprocesses/sorts splats (compute), then rasterizes them as billboards and **composites them into the scene via post-process hooks**.
- Target engine: UE **5.8** (also 5.7). Use `#if __has_include(...)` guards for version-divergent headers (e.g. `SceneViewState.h`).
- **You cannot self-verify rendering.** Builds compile, but correctness needs a human looking at the editor. There are buffer-visualization views (see §7) to make internal state visible.

### Pipeline stages that matter
```
1. Depth prepass + base pass (opaque)            -> scene color + opaque SceneDepth
2. Lighting
3. ── PostOpaque delegate ─────────────────────  (RegisterPostOpaqueRenderDelegate)  [SceneDepth WRITABLE]
4. TRANSLUCENCY passes (glass)  -> blended into scene color
5. Post-processing:
     BeforeDOF  (AfterPass callbacks)            [SceneDepth READ-ONLY]
        - RenderCustomTonemapPass  (the early custom tonemap, "EarlyMasked")
        - RenderDeferredBackgroundSplatsPass
     DOF
     ReplacingTonemapper (UE tonemapper suppressed by Harmony)
     Tonemap (AfterPass) -> RenderForegroundSplatsPass (extended tonemapper)
```
Key facts:
- **Stage 3 (post-opaque) is BEFORE translucency and SceneDepth is writable.**
- **Stage 5 (post-process) is AFTER translucency and SceneDepth is read-only.**
- `bApplyInverseTonemap` (CVar `r.Harmony.Feature.Background.InverseTonemap`) defaults **OFF** — the inverse-tonemap code paths are effectively dead; do not build on them.

---

## 2. The problem and why we're here

**Original problem:** splats over opaque geometry showed a **hard occlusion edge** instead of a soft Gaussian falloff. Root cause: the proxy path occludes via a single **averaged-depth-per-pixel** reject (hard); only a **per-splat** reject feathers correctly.

**What we did (changelist 449):**
1. Fixed the hard edge with a **per-splat feathered depth reject** (the real win — keep this).
2. Built **R1 "proxy hybrid"**: in proxy mode, compose the cached proxy where there's no opaque, and per-splat direct-draw where there is opaque.
3. Moved **all** splat compositing to **after the custom tonemap** (the "deferred composite", stage 5) to avoid the tonemap mangling splat colors.

**What that broke:** moving everything to stage 5 means splats composite **after translucency** → splats paint over glass, and you can't put splats *behind* glass. Also: stage 5 can't write SceneDepth, so splat depth vanished from DOF/fog in EarlyMasked mode.

**The realization (the new plan):** only the **opaque-overlap** region ever needed stage 5. The **no-opaque** region (the bulk of the environment) was fine at stage 3 and should go back there, where translucency composites over it correctly. Split by the opaque mask.

---

## 3. Target architecture — the two-pass split

| Pixel region | Stage | Tonemap | Mask test | Reject |
|---|---|---|---|---|
| **No opaque in front** (sky / far background) | **EARLY** (post-opaque, stage 3) | pre-tonemap, **masked-preserve** | draw where `sceneDepth >= opaqueDist` (no near opaque) | n/a (mask) |
| **Opaque in front** (car, cube, building) | **LATE** (deferred, stage 5) | post-tonemap, display | draw where `sceneDepth < opaqueDist` (opaque present) | **per-splat feather** vs SceneDepth |

### Direct mode (`bUseProxyRT=false`)
- **EARLY:** direct-draw splats into scene color, **masked to no-opaque** pixels. (Translucency then draws over → glass works.)
- **LATE:** direct-draw splats into the post-tonemap Output, **gated to opaque** pixels, with the per-splat feather. (Soft edge over the car.)

### Proxy mode (`bUseProxyRT=true`)
- **EARLY:** raster all splats into the proxy RT (as today), then **compose proxy → scene only where no-opaque**.
- **LATE:** **direct** draw (not from proxy) into post-tonemap Output, gated to opaque, per-splat feather.

### Why this fixes transparency
- No-opaque splats are in scene color **before** translucency → translucency blends over them → **you see splats through glass**.
- Opaque-region splats drawn late are foliage etc. that is genuinely **in front** of the car/glass → drawing over the glass is correct.
- Only wrong case: a splat sandwiched *between* a transparent surface and the opaque behind it (e.g. between windshield and dashboard). Doesn't happen with outdoor environment splats. Acceptable; a transparent-depth refinement could fix it later (see §8).

### The mask comes back
Because the EARLY splats are **pre-tonemap**, the custom tonemap must **preserve** them (don't tonemap splat-covered pixels) — the selective coverage mask. We removed/short-circuited that mask when we went "deferred-everything"; **restore it**, but it now only needs to preserve: (early no-opaque splat coverage) OR (transparent geometry). The mask is fundamentally a **scene-depth test** (`SceneLinearDepth < OpaqueDepthDistance` = opaque present) plus splat-coverage and separate-translucency-visibility terms.

---

## 4. Current code state (changelist 449, after reverting depth-only)

**449 is pending/unsubmitted.** The "depth-only / SceneDepth snapshot / `USE_OPAQUE_DEPTH_OVERRIDE`" experiment was just reverted via shelf — do **not** re-introduce it; the two-pass split makes it unnecessary (EARLY pass writes splat depth naturally at stage 3 while depth is writable, and there's no reject/depth-write conflict because EARLY writes only in no-opaque regions while the LATE reject reads the opaque region).

### KEEP (already in 449, reuse these)
- **Per-splat feathered reject** (s0.usf) — the hard-edge fix. Reject args in `InitializeDirectPassParams` (`HarmonyViewExtension.cpp`). `OpaqueDepthRejectFeather`/`...Bias`/`...Mode` CVars.
- **Deferred-after-tonemap path:** `RenderDeferredBackgroundSplatsPass_RenderThread` (registered right after `RenderCustomTonemapPass` at BeforeDOF). This is the **LATE pass** vehicle.
- **R1 hybrid opaque-only draw:** the `HYBRID_OPAQUE_ONLY` shader permutation on `FTrianglePS` + the `Harmony.ProxyHybridDirectBackground` draw block in `Common`. This **is** the late opaque-region draw.
- **Compose-skip:** `ComposeProxyHybridSkipOpaque` cbuffer flag + the `bProxyHybridSkip` logic in `ComposeAfterDOFPS` (s0.usf). This is the proxy "compose only where no-opaque." (For the two-pass model this skip logic moves to / is reused at the EARLY proxy compose.)
- **Buffer visualizations** (independent, useful for debugging this work): `Harmony Scene Coverage Mask`, `Harmony Separate Translucency`, `Harmony Scene Depth (opaque + splats)`. In `HarmonyBufferVisualization.cpp` + the viz passes in `HarmonyViewExtension.cpp`.
- **Defaults:** `r.Harmony.Feature.Compose.DeferredBackgroundComposite=1` and `r.Harmony.Feature.Compose.ProxyHybridDirect=1` (flipped to on in the CVar declarations).
- **Stride fix** (already SUBMITTED separately as change 444): `LayerPartition*` shaders use `SplatIndex * 5u`. Not in 449.

### CHANGE (the actual work)
- Currently 449 routes **everything** to the deferred stage (both direct and proxy composite at stage 5; the post-opaque pass early-returns when `IsDeferredBackgroundCompositeEnabled`). **Re-shape** so the **no-opaque region composites at stage 3 (post-opaque)** and only the **opaque region composites at stage 5 (deferred)**.
- **Restore the selective coverage/preserve mask** for the early (pre-tonemap) splats. Look at the pre-449 base (`p4 print <file>#<have-1>` or diff 449 vs base) for the original mask code that was removed/bypassed — specifically the `bApplyTonemapToWholeScene` change (we forced it true for deferred; it needs to NOT drop the transparent + early-splat preservation).

---

## 5. Files & key locations (paths under `Plugins/Harmony/`)

- `Shaders/Private/s0.usf` — all splat/compose/tonemap shaders.
  - `SampleSceneLinearDepthAtPixel` / `SampleSceneDeviceDepthAtPixel` (~L1362, L1460) — the reject's depth read.
  - `bSceneDepthCanOccludeBackground` / `OpaquePresenceDepthDistance` — the "opaque present" test (the mask core).
  - `ComposeAfterDOFPS` (~L2647) — proxy compose; has `bProxyHybridSkip`.
  - `ComputeSceneCoverageMaskSample` (~L2463) — builds the coverage mask (`GeoApplyMask`, splat-coverage, `NoTransparentMask` from separate translucency).
  - The extended-tonemapper main (~L3100–3322) — `lerp(PreservedColor, TonemappedColor, GeoApplyMask)`; foreground splat composite; `OutColor`.
  - Direct PS (`FTrianglePS` body, ~L1950–2200) — per-splat reject, `HYBRID_OPAQUE_ONLY` discard gate (~L1957), alpha/min-alpha, `OutColor = float4(Color,1)*Alpha`.
- `Source/Harmony/Private/HarmonyViewExtension.cpp` (~8000 lines):
  - `SubscribeToPostProcessingPass` (~L2350) — registers custom tonemap (BeforeDOF), deferred splats (BeforeDOF, after tonemap), foreground (Tonemap), tonemap-replacement (ReplacingTonemapper), and the viz passes.
  - `RenderCustomTonemapPass_RenderThread` — the early tonemap; builds `bApplyTonemapToWholeScene` (the mask-or-not decision, ~L2831).
  - `RenderDeferredBackgroundSplatsPass_RenderThread` (~L3321) — the LATE pass entry; calls `Common` with `bDeferredCompositeStage=true`.
  - `RenderBackgroundSplatsPostOpaque_RenderThread` (~L3935) — the EARLY/stage-3 entry; currently early-returns when deferred — **this is where the early no-opaque pass should run.**
  - `RenderBackgroundSplatsCommon_RenderThread` (~L4011) — the ~1500-line workhorse: prepare, proxy raster, color draws, depth-write chain, compose, hybrid draw, ambiguity. `InitializeDirectPassParams` lambda (~L4460) sets all direct-draw params incl. reject config.
  - Splat depth-write chain (~L4900 raster → ~L4950 normalize → ~L5020 `FHarmonyWriteBackgroundSceneDepthPS` stamps into SceneDepth via `FDepthStencilBinding`).
- `Source/Harmony/Public/HarmonyShaders.h` — `FTrianglePassParams`, `FTrianglePS` (permutations: `FWriteSplatAverageDepth`, `FWriteSplatCoverage`, `FHybridOpaqueOnly`), `FHarmonyComposeAfterDOFPassParams`, `FHarmonyComposeExtendedTonemapperPS`.
- `Source/Harmony/Public/HarmonyViewExtension.h` — `Common` signature, `FRenderThreadState`.
- `Config/DefaultEngine.ini` `[/Script/Harmony.HarmonySettings]` — `bUseProxyRT`, `TonemappingMode=EarlyMasked`, splat-tuning defaults.

---

## 6. Suggested implementation order (each step independently testable)

1. **Restore the mask.** Undo the `bApplyTonemapToWholeScene = true` shortcut so the custom tonemap again preserves splat-covered + transparent pixels via the coverage mask. *Test:* EarlyMasked still renders; transparent geometry no longer gets mangled. (This alone may restore glass for the no-opaque-through-glass case once step 2 lands.)
2. **Early no-opaque draw (direct mode).** In `RenderBackgroundSplatsPostOpaque_RenderThread`, instead of early-returning when deferred, run the splat draw **masked to no-opaque pixels** (draw where `sceneDepth >= opaqueDist`). Reuse the direct draw; add a "no-opaque only" gate (mirror of `HYBRID_OPAQUE_ONLY`, e.g. a permutation `NO_OPAQUE_ONLY`). *Test (Scene Coverage Mask + viewport):* background splats appear pre-tonemap and translucency composites over them.
3. **Late opaque draw (direct mode).** Keep the deferred pass, but gate its direct draw to **opaque-only** (this is already the `HYBRID_OPAQUE_ONLY` behavior — reuse it). Ensure no double-draw between early/late (they're complementary by the depth test). *Test:* soft feather over the car; no double-bright seam at the silhouette.
4. **Proxy mode.** Early: raster→proxy then compose proxy **only where no-opaque** (reuse `ComposeProxyHybridSkipOpaque` but inverted to "compose where no-opaque"). Late: direct opaque-only draw (reuse hybrid). *Test:* proxy matches direct quality; glass intact.
5. **Cleanup.** Remove now-dead "deferred-everything" routing, the averaged-depth/ambiguity machinery if unreachable (roadmap C5), retire the foreground/background split (C1, already approved). Rename `EarlyMasked` → `EarlyCustom` once the legacy mask path is gone (touches the enum + config strings).

---

## 7. Verification (no self-test — use these)

- **Test scene:** a car (Chaos vehicle) with **glass** material (`M_GLASS_*`), foliage/environment splats around and in front of it. Setting: `TonemappingMode=EarlyMasked`.
- **Buffer Visualization menu** (Lit viewport → Buffer Visualization):
  - **Harmony Scene Coverage Mask** — yellow = opaque (tonemapped), magenta = transparent. Shows the opaque/transparent split the mask sees.
  - **Harmony Separate Translucency** — confirms the glass buffer is present at compose time.
  - **Harmony Scene Depth (opaque + splats)** — shows what the reject reads; confirms whether splat depth reached SceneDepth.
- **Toggles for bisecting:** `r.Harmony.Feature.Compose.ProxyHybridDirect`, `r.Harmony.Feature.Compose.DeferredBackgroundComposite`, `r.Harmony.Feature.Occlusion.DepthReject`, `bUseProxyRT`, `TonemappingMode`. Remember **console overrides beat code defaults** until reset/restart (bit us repeatedly).
- After C++ changes: rebuild C++. After shader changes: `recompileshaders changed`. Some changes need an editor restart (buffer-viz material regen).

---

## 8. Gotchas / hard-won lessons (do not relearn these)

1. **Use shader PERMUTATIONS, not loose-global runtime flags, for anything that gates the depth read or discards fragments.** A loose-global texture param shared across many passes caused an unbound-binding regression (black screen) that took ~10 iterations to find. The permutation strips the param from passes that don't use it. `HYBRID_OPAQUE_ONLY` is the good pattern.
2. **`bApplyInverseTonemap` defaults OFF** — splats are already display-ish color; the inverse-tonemap path is dead code. Don't reason about it.
3. **The deferred (post-process) stage cannot write engine SceneDepth (read-only).** That's *the* reason the EARLY pass exists for depth. Don't try to write SceneDepth at stage 5.
4. **Reject reads SceneDepth; do not pollute SceneDepth with splat depth before the reject runs.** In the two-pass model this is naturally avoided (early writes no-opaque regions only; late reject reads opaque regions). Don't add a SceneDepth splat-write that the same-frame reject will read.
5. **`tonemap(a over b) != tonemap(a) over tonemap(b)`** — compositing post-tonemap is an approximation; fine for glass, watch for additive FX.
6. **Console-set CVars override compiled defaults** until reset/restart — always reset CVars before declaring a regression.
7. The **post-opaque delegate and the post-process passes share one RDG graph** in UE5's deferred renderer (relevant only if you ever pass RDG handles between them — but the two-pass model shouldn't need to).
8. **Don't self-test by claiming it works** — every visual claim needs the human. Propose the exact toggle/view to check.

---

## 9. CVar / setting quick reference

- `r.Harmony.Feature.Compose.DeferredBackgroundComposite` (default 1) — the after-tonemap composite path (the LATE pass vehicle).
- `r.Harmony.Feature.Compose.ProxyHybridDirect` (default 1) — proxy hybrid (compose no-opaque + direct opaque).
- `r.Harmony.Feature.Occlusion.DepthReject` — per-splat opaque reject on/off.
- `r.Harmony.Tuning.Occlusion.DepthRejectFeather` / `...Bias` / `...Mode` — feather of the reject.
- `r.Harmony.Feature.Draw.Background.WriteSceneDepth` (default 1) — stamp splat depth into SceneDepth (works at stage 3, not stage 5).
- `bUseProxyRT` (project setting) — proxy vs direct.
- `TonemappingMode=EarlyMasked` — the early custom tonemap path (the only one with the mask).

---

## 10. Definition of done

- Splats blend **softly** over opaque geometry (no hard edge) in both direct and proxy modes, EarlyMasked.
- **Transparent geometry (glass) renders correctly:** you see environment splats *through* glass; foliage in front of glass draws over it.
- No regression in non-EarlyMasked tonemap modes.
- DOF/fog see the (no-opaque) splat depth.
- Verified via the test scene + the three Harmony buffer-viz views.
