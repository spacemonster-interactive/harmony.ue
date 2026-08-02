# Harmony Rendering Architecture

## Purpose

Harmony renders Gaussian splats as first-class participants in an Unreal scene. The desired result is not simply a background image placed behind Unreal geometry. Splats must:

- interleave convincingly with opaque and translucent Unreal content;
- retain their authored/display appearance rather than being unintentionally washed out by Unreal's tonemapper;
- participate in depth-driven Unreal features such as depth of field;
- remain compatible with temporal anti-aliasing/upscaling and motion;
- preserve soft Gaussian boundaries at splat/geometry intersections; and
- remain affordable when millions of overlapping splats are visible.

These goals conflict with one another because Unreal does not expose a single render stage at which all of them can be satisfied. Harmony therefore uses two splat stages with different responsibilities.

This document records the stable intent, constraints, and accepted compromises. It deliberately avoids source line numbers, exact callback names, and most console variables so it remains useful as the implementation changes.

## Production Direction

The intended production architecture uses a dedicated proxy render target for both splat stages:

1. The **early splat pass** rasterizes its contribution into an early splat layer.
2. The early layer is composed into the Unreal scene before normal translucency.
3. Harmony selectively tonemaps the Unreal content while preserving the intended splat appearance.
4. The **late splat pass** rasterizes its contribution into a separate late splat layer.
5. The late layer is composed before depth of field.

The early and late layers are not interchangeable caches of the same result. They have different pixel ownership, depth tests, timing, and cacheability.

For enclosing environment captures, the optional global **Treat Early Background Splats as Opaque** policy can resolve every qualifying non-empty pixel of the cached early proxy as opaque. Because the proxy stores premultiplied color, composition first divides RGB by accumulated coverage and then sets alpha to one; forcing alpha alone would darken low-coverage pixels. An advanced console-variable threshold can reject numerical or excessively faint tails when diagnosing an asset. This policy is deliberately limited to the cached early/background proxy and does not alter late foreground splats, rasterization, preprocessing, or sorting. Its cost is confined to a small branch and arithmetic in the existing fullscreen proxy-composition and coverage-mask passes.

Production splat rasterization is proxy-only. Harmony may still use fullscreen composition passes to
merge a proxy layer into scene color, but splat geometry itself is never rasterized directly into the
scene-color target. If a required proxy resource is unavailable, Harmony skips that splat stage rather
than silently selecting the retired direct path.

## Why Two Passes Are Required

### Early-only is insufficient

Drawing every splat before translucency gives Unreal glass and other translucent materials the expected opportunity to blend over splats. It can also write useful splat depth while scene depth is writable. However, early splats would ordinarily pass through Unreal's tonemapper and change appearance. Early drawing alone also cannot resolve every splat that belongs in front of already rendered Unreal content without careful ownership and depth rules.

### Late-only is insufficient

Drawing every splat after translucency protects splat color from the native tonemapper and makes it easy to place splats over opaque Unreal geometry. It also causes splats that should be behind glass, modulation planes, or other translucency to paint over those materials. At this stage engine scene depth is effectively read-only, so depth of field and related systems cannot receive newly written splat depth.

### Split ownership

Harmony therefore divides the final splat result:

- **Early owns background/behind-translucency contributions.**
- **Late owns contributions that must appear over opaque geometry or in front of participating
  translucency.**

The ownership tests must be complementary. A visible splat contribution should belong to one stage, not neither stage and not both stages. Small conservative overlap may be used at a boundary when it is required to prevent single-pixel seams, but it must not create visible double blending.

## Conceptual Pipeline

```text
Unreal opaque geometry and opaque SceneDepth
                    |
                    v
Splat preprocess, cull and global depth sort
                    |
                    v
EARLY SPLAT RASTER -> early proxy RT (+ coverage/depth products)
                    |
                    v
Compose early proxy into the pre-translucency scene
                    |
                    v
Unreal translucency, glass and modulation materials
                    |
                    v
Harmony selective/custom tonemap (Early Masked mode)
                    |
                    v
LATE SPLAT RASTER -> late proxy RT
  - reads opaque depth
  - may read participating translucent CustomDepth
  - applies late ownership and per-splat occlusion
                    |
                    v
Compose late proxy into the tonemapped scene
                    |
                    v
Depth of field and remaining post processing
                    |
                    v
Harmony tonemap replacement/pass-through when required
```

The exact Unreal callbacks may move between engine versions. The relative ordering above is the architectural requirement.

Path Tracing is a deliberate special case. Its complete SceneColor is not
available at the post-opaque early hook, so Harmony does not insert splats into
the traced HDR image before tonemapping. In Early Masked mode the PT route is:

