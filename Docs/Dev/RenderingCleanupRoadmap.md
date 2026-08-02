# Harmony Rendering — Cleanup & Architecture Roadmap

> Status: **planning only**. Nothing here authorizes a code change on its own.
> This document exists so that when we eventually do a cleanup pass, we remove the
> *right* things and don't break the subtle, non-obvious couplings that this code is full of.

---

## 0. Prime directive (read first)

**KEEP is the default. A path is only removed once its intent is confirmed and written down.**

Rules for any future cleanup work (human or model):

1. **Do not remove a path because it "looks redundant."** Several paths in this code exist
   for non-obvious reasons (see §3 Landmines). Each candidate in §4 has a **"Must verify
   before removing"** gate — treat it as blocking.
2. **Gate, ship, A/B, *then* delete.** Prefer: introduce a CVar defaulting to current
   behavior → verify the replacement visually → flip the default → only then delete the old
   path. Don't delete speculatively in one pass.
3. **Rendering cannot be unit-tested.** Every change needs human visual verification across
   the mode matrix (proxy × split × tonemap × static/moving camera). Budget for that, and
   never assume "it compiles" means "it's correct."
4. **If intent is unknown, it is `NEEDS CONFIRMATION`, not `remove`.** Write the open
   question down; do not guess.

---

## 1. Why this is needed

The compose/occlusion behaviour is a **product of many orthogonal axes**, and the number of
combinations has outgrown what's safely reason-about-able. Symptom: fixing one thing breaks
another. Root cause: multiple overlapping mechanisms solving the same problem, plus shared
mutable state with implicit coupling.

Even careful code reading mis-predicted behaviour several times during the session that
produced this doc (e.g. "direct mode is per-splat" was wrong — it was secretly using averaged
depth). That mis-prediction *is* the evidence that the abstraction leaks and needs tightening.

---

## 2. Current architecture map

### Orthogonal axes (the matrix)
- `bUseProxyRT` — proxy compose (cached, flattened) vs direct draw (per-splat, expensive)
- `TonemappingMode` — `EarlyMasked` / `TonemapCompensation` / `StockTonemapping`
- `r.Harmony.Feature.Compose.DeferredBackgroundComposite` (new) — composite splats *after* the early tonemap
- `r.Harmony.Tuning.Occlusion.UseResolvedDepthReject` (new) — averaged vs per-splat occlusion for direct draw
- layer partition on/off; static-view-preprocess cache on/off

### Key code
- `RenderBackgroundSplatsCommon_RenderThread` (~1500 lines) — the workhorse: proxy + direct +
  depth-write + ambiguity all in one function. **Primary complexity hotspot.**
- `RenderCustomTonemapPass_RenderThread` — EarlyMasked early tonemap (hooks `BeforeDOF`).
- `RenderDeferredBackgroundSplatsPass_RenderThread` (new) — deferred composite after the tonemap.
- `RenderBackgroundSplatsPostOpaque_RenderThread` — post-opaque entry into Common.
- `s0.usf` — preprocess (covariance projection), sort, layer partition, direct-draw PS (reject),
  compose PS, masked-tonemap PS.

---

## 3. Landmines / non-obvious couplings (READ BEFORE TOUCHING ANYTHING)

These are the traps that make "clean up the wrong thing" easy:

- **Averaged-depth RT is dual-purpose.** It feeds *both* occlusion *and* the
  splat→SceneDepth write (for DOF). Removing the averaged occlusion must not silently kill the
  depth-write path.
- **EarlyMasked mask depends on coverage textures written by the post-opaque direct draw.**
  Moving/removing the post-opaque draw breaks the mask. (This coupling is exactly why the
  deferred-composite approach can drop the mask entirely — it tonemaps a splat-free scene.)
- **The opaque presence-mask hard-discard is gated on `DirectDrawDebugViewMode == 0`.** The
  debug views (`r.Harmony.Debug.Background.Draw.View 1..6`) therefore **do not show** it.
  Debugging occlusion purely via debug views is misleading — confirmed the hard way.
- **`UseOpaquePresenceMaskReject` is proxy-only** (`bUseBackgroundSplatsTexture` required). It
  does *not* run in direct mode. Don't attribute direct-mode artifacts to it.
- **`bUseResolvedBackgroundDepthRejectForDirectDraw` couples three things**: the occlusion
  source (averaged vs per-splat), the averaged-depth *write*, and the separate depth pass.
  Touching one affects the others.
