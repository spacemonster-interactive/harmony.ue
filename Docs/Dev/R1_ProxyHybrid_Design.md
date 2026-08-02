# R1 — Proxy + Localized Direct Hybrid · Design & Build Plan

> Companion to `RenderingCleanupRoadmap.md` §5 R1. This is the implementable spec.
> Build in the numbered increments — each is independently testable. CVar-gated, default off.

## Goal

In `bUseProxyRT = true`, get **direct-draw occlusion quality** at splat↔geometry interfaces
while keeping proxy's cached-compose perf everywhere else, by splitting the screen on one mask:
**is there opaque geometry at this pixel?**

- **No opaque** → composite from the proxy RT (nothing to occlude; cached color is exact).
- **Opaque present** → per-splat direct draw (each splat drawn only if closer than opaque depth).
- Mutually exclusive per pixel — never both.

End state: proxy RT becomes "cached splat color + alpha" only; average-depth occlusion,
ambiguity-resolve, and presence-mask (roadmap C5) become deletable.

## Current code (touch-points)

All in `HarmonyViewExtension.cpp` / `RenderBackgroundSplatsCommon_RenderThread`:
- **Proxy RT raster:** the direct draw writes into the proxy texture when `bUseBackgroundSplatsTexture`
  is true (`BackgroundDrawTargetTexture = bUseBackgroundSplatsTexture ? BackgroundSplatsTexture : Output.Texture`).
- **Proxy compose:** `if (bUseBackgroundSplatsTexture)` block ~`:5169`, `Harmony.ComposeBackgroundSplats`
  (`FHarmonyComposeAfterDOFPS`) — full-screen, samples proxy RT + average depth, writes `Output`.
- **Per-splat direct draw:** `Harmony.DirectDrawBackground` ~`:4698` (`FTrianglePS`), currently only
  in the non-proxy branch. **This is the pass we re-use, stencil-gated, inside proxy mode.**
- Depth-stencil states on these passes are `TStaticDepthStencilState<false, CF_Always>` today (no
  stencil) — we add a stencil read/write.

## Design

### The opaque mask
"Opaque present" = scene depth at the pixel is closer than sky/far (finite, < far plane). Source:
`SceneTexturesStruct.SceneDepthTexture`. Two ways to gate on it:
- **v1 (simple, correctness-first):** PS-side test. Compose PS skips (keeps scene color) where
  opaque; direct-draw PS skips (discard) where NOT opaque.
- **v2 (perf):** write an actual **stencil** (opaque=1) and use hardware stencil test so the
  rasterizer early-rejects — direct draw stencil-tests `==1`, compose stencil-tests `==0`.

Start v1 (no stencil plumbing); move to v2 once correct.

### Pass flow (proxy + hybrid, CVar on)
1. Raster splats → proxy RT (unchanged).
2. **Proxy compose, gated to no-opaque pixels** — where opaque, output scene color unchanged
   (the direct pass will fill those).
3. **Direct draw splats → `Output`, gated to opaque pixels**, with per-splat reject vs scene
   depth (the `EnableOpaqueDepthReject` path, same one now default for direct mode). Standard
   premultiplied-over blend, so it composites over the scene color already in `Output`.
4. Done — proxy filled the no-opaque pixels, direct filled the opaque pixels.

### Mutual exclusivity / boundary
- Compose writes only no-opaque; direct writes only opaque → no double-composite by construction.
- Boundary is the opaque silhouette (a real depth edge) → correct occlusion there. Both regions
  produce the same alpha-blended splat color, so color is continuous; only watch proxy-RT
  resolution vs full-res sampling right at the edge (feather/widen by 1px if it shows).

## Build increments (each testable)

**Inc 0 — CVar + plumbing (no behavior change).**
`r.Harmony.Feature.Compose.ProxyHybridDirect` (default 0). Thread a `bProxyHybrid` bool through
the proxy branch. No visual change yet. *Test: nothing breaks, CVar registered.*

**Inc 1 — Compose skips opaque pixels.**
Add `ComposeOpaqueMaskMode` to `FHarmonyComposeAfterDOFPS`; when hybrid on, the compose PS samples
scene depth and outputs scene color unchanged where opaque present. *Test: with CVar on, splats
vanish over opaque geometry (holes where the car is), proxy intact elsewhere. Confirms the mask.*

**Inc 2 — Direct draw fills opaque pixels.**
In the proxy branch, additionally issue the `FTrianglePS` direct draw into `Output` with
`EnableOpaqueDepthReject=1` and a new "draw only where opaque present" gate in its PS. *Test:
splats reappear over the car, now per-splat occluded; blending matches `bUseProxyRT=false`.*

**Inc 3 — Stencil optimization (v2).**
Replace the two PS-side masks with a real stencil (opaque=1) so rasterizer early-rejects. *Test:
identical image, lower cost in `ProfileGPU` (direct pass no longer shades no-opaque pixels).*

**Inc 4 — Retire C5.**
Once Inc 1–3 hold across the mode matrix, stop allocating/using proxy average depth + ambiguity
resolve + presence mask in this path. Proxy RT → color+alpha only. *Test: no regression; simpler.*

**Inc 5 (optional) — nearest-splat-depth band.**
Only if profiling shows opaque-region direct draw is too costly: capture alpha-gated
`nearestSplatDepth` (Min-blend channel) and shrink the direct region to the ambiguous band
(`nearestSplatDepth < opaqueDepth`). Perf-only.

## Risks / open questions
- **Cannot self-test** — every increment needs human visual verification across proxy×split×
  tonemap×static/moving. Inc 1 ("holes over geometry") is the clean first checkpoint.
- Translucent UE geometry / particles don't write opaque depth → they fall in the proxy region.
  Confirm acceptable (probably yes).
- Interaction with the existing deferred-composite + EarlyMasked tonemap in proxy mode — verify
  the direct draw in the opaque region lands at the right pipeline stage relative to tonemap.
- Foreground split (C1) is slated for removal; ideally remove it first so this integrates into a
  simpler base, but R1 can proceed independently.

## Status
- [ ] Inc 0  - CVar + plumbing
- [ ] Inc 1  - compose skips opaque
- [ ] Inc 2  - direct fills opaque
- [ ] Inc 3  - stencil
- [ ] Inc 4  - retire C5
- [ ] Inc 5  - (optional) nearest-depth band