```text
Path-traced premultiplied foreground + inverse-opacity
                    |
                    v
Harmony full-screen custom tonemap
  - unpremultiply foreground RGB
  - tonemap straight foreground
  - restore premultiplication
                    |
                    v
Composite the complete cached splat layer underneath
  - use the pre-tonemap PT inverse-opacity snapshot for ownership
  - keep splats display-referred (never inverse-tonemap them)
  - decode splat sRGB only when MRQ's target stores linear WithToneCurve data
  - update inverse-opacity with the splat layer for MRQ output
                    |
                    v
Depth of field and remaining post processing
                    |
                    v
Harmony tonemap replacement/pass-through
```

This avoids a selective nonlinear-tonemap boundary in PT. Harmony snapshots
the path tracer's inverse-opacity before tonemapping because PIE commonly
retains that channel in the intermediate target while MRQ may replace or
reinterpret it. The snapshot supplies exact background visibility to both the
cached proxy compose and its complementary late draw. The proxy compose also
subtracts its premultiplied splat coverage from the remaining transmittance so
MRQ's surface readback produces an opaque background rather than showing the
splat RGB only through partially transparent foreground pixels. MRQ's
`FinalToneCurveHDR` path is representationally different from the PIE display
target: it expects linear sRGB before output encoding. Harmony therefore
decodes only the post-tonemap splat layer for that output device; this is not a
second tonemap and does not change the intended displayed splat appearance.

Stock Tonemapping and Tonemap Compensation leave UE's stock tonemapper active.
MRQ's stock tonemap/output route does not reliably include Harmony's splat
coverage in final alpha, even though the splat RGB survives. Harmony therefore
restores only alpha after the stock tonemap while copying UE's final RGB
unchanged. PT restores the exact inverse opacity saved after its explicit
under-composite. Deferred consumes the remaining native-scene transmittance
with the complete cached splat-proxy opacity. Without this repair, empty splat
background becomes transparent in MRQ while the same background remains
visible through vehicle glass because the translucent foreground supplies
non-zero output alpha.

The PT camera background must remain transparent for this under-composite.
`r.PathTracing.VisibleLights=1` writes every directly visible light into camera
rays, while `2` writes the skydome only. That radiance is already integrated
into SceneColor, including through glass, and cannot be separated from the
foreground using the final alpha channel. By default
`r.Harmony.Feature.PathTracing.SuppressCameraVisibleLights=1` temporarily
forces camera-visible PT lights off while an active Harmony splat scene is
rendered, then restores the configured UE value when Harmony splats are no
longer active. This affects camera visibility only; the skylight continues to
illuminate and reflect in path-traced geometry.

## Shared Preprocess

Both stages consume one common preprocessed and globally depth-sorted splat stream. Preprocess is responsible for work that is cheaper to perform once per splat than repeatedly per fragment:

- instance transforms;
- frustum and near-plane culling;
- projected axes and centre;
- spherical-harmonic color evaluation;
- per-instance opacity and depth offsets;
- whole-splat alpha and screen-size rejection;
- optional splat-level volume evaluation;
- stable sort keys and sorted indices.

The globally sorted order is important for Gaussian alpha blending. Splitting affected and unaffected splats into unrelated draw lists can produce incorrect results when their depths interleave. Prefer precomputed attributes or tags consumed by one sorted raster unless an order-preserving partition and merge is explicitly implemented.

## Early Splat Pass

### Responsibilities

The early pass should do only work required to build the cacheable background splat layer:

- project and rasterize sorted splat quads;
- evaluate the Gaussian and fragment alpha threshold;
- apply base color, opacity, camera fade, and ground fade;
- output premultiplied color and coverage;
- output the depth information needed by early composition and optional scene-depth writing;
- apply temporal projection jitter while temporal accumulation is active.

The early proxy should remain geometry-independent wherever practical. This is what allows a static camera/splat view to reuse the expensive raster even when unrelated Unreal geometry changes.

### Cache policy

The early proxy is the primary persistent cache candidate. It may be reused only when every input that changes its pixels is stable, including:

- camera and projection;
- render extent and view rectangle;
- splat assets and instance transforms;
- splat appearance and culling settings;
- preprocess-affecting volumes;
- temporal jitter state.

If the early proxy is made dependent on current opaque or translucent geometry, those geometry changes must invalidate it. Avoiding that dependency is an intentional performance/ordering compromise.

### Depth writing