- **The preprocessed-splat buffer is stride-5 (`float4`/splat).** Every shader that indexes it
  must use `*5`. A `*4` mismatch silently corrupts (this was a real bug — `LayerPartitionCount`/
  `Scatter`). **Adding any new per-splat field changes the stride everywhere** — high-blast-radius.
- **`HARMONY_UE_5_8_OR_LATER` is defined as `UE_VERSION_NEWER_THAN(5,7,0)`** — i.e. true for
  5.7.x too, despite the name. Engine-version conditionals here are fragile; 5.7 is a "broken
  middle" (new ACES API, old header layout).

---

## 4. Cleanup candidates (with risk + verification gates)

> Format: each has intent (often a *hypothesis* — verify it), evidence, the blocking gate, and risk.

### C1 — Legacy Background/Foreground split  ·  **PARTIALLY REMOVED (remaining execution = HIGH RISK)**
- **Removed:** project setting, config/CVar hooks, component split fields, preprocess layer filter,
  and per-splat background/foreground classification.
- **Remaining:** layer partition scaffolding and the dedicated foreground pass.
- **Intent:** was meant to draw "foreground" splats unconditionally in front of opaque geometry
  (no depth reject) as a separate late layer.
- **Decision (product owner, this session):** no compelling use case — adds complexity without
  meaningful gain. **Cleared for removal.**
- **Execution risk: HIGH.** Deeply wired: layer partition (count/prefix/scatter), per-splat layer
  classification in preprocess, the separate foreground pass, and several CVars. (Was the source
  of the stride bug.) Remove in its **own** change, incrementally, re-verifying the full mode
  matrix. **Do NOT bundle with R1.**
- **Sequencing:** preferably land this BEFORE R1 so R1 integrates into a simpler base — but they
  are largely independent and can go either order.

### C2 — Averaged-depth occlusion reject (direct)  ·  **LIKELY REMOVABLE**
- **What:** `UseBackgroundDepthComposeReject` / `ComputeDirectBackgroundDepthVisibility`.
- **Status:** Already off-by-default via `UseResolvedDepthReject 0` (this session). Per-splat is
  now the direct-mode default and blends correctly.
- **MUST VERIFY before deleting outright:** nothing but direct occlusion relies on it; and the
  averaged-depth *write* is still available for the splat→SceneDepth (DOF) path where needed.
- **Risk:** MEDIUM (the dual-purpose RT coupling — see §3).

### C3 — EarlyMasked coverage mask + temporal history  ·  **LIKELY REMOVABLE if deferred becomes default**
- **What:** the selective-tonemap coverage mask, `SceneCoverageHistory` (temporal reprojection),
  velocity-falloff, cross-blur — the whole apparatus that hides the tonemap seam.
- **Evidence:** the deferred composite removes the *need* for the mask (tonemap runs on a
  splat-free scene). The mask's motion-dependent seam was a real defect.
- **MUST VERIFY before removing:** does the deferred path cover **every** case the mask did —
  in particular **proxy mode** and **split foreground**? Confirm those tonemap paths before
  deleting the mask machinery.
- **Risk:** MEDIUM–HIGH (lots of state; used across modes).

### C4 — Debug / experimental CVars  ·  **COLLAPSE WINNERS, DELETE REST**
- **What:** the many `r.Harmony.Debug.*` and `r.Harmony.Tuning.*` knobs.
- **Action:** once a value is chosen, bake it in and delete the toggle.
- **MUST VERIFY:** none are load-bearing in `DefaultEngine.ini` / project config before deleting.
- **Risk:** LOW each, but do it in small batches with a config grep.

### C5 — Proxy occlusion: presence mask + ambiguity resolve  ·  **CONSOLIDATE (pending R1)**
- **What:** the hard presence-mask discard + the ambiguity-resolve pass (both proxy-side).
- **Evidence:** both are attempts to compensate for proxy depth flattening.
- **Note:** if the hybrid (R1) lands, both are likely *replaced* by the localized direct band.
  Do not remove before R1 exists.
- **Risk:** MEDIUM. Tied to R1.

---

## 5. Architecture roadmap (FORWARD — improvements / replacements, not pure removals)

### R1 — Hybrid proxy + localized direct draw  ·  **flagship idea**
- **Goal:** keep proxy perf where it's free, get full direct-draw quality where it matters, by
  splitting the screen on a trivial mask: **is there opaque geometry at this pixel?**