Early splat depth may be committed separately from early color. Deferring the scene-depth commit until after normal translucency prevents synthetic splat depth from incorrectly rejecting modulation or shadow geometry, while still allowing later depth-driven effects to observe splats.

Coverage thresholds are required because low-alpha Gaussian tails should not indiscriminately stamp scene depth.

## Late Splat Pass

### Responsibilities

The late pass produces only the contribution that must be applied after Harmony's early tonemap:

- identify pixels/splats owned by the late stage;
- read the pristine opaque-depth reference used by the stage split;
- perform per-splat depth rejection against Unreal geometry;
- use participating translucent CustomDepth when an exact translucent split is requested;
- preserve the soft Gaussian/geometry boundary;
- output a transparent premultiplied late layer;
- composite that layer before depth of field.

### Render resolution

The late layer should rasterize at the same render resolution and use the same viewport mapping as the early layer. Under temporal upscaling, rendering one stage at render resolution and the other at output resolution produces displaced silhouettes and seams. The late layer can be spatially upscaled when it is composed.

### Per-splat occlusion remains essential

Using a proxy RT does not imply using one averaged proxy depth to decide all occlusion during the fullscreen composite. That approach creates hard boundaries and cannot represent multiple splat depth layers at a pixel.

The late raster must perform the important depth and ownership tests per splat/per fragment while constructing the late RT. The final fullscreen operation should normally be a straightforward premultiplied-over composite.

### Cache policy

The late layer depends on Unreal geometry and is less naturally cacheable. Reuse requires stability of the camera, splats, opaque-depth ownership, participating translucent depth, relevant volume state, and every other input used by the late shader. A stale late layer can place splats on the wrong side of moving geometry, so correctness takes priority over speculative caching.

## Production Shader Roles

The production shaders should be explicit rather than one general shader with a large permutation and runtime branch surface.

### Dedicated early shader

The early shader should expose only the parameters and interpolators needed by the early proxy. It should not contain late geometry gates, opaque-depth rejection, ambiguity/debug paths, or fragment-volume loops that are not part of the selected production quality mode.

The current implementation uses `FHarmonyEarlySplatProxyVS/PS` with explicit
`HarmonyEarlySplatProxyVS/PS` entry points for the normal production proxy path. Fragment-stage
Harmony volumes are deliberately ignored by this path for now; future volume support should be
resolved before rasterization or by an explicitly selected higher-cost mode.

### Dedicated late shader

The late shader should contain only:

- Gaussian/color/alpha behavior shared with the early pass;
- late-stage ownership tests;
- opaque and participating-translucent depth interaction;
- stage-boundary expansion/feathering needed for seam-free composition;
- late-specific diagnostics that cannot be reproduced by the general debug shader.

The current implementation uses `FHarmonyLateSplatProxyVS/PS`. Its pixel-shader domain contains
only opaque-depth snapshot, opaque-depth reject, and fragment-volume choices; early-stage, MRT,
stencil, direct-draw, and debug permutations are fixed out. During the behavior-preserving
transition it shares the established raster implementation with the general shader.

### General/debug shader

A general shader remains useful for:

- buffer visualizations;
- overdraw and depth diagnostics;
- experimental rendering modes;
- specialized diagnostic permutations.

It should not silently become the normal production path merely because a debug feature exists.

The early and late production proxy passes always use their dedicated shaders. The general shader is
reserved for diagnostic and non-production permutations.

### Shared shader code

Dedicated entry points must use shared helper functions for:

- Gaussian evaluation;
- base color conversion;
- camera and ground fades;
- alpha cutoff policy;
- depth conversion;
- volume shape math where applicable.

This limits duplication and reduces the risk that early and late splats acquire different color,
opacity, or support-radius behavior.

## Volume Evaluation

Harmony currently needs two broad classes of volume behavior:

- alpha removal, optionally with a feathered boundary or camera-distance rule;
- depth offset within a volume.

Per-fragment volume evaluation gives the finest boundary but can add an unacceptable cost because it
reconstructs world position and evaluates volume rules across heavy Gaussian overdraw.

The preferred quality tiers are:

1. **Fast production:** dedicated early shader; no fragment-volume loop.
2. **Splat-level volumes:** evaluate volume influence during preprocess and store alpha/depth results per splat. This preserves the production raster and should be the normal volume solution.
3. **Pixel-exact volumes:** explicit expensive mode for fragment-accurate boundaries when the visual difference justifies the cost.

Preprocess hard culling by splat centre already demonstrates the fast model. It can be extended with
per-splat alpha multipliers and depth offsets. Recompute should be driven by splat, instance, volume,
camera-rule, or settings dirtiness and should occur on the GPU rather than scanning large splat sets
on the CPU.

Any pixel-exact mode must be independently tested. It should not prevent the production shader from being
the default when no working pixel-exact effect is required.

## Tonemapping Modes

Tonemapping mode changes both color handling and which split stages are meaningful.

### Early Masked

**Intent:** preserve authored splat appearance while keeping Unreal geometry close to stock
tonemapping.

- Early splats are present before translucency.
- Unreal translucency can blend or modulate over them.
- Harmony runs its replacement/custom tonemap before depth of field.
- A scene-coverage mask applies tonemapping to Unreal content while preserving selected splat pixels.
- The late splat layer is added after this custom tonemap and before depth of field.
- Harmony replaces the later native Unreal tonemap with a pass-through because tonemapping has already occurred.
- Path Tracing instead tonemaps its splat-free foreground across the whole
  screen and composites the complete cached splat layer underneath afterward;
  it does not use the selective scene-coverage mask.

This is the most capable integration mode and the one that needs the full early/late architecture. It also carries the greatest complexity: mask accuracy, translucency classification, proxy ownership, and matching Unreal's stock appearance.

The deferred scene-coverage texture has two distinct contracts. RGB stores the
tonemap-selection diagnostics and may be blurred, temporally reprojected, or
morphologically adjusted. Alpha stores raw visible early-splat opacity and must
not receive any of those operations. Final UE inverse opacity is composed as
`T_after = T_scene * (1 - splatOpacity)` before MRQ converts it to conventional
PNG alpha. A modulation-only shadow plane changes scene RGB but must not change
this physical coverage channel. `r.Harmony.Debug.Compose.Tonemap.View 12`
visualizes the early-splat opacity used by the deferred alpha composition.

When there are no splats, Harmony should leave Unreal's native tonemapper active rather than paying for or visually approximating it.

### Tonemap Compensation

**Intent:** retain Unreal's native tonemapper while pre-expanding splat color so the final result more closely resembles the authored splat appearance.

- Harmony does not replace Unreal's tonemapper.
- Splat color is compensated before entering the scene.
- Unreal then tonemaps splats and geometry together.
- Compensation is an approximation and can vary with exposure, local exposure, film settings, color grading, and scene luminance.

This mode has lower tonemap-pipeline complexity but cannot guarantee unchanged splat pixels. The Early Masked late-after-custom-tonemap stage is not available in the same form because Harmony has not already produced the final tonemapped scene. Contributions must be placed at a stage consistent with the native tonemapper and accepted as tonemapped content.

### Stock Tonemapping

**Intent:** leave Unreal's native color pipeline untouched.

- Splat pixels enter scene color without compensation.
- Unreal's stock tonemapper, exposure, local exposure, grading, and film curve apply normally.
- Unreal geometry receives the most exact stock treatment.
- Splat appearance may become brighter, darker, desaturated, or washed out relative to its authored display-space color.

This is the simplest and most predictable mode from Unreal's point of view. It is appropriate when native Unreal content is primary or when splats are intentionally treated as ordinary HDR scene content.

### Mode comparison

| Mode | UE tonemapper | Splat appearance | Full early/late split | Main compromise |
|---|---|---|---|---|
| Early Masked | Harmony custom pass; later UE pass suppressed | Best preservation target | Yes | Mask and pass-order complexity |
| Tonemap Compensation | Native UE | Approximate compensation | Limited by native tonemap ordering | Cannot exactly invert a scene-dependent tonemapper |
| Stock Tonemapping | Native UE | Tonemapped normally | Limited by native tonemap ordering | Authored splat color changes |

## Translucency and CustomDepth

Opaque SceneDepth cannot describe translucent surfaces. Participating translucent meshes may write CustomDepth and an optional stencil identifier so Harmony can compare individual splat depth with the frontmost participating translucent surface.

Requirements for an exact participating surface include:

- the material allows translucent CustomDepth writes;
- the mesh component renders CustomDepth;
- the stencil configuration, if used, matches Harmony's selection.

CustomDepth supplies only one participating depth per pixel. It cannot represent arbitrary stacks of overlapping translucent surfaces and splat layers. Modulate/shadow materials may also need different ownership policy from ordinary glass because they alter the background rather than replace it.

Keeping the early proxy geometry-independent improves caching but makes its translucent ordering an approximation. The late pass can restore splats known to be in front of participating CustomDepth, but a single cached early RGBA/depth result cannot perfectly separate every mixed front/behind splat layer at the same pixel.

## Temporal Integration

The early pass occurs before Unreal's temporal resolve and can use Unreal's current temporal jitter so TSR/TAA accumulates subpixel splat detail.