- **Pipeline (v1 — the simple model):**
  1. Rasterize all splats → proxy RT. **Color + alpha only — no average depth needed.**
  2. Build a mask from the scene depth buffer: **opaque present** (depth closer than sky/far) vs not.
  3. **No-opaque pixels → composite from the proxy RT.** Nothing to occlude against, so the cached
     alpha-blended splat color is exactly correct (proxy == direct here).
  4. **Opaque pixels → direct-draw splats** with per-splat reject (each splat drawn only if its
     own depth is closer than the opaque depth). Restrict via **stencil** so the rasterizer skips
     the no-opaque region.
  5. **Mutually exclusive** — proxy *or* direct per pixel, never both (no double-composite).

- **Why it's correct (= full direct quality):** no-opaque has no occluder so proxy == direct;
  opaque uses true per-splat occlusion. Result equals full direct draw everywhere, paying the
  direct cost only over opaque-present screen area.

- **Why it's the big simplification:** the proxy never has to occlude against geometry, so it no
  longer needs averaged depth. This makes **C5 deletable outright** — proxy averaged depth, the
  **ambiguity-resolve** pass, and the proxy **presence-mask** all become obsolete. The proxy RT
  returns to being *just "cached splat color + alpha."* The per-splat reject used in the opaque
  region is the same path now default for direct mode (`UseResolvedDepthReject 0`).

- **Perf characteristic:** cost ∝ opaque-present screen area. Lots of sky/open → mostly cheap
  proxy. Mostly geometry → degrades toward full direct, which is the correct cost for quality
  there anyway.

- **v2 optimization (OPTIONAL — only if profiling demands):** shrink the direct region from "all
  opaque pixels" to just the **ambiguous band**, by also capturing a per-pixel **`nearestSplatDepth`**
  (Min-blend channel, **alpha-gated** so faint Gaussian tails don't balloon the band) and skipping
  direct where `nearestSplatDepth >= opaqueDepth` (all splats already correctly occluded). This is
  the earlier, more complex design — a perf refinement, **not** needed for correctness in v1.

- **Open design questions:**
  - Proxy↔direct boundary continuity at opaque silhouettes (both produce the same alpha-blended
    color; main risk is proxy-resolution vs full-res sampling at the edge — feather if needed).
  - How "opaque present" should treat translucent UE geometry / particles (they don't write
    opaque depth → fall in the proxy region; confirm that's acceptable).
  - Stencil setup cost vs. benefit.

- **Action:** when prioritized, give R1 its own design doc. It is the path that lets C5 retire.

### R2 — Depth-extent (soft) reject
- **Goal:** soft silhouettes. Give each splat a view-Z extent (derivable from its covariance);
  fade the splat over its own depth thickness as it crosses a surface, instead of flipping at a
  single center depth.
- **Helps:** direct (soft silhouettes) and — if a depth *variance* channel is stored — proxy too.
- **Cost note:** adds a per-splat field → changes the stride-5 buffer layout (see §3 landmine).

### R3 — Unify the compose path
- **Observation:** proxy and direct have drifted into largely separate compose paths → both the
  quality gap *and* the maintenance burden. Unify so both feed the **same** reject (proxy
  supplies averaged + variance into the shared per-pixel test). Shrinks the matrix and narrows
  the gap in one move.

---

## 6. Changed this session (context for future readers)

- **`s0.usf`:** `LayerPartitionCount`/`Scatter` stride `*4u` → `*5u` (real corruption bug fix).
- **Deferred composite:** new pass `RenderDeferredBackgroundSplatsPass_RenderThread` +
  CVar `r.Harmony.Feature.Compose.DeferredBackgroundComposite` (default 0). Composites splats
  after the early tonemap (BeforeDOF) so the tonemapper runs splat-free → no coverage mask /
  no motion seam. Direct-draw (proxy-off) only.
- **Per-splat direct occlusion:** CVar `r.Harmony.Tuning.Occlusion.UseResolvedDepthReject`
  (default 0 = per-splat). Makes proxy-off occlude each splat against its own depth instead of
  the averaged-depth RT. **This is what fixed the direct-mode blending.**
- **`SceneViewState.h`:** wrapped in `#if __has_include(...)` for 5.7/5.8 compat.

---

## 7. Suggested sequencing (when we start)

1. **Document intent** for C1 (foreground split) and C3 (mask) — answer the verification
   questions *before* any deletion.
2. **Make deferred composite + per-splat the default** for EarlyMasked direct, A/B it broadly.
3. Only then retire C2/C3 machinery that's provably dead.
4. Design **R1** (hybrid) separately; it's the path to making proxy good enough that C5 can go.
5. C4 (CVar collapse) can happen incrementally alongside the above, with config greps.