The late pass occurs after that temporal resolve in the current ordering. It must not independently apply the same temporal jitter because no matching temporal resolve follows it. Rendering it at the same base render resolution as the early pass preserves spatial boundary alignment, but the late layer itself has no native temporal history.

Static-view proxy reuse and temporal jitter conflict: a proxy cannot be reused while each temporal
sample intentionally differs. The **Splat Temporal Super Resolution** setting makes this trade-off explicit:

- enabled and at or below its configured maximum screen percentage: the early proxy follows Unreal's
  jitter every frame, preserving TAA/TSR subpixel detail while still allowing preprocess and sort reuse;
- disabled or above its maximum screen percentage: the early proxy uses zero jitter and may be
  completely reused for a stable view.

Harmony does not transition automatically between these modes because replacing the jittered sequence
with a cached zero-jitter proxy produces a visible stationary-view quality drop.

## Motion and Velocity

Splat color reuse, preprocess reuse, and depth/velocity reuse are separate decisions. A stable camera does not imply a stable scene.

Where Harmony writes splat depth for Unreal effects, it should provide compatible velocity for the same ownership region when reliable motion information exists. Missing or stale velocity can cause TSR/DOF trails even when the color composite appears correct.

A future late-layer cache must use explicit scene stability, not merely lack of camera motion. Screen-space velocity can help reject reuse, but zero velocity is not by itself proof that all depth/ownership inputs are unchanged.

## Known Compromises

1. A single proxy color plus averaged depth cannot represent multiple independently ordered splat layers at one pixel.
2. Participating CustomDepth represents only the frontmost selected translucent depth.
3. Geometry-independent early caching trades exact moving-translucency ordering for performance.
4. Late splats cannot write engine SceneDepth at the same stage at which they are composited.
5. The late layer does not pass through the same temporal resolve as the early layer.
6. Tonemap compensation cannot exactly invert Unreal's scene-dependent tonemapper.
7. Harmony's custom tonemap aims for stock parity, but engine-private inputs or future engine changes can prevent pixel identity.
8. Boundary expansion may deliberately move ownership by a pixel to eliminate cracks caused by resolution conversion and sampling.
9. Splat-level volume feathering is less exact than per-fragment feathering but is expected to be the practical production trade-off.

## Architectural Invariants

Future rendering changes should preserve these rules:

- Early and late ownership use the same coordinate mapping and compatible depth reference.
- The production path does not shade the full general/debug fragment program unnecessarily.
- Late occlusion is not reduced to a single averaged proxy-depth comparison.
- Splats behind participating translucency remain available to the early/translucency composition.
- Splats in front of participating translucency can be restored by the late stage.
- Synthetic splat depth must not prevent normal translucency/modulation from rendering.
- Both proxy layers use premultiplied color and alpha consistently.
- Early and late render rectangles remain aligned under dynamic resolution and TSR.
- Cache keys include every input that can change the cached pixels.
- No-splat views leave Unreal's stock rendering path alone.

## Regression Matrix

At minimum, rendering changes should be checked against:

| Area | Required cases |
|---|---|
| Tonemapping | Early Masked, Tonemap Compensation, Stock |
| Geometry | No UE geometry, opaque in front of splats, opaque behind splats |
| Translucency | Glass over splats, splats in front of glass, modulate/shadow plane |
| Boundaries | Opaque/splat silhouette at render=output and with TSR upscale |
| Depth effects | DOF with foreground/background splats; depth-write on/off |
| Motion | Moving camera, moving opaque object, moving translucent object |
| Cache | Reuse off, Splat Temporal Super Resolution on, Splat Temporal Super Resolution off/full proxy reuse, invalidation |
| Volumes | Hard preprocess cull, feather, depth offset, moving volume |
| Diagnostics | Early only, late only, proxy layers, coverage/depth views |

Visual verification should be paired with `ProfileGPU`. A result is not considered a production
improvement if it fixes one mode by silently selecting a slower general shader or invalidating the
proxy every frame.

## Planned Simplification

The next architectural cleanup is:

1. Treat proxy RT rendering as the only production early and late path.
2. Introduce dedicated early and late production shader entry points.
3. Retain one general/debug shader for diagnostics and experiments.
4. ~~Remove direct scene-color raster logic after proxy parity is verified.~~ Complete.
5. Keep the dedicated early feature set as the production default.
6. Move practical volume behavior into splat-level preprocess data.

This order keeps the current visual baseline testable while progressively removing the large
conditional surface that has made performance and ordering regressions difficult to reason about.
