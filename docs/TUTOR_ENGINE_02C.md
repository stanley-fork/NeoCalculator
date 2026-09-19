# TUTOR-ENGINE-02C — checked periodic trigonometric families

Implementation candidate, September 2026. No commit, push or hardware write was
performed. This report describes host/emulator/build evidence, not physical
acceptance or ESP32 timing measurements.

## Preserved baseline

- Repository: `C:\Users\Juan Ramón\Documents\Calculadora`, branch `main`.
- HEAD: `fb8a2b329c3cf14547a7ba4cbc46e4125211edb6`; its parent is the accepted
  02B commit `8b10bfbefbb2df558f471052990b65e76fc93e02`.
- Initial index and working tree were clean. The newer HEAD only ignores the
  unrelated `.vscode/settings.json`; its bytes remain untouched.
- Full baseline fingerprint:
  `934f4981eb2547c67500a235ed291e0aa419869ed1634e07ac61240dafb5a6da`.
- Runtime baseline fingerprint:
  `671ee924b1dcd7bd8fd5d24a23675ae0754f14372c58fe5ed2a9295fb15639eb`.
- Editor SHA-256:
  `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.

Readable source snapshot, separate binary-capable index/worktree patches and
path/hash manifest are under `out/tutor-engine-02c/`. Builds use the explicit
ASCII snapshot `C:/.codex-cache/tutor-engine-02c/source`, separate native,
firmware, fixed-pool and WASM build directories, and existing pinned tools.

## Supported contract

| Input, real policy | Checked result |
|---|---|
| `sin(a*x+b)=c`, `cos(a*x+b)=c` | Complete union of at most two periodic families, or empty set outside the range |
| `tan(a*x+b)=c` | One complete periodic family, with original pole restriction |
| Reversed equation sides | Same theorem, preserving authored sides |
| RAD / DEG | Exact principal angles and respective full/half-turn periods |
| Stored rational A–F | Existing immutable value/context snapshot and invalidation |
| Unfamiliar rational target, e.g. `1/3` | Exact principal inverse, never a decimal substitute |

Here `a` is a nonzero exact rational, `c` is exact rational, and `b` is an
exact constant built from rational numbers and π by supported arithmetic.
Admission walks Giac nodes, proves the argument is affine, and replays its
exact reconstruction. It does not recognize pretty strings. The initial
boundary must already be an isolated single sine/cosine/tangent occurrence.

Complex policy, zero/unknown slope, non-affine argument, nested/multiple trig,
trig products, non-rational targets, inverse-trig equations, matching-function
equations, interval filtering and general inequalities remain unsupported.
The ordinary result remains available. No outer trig isolation, numerical
search or optional same-function theorem was added.

## Representation probe and vendor dependency

`tests/host/tutor_trig_probe.cpp` records authored VPAM `NodeFunction` trees,
explicit Giac serialization, parsed nodes, structured results, exact π and
ordinary/public solve forms in both modes. Inverse functions use existing
`FuncKind::ArcSin/ArcCos/ArcTan`; no parser or function-node type was added.

The initial probe exposed a link-order-dependent embedded-port defect:
owned `2π`/`π/2` constants were dynamically initialized from `cst_pi` in
another translation unit. A reproducer obtained `π/2=0` and
`acos(1/2)=-π/6`; the public DEG path also reached legacy alias-backed degree
conversion constants. The narrow correction initializes π from its identifier
and uses owned `gen`/`symbolic` storage for the two degree conversion values.
It changes no solving algorithm. See `lib/giac/NUMOS_CHANGES.md` and the retained
`representation-baseline-2.log` / `representation-vendor-fixed.log`.

Ordinary `solveStructured` returns finite representatives. The public Giac
entry with `all_trig_sol=true` can return parametric expressions using Giac's
own integer parameters. The current adapter has no independent typed binder
adaptation for those expressions. This phase therefore keeps ordinary Giac
answers independent, labels them **Giac representatives** for a complete
periodic trace, and explains the limitation on the final teaching page.
No tutor family is copied into the primary answer. The probe uses an isolated
context and restores its flag; production does not enable global verbose or
all-trig modes.

Reconciliation checks each returned finite representative for exact membership
in the proved union. A spurious representative is a reconciliation failure.
Successful finite membership leaves reconciliation `Unknown`: it cannot prove
equality with an infinite set. Tutor completeness comes from theorem replay.

## Model, kernel and rules

`PeriodicFamily` is separate from finite candidates: solve variable, current
left expression, exact offset/period, snapshot-scoped binder ID, typed
`AllIntegers` domain, mode, theorem step/branch and original-equation verdict.
There are at most two families; no nested union or root enumeration exists.
The display spelling `k` is never parsed back, assigned in VariableManager or
used as mathematical identity. A stored `k:=27` has no effect and is preserved.

The state fingerprint includes families, conditions, periodic stage and binder
identity. Old finite states retain their original fingerprint representation.
Replay JSON adds typed family fields only on the periodic path.

| Rule | Independent replay obligations |
|---|---|
| `trig.range` | Actual isolated function, exact target, real mode; sine/cosine range or tangent definedness |
| `trig.impossible` | Target strictly outside `[-1,1]`; no inverse call or invented candidate |
| `trig.principal` | Correct real principal inverse; exact named-angle simplification checked by range and forward identity |
| `trig.families` | Sine supplementary / cosine opposite / tangent single branch; exact full/half-turn period and integer domain |
| `family.shift` | Same checked constant subtracted from every relation; period unchanged |
| `family.divide` | Nonzero rational slope divides both offset and period; no division by one |
| `family.normalize` | Nonzero positive period via `k↦−k`; duplicate only when offset difference / common period is an exact integer |
| `family.finish` | Exhaustive theorem ancestry, solved variable, integer bijection and original definedness for every family |

The planner and checker are separate functions. The checker does not invoke the
planner or `solve()`. It reconstructs the expected state, checks exact operands,
relation, typed message, paths, prerequisites, conditions and binder metadata;
generator verdicts are not evidence. Every step enters the graph only after
replay. Arbitrary finite samples are never used as completeness evidence.

For a final `x=o+T*k`, the checker proves that `a*o+b` is the original theorem
angle and `a*T` is ± the theorem period. Thus every integer maps bijectively
onto its branch. For tangent, the principal inverse lies strictly between its
poles, and whole half-turns preserve definedness. `cos(a*x+b)≠0` remains a
typed `Nonzero` condition with authored tangent provenance.

RAD principal ranges are `asin: [-π/2,π/2]`, `acos: [0,π]`,
`atan: (-π/2,π/2)`. DEG is the exact conversion by `180/π`; the shared Giac
context is restored after each boundary. Non-elementary inverse values in DEG
are explicitly marked `rad` inside the conversion, so the header cannot imply
that an internal radian inverse was already evaluated in degrees.

## Actual teaching traces

`sin(2x)=1/2`, RAD:

1. Show the authored equation and `−1≤1/2≤1`.
2. Principal angle `α=π/6`.
3. Sine symmetry and full-turn periodicity:
   `2x=π/6+2πk` or `2x=5π/6+2πk`, `k∈ℤ`.
4. Display division of **each entire relation** by 2, followed by
   `x=π/12+πk` or `x=5π/12+πk`.
5. Complete family union, universal original check and ordinary-result scope.

`tan(3x)=1`, RAD: preserve `cos(3x)≠0`; principal `π/4`; derive
`3x=π/4+πk`; display division by 3; conclude `x=π/12+(π/3)k`, all integers.

`sin(x)=1/2`, DEG: principal 30; families `x=30+360k` or `x=150+360k`.
`sin(x)=2`: one concise terminal page, with the true failed bound `2>1`.
`cos(x)=−2` analogously shows `−2<−1`. No false chained inequality is displayed.

`sin(x)=0` deliberately retains `x=2πk` or `x=π+2πk`; it does not invent an
unimplemented union-compression proof. Sine/cosine endpoint duplicates are
removed by the exact integer-shift rule, retained in the checked graph.

All formulas are structured VPAM nodes. Four reusable canvases remain the
limit. Explicit balancing formulas expose the operation before its result.
English messages are complete; representative ES/FR templates and explicit
English fallback preserve the same graph. Pseudolocale, scrolling and VAR pan
are exercised. STIX `α`, `∈`, `ℤ` coverage passes at 18/12/8 px; no font subset,
parenthesis assembly, renderer geometry or input-key behavior was changed.
The integer-membership display node supplies relation spacing locally.

Exact 320×240 frames and complete contact sheets are in
`out/tutor-engine-02c/ui-final/`, with the corrected range/pan frames in
`ui-pan-resumed/`. The author inspected full sequences, including scrolled
conditions, affine operations, both families and unfamiliar DEG inverses.
No independent human/agent mathematical or teaching approval is claimed.

## Reproducible gates and evidence

Use the pinned native build with:

```text
python scripts/test-tutor-transcendental-host.py --source SNAPSHOT --build BUILD/emulator_pc --out OUT/host --trig
python scripts/test-tutor-trig.py --bin OUT/host/tutor_engine_main.exe --out OUT/seeded
python scripts/test-tutor-trig.py --bin OUT/host/tutor_engine_main.exe --out OUT/challenge --challenge tests/fixtures/tutor-trig-challenge.json
python scripts/test-tutor-teaching-ui.py --bin EMULATOR --out OUT/ui --cases trig-sine trig-affine trig-impossible trig-cosine trig-tangent trig-tangent-affine trig-sine-deg trig-cosine-deg trig-tangent-deg trig-affine-deg trig-unfamiliar-deg
python scripts/test-tutor-nonlinear-lifecycle.py --bin FIXED_POOL_EMULATOR --out OUT/pool50 --trig
node scripts/tutor-teaching-web.mjs SNAPSHOT --nonlinear --transcendental --trig --browser=chromium
```

The existing page profiler and benchmark entry points accept the new trig
fixtures. The existing allocation overlay exercises all new formula pages,
including persistent faults and later healthy recovery without resetting Giac.
Exact commands, object hashes, stdout/stderr and first failures are retained
under the ignored output paths. No goldens or masks were changed.

## Resource and regression ledger

The source was paused on September 17 and resumed September 19. All recorded
source hashes, HEAD, the empty index and the unrelated editor hash matched
before resuming. Runtime source fingerprint:
`d4c83f32543c0f2f69d577efe83c32652d65efbe9d4a247b28bf747da626dd20`.
The final full path/hash manifest and readable binary patches are in
`out/tutor-engine-02c/final-source.json`; documentation is included in its full
fingerprint separately from the runtime fingerprint.

Pinned build tools: PlatformIO 6.1.19, Espressif32 6.12.0, Arduino 2.0.17 /
IDF 4.4.7, Xtensa GCC 8.4.0+2021r2-patch5; native GCC 15.2, SDL2; Emscripten
6.0.3; LVGL 9.5. The resume log records that another installed PIO Core had
used 6.2.0 during the pause. Invoking the original 6.1.19 executable restored
its required SCons 4.8.1 package automatically; firmware compiler/framework
versions and project dependencies stayed as above. No toolchain upgrade or
production build flag change was made for this feature.

### Firmware and bounded payload

| Ordinary WROOM measurement (bytes) | Immediate 02B baseline | 02C | Delta |
|---|---:|---:|---:|
| Linked flash | 5,552,209 | 5,581,493 | +29,284 |
| Firmware image | 5,552,576 | 5,581,856 | +29,280 |
| Static RAM | 118,984 | 119,016 | +32 |
| IRAM text | 60,407 | 60,407 | 0 |

Ordinary production image SHA-256:
`6ef7e3ccdc50d6f96b3fafd9ba565c749dac0b831bc253bff69a7bf606caf8f7`.
This image was built, not flashed. Section sizes and identity are recorded in
`production-identity.json`.

| Measured / enforced item | Result |
|---|---|
| PeriodicFamily object | 112 B Xtensa; 144 B native (including string/vector headers, excluding owned text) |
| Xtensa State object | 48 B |
| New 197-case corpus maximum retained trace estimate | 13,516 B |
| Maximum tracked construction vector heap | 5,952 B |
| Maximum new-corpus symbolic calls | 399 |
| Family union / step limits | 2 / existing 48 |
| Existing retained / vector / symbolic-call budgets | 64 KiB / 128 KiB / 4,096, unchanged |
| Healthy presentation maximum live AST nodes | 122 |
| C++ requested allocation peak / AST node peak | 3,120 B / 7,552 B (separate maxima) |
| Measured simultaneous combined presentation extra | 9,080 B |
| LVGL configured pool | 64 KiB, unchanged |

The old corpus maximum retained estimate increases from 30,043 to 30,811 B
because every State now owns a family-vector header; its mathematical graph
is unchanged. These are accounted C++ payload/vector measurements, not total
allocator or Giac heap peaks. Giac malloc, allocator metadata and LVGL are not
included in the presentation payload figures. Independent maxima are not added.

Disassembled own Xtensa entry frames: admission 128 B, principal-angle helper
208 B, normalization 112 B, checker 640 B, planner 768 B, formula build 368 B,
Steps publication 928 B, tutorFormula 592 B (`own-frames.json`). These are
individual frames, not a cumulative call-chain bound or FreeRTOS high-water
measurement. No fresh internal-heap/PSRAM/task-stack board claim is made.

The final fixed-pool binary passed 50 mixed cycles after resume, without Giac
reset between cases. All 300 sampled states are in `pool50-resumed/result.json`.
Minimum sampled free payload was 7,208 B; each HOME restored 38,312 B used
payload, 67 objects, 3 timers and zero retained engine handles. HOME total/free
pairs were 57,472/19,160 or 57,480/19,168 B: reversible 8 B TLSF metadata
coalescence, with identical live payload and no decreasing free payload.
Sampled free memory does not measure the true allocation peak. Native `heap=0`
is an unavailable telemetry field, not evidence of zero process heap usage.

### Host performance (not ESP32 timings)

Function timings below use microseconds. Each warmed final-page workload has
30 retained samples after five warmups; no slow samples were removed. These
measure preparation/dispatch, not LCD first-pixel latency. Concurrent builds
make host wall-clock values diagnostic; operation counts are the hard gate.

| Exact workload | First page | First preparation of final page | Warm final reopen median / p95 / max |
|---|---:|---:|---:|
| `sin(x)=1/2`, RAD | 737.3 | 615.0 | 950.6 / 1,105.0 / 1,349.8 |
| `sin(2x)=1/2`, RAD | 749.3 | 705.8 | 901.2 / 1,109.6 / 1,136.9 |
| `tan(3x)=1`, RAD | 689.0 | 570.0 | 865.7 / 1,091.7 / 1,157.5 |
| `sin(3x-1)=1/3`, RAD | 796.3 | 1,202.8 | 1,136.35 / 1,571.7 / 1,757.1 |
| Existing general quadratic final | separate workload | separate workload | 857.3 / 1,164.2 / 1,188.4 |
| Existing complex quadratic final | separate workload | separate workload | 789.25 / 1,064.8 / 1,261.4 |

| Equation | First ordinary / tutor generation | Warm ordinary / tutor median |
|---|---:|---:|
| `sin(x)=1/2` | 432 / 3,132 | 322 / 2,664.5 |
| `sin(2x)=1/2` | 257 / 2,918 | 355 / 2,695.5 |
| `cos(x)=1/2` | 211 / 3,188 | 343 / 2,759.5 |
| `tan(3x)=1` | 408 / 1,980 | 432 / 2,529.5 |
| `sin(3x-1)=1/3` | 338 / 5,461 | 464 / 6,710.5 |

The second table also uses 30 steady samples after five warmups. A fixture's
first use is not a fresh process or cold Giac reset. Raw timings and nested
scopes are retained in `tutor_transcendental_timing-final-run.log`,
`page-benchmark/` and `first-pages.json`. Nested times are not summed.
`counter-invariance.json` compares 540 existing quadratic, complex and log
workload rows: no changed operation counts, one proof build per valid session,
and zero redundant polynomial captures on quadratic/complex pages. Drawing and
layout do not invoke Giac; no new page cache or per-frame allocation was added.

### Regression ledger

All paths below are relative to `out/tutor-engine-02c/`, except the isolated
host suite at `C:/.codex-cache/tutor-engine-02c/host-final`.

| Gate | Final result and evidence |
|---|---|
| Historical / 02A / 02B / 02C corpus | PASS: 180 / 151 / 233 / 197; `host-final.json`; 02C has 180 seeded + 17 separate challenge cases |
| Exact old trace invariance | PASS: 563 unchanged graphs and call counts; `invariance-final.json`. Former unsupported `sin(x)=0` is the one intended coverage upgrade |
| Trig independent checker / mutations | PASS: 861 checks, 166 rejecting mutations; `tutor_trig_checks-final-run.log` |
| Old nonlinear / transcendental mutations | PASS: 235 checks / 23 mutations and 386 checks / 25 mutations; isolated host logs |
| Old kernel / teaching / composite guards | PASS: host historical mutations and teaching executable; no validation bypass |
| Persistent planner allocation failures | PASS: 880 injections, no false Complete, tracked vectors restored, later healthy proof without reset; `tutor_nonlinear_allocation-final-run.log` |
| Page allocation failures | PASS: 2,028 sampled persistent/one-shot boundaries; `allocation-summary-final.json`; original answer and recovery retained |
| Guided / summary / provenance / locales | PASS: `ui-final.json`, final corrected range/pan `ui-pan-resumed.json`; exact 320x240 full scroll frames |
| Angle change / stale proof / binder collision | PASS: RAD-to-DEG invalidation and regeneration only on Solve; stored `k=27` untouched; host mutations and `ui-final/angle-change-reopen.log` |
| Notation / MathEnginePhaseRegression / STIX / plus-minus / Delta / integer glyphs | PASS: `notation.json`, `stix.json`, `plus-minus.json`; 18/12/8 px coverage; no font regeneration |
| Equations / Calculus-F7 / Grapher Templates | PASS: focused 5 / 20 / 20 cycles; `equations.json`, `calculus.json`, `grapher.json` |
| Ordinary Giac / Calculus / Neo / cross-app | PASS: 179 / 56 / 44 / 14; official consistent-profile `host-standard.log` |
| Fixed 64 KiB pool lifecycle | PASS: 50 final mixed cycles, `pool50-resumed.json` |
| Native and ordinary WROOM | PASS: `native-range-final.json`, `production-range-final.json` |
| WROOM bring-up / demo, CAM validation | PASS: `firmware-matrix.json`, exact commands/log retained |
| CAM normal final display correction | PASS: `cam-resumed.json`; interrupted first link retained separately |
| Chromium / Firefox / WebKit teaching and smoke | PASS: `wasm/settings-enter/*.json`, final range-corrected source, Settings DEG path |
| WASM-MATH Release / Debug | PASS: release and sequential debug rerun; initial port failure retained |
| Existing application screenshots | 42 baseline/candidate bodies unchanged below header; 18 historical golden mismatches remain (`golden-ledger.json`), masks unchanged |
| Whitespace / index / unrelated editor | Final `git diff --check`; index empty and editor SHA checked by `final-source.py` |
| Physical LCD / board memory or latency | NOT RUN in this implementation phase |

The new mutations explicitly corrupt periods, supplementary/opposite branches,
principal values, range acceptance, tangent poles, affine offset/period division,
normalization signs, mode, integer domain, binder scope, theorem provenance,
message operands, family union and completion. They also reject stale epochs,
engine generations and false finite-sample proofs. Tests detect these bounded
failures; neither the corpus nor spot checks constitute a universal theorem.

First failures remain available: initial vendor constant/lifetime probe;
invalid restriction caption; false impossible-range chained inequality; strict
pool total/free equality before accounting for reversible metadata; a mixed
native/host-object diagnostic link/profile failure (replaced with the official
consistent-profile harness, not weakened expectations); Debug WASM port clash;
wrong Settings EXE alias in the browser test (corrected to Enter); and the first
pan assertion before scrolling to the wide formula. The initial physical-key
negative argument fixture failed during authoring before tutor admission;
`cos(0-2x)=1` exercises the negative-slope theorem without changing the editor.
The interrupted CAM link on September 17 is retained separately. No first
failure was suppressed or labeled PASS, and no golden was promoted.

### Exact changed-file boundary

The following 32 files form this uncommitted candidate. Generated screenshots,
logs, binaries, backups and source snapshots stay under ignored output paths.
Unrelated `.vscode/settings.json` is excluded and unchanged.

```text
docs/TUTOR_ENGINE_02C.md
lib/giac/NUMOS_CHANGES.md
lib/giac/src/kusual.cc
scripts/benchmark-tutor-pages.py
scripts/test-equations-rebuild.py
scripts/test-tutor-engine.py
scripts/test-tutor-nonlinear-lifecycle.py
scripts/test-tutor-teaching-ui.py
scripts/test-tutor-transcendental-host.py
scripts/test-tutor-transcendental-ui-failure.py
scripts/test-tutor-trig.py
scripts/tutor-teaching-web.mjs
src/apps/EquationsApp.cpp
src/apps/TutorPresentation.h
src/apps/TutorPresentation.inc
src/apps/TutorStepsView.inc
src/math/GeneratedMathNotation.h
src/math/giac/GiacTutor.inc
src/math/giac/GiacTutorTranscendental.inc
src/math/giac/GiacTutorTrig.inc
src/math/giac/GiacTutorTrigPlanner.inc
src/math/tutor/Derivation.h
src/math/tutor/Messages.inc
src/math/tutor/TeachingPlan.h
tests/fixtures/tutor-trig-challenge.json
tests/host/math_notation_main.cpp
tests/host/tutor_engine_main.cpp
tests/host/tutor_nonlinear_allocation.cpp
tests/host/tutor_teaching_math.cpp
tests/host/tutor_transcendental_timing.cpp
tests/host/tutor_trig_checks.cpp
tests/host/tutor_trig_probe.cpp
```


## Limitations and next boundary

The main remaining product mismatch is Giac's finite representative adapter
versus the tutor's complete periodic theorem. A future adapter must preserve
Giac-origin integer binders independently before claiming primary periodic
answers. Matching-function equations, irrational targets, general trig
isolation and interval restrictions need their own admission/theorems; none
is inferred from the present corpus.

A later optional production check should inspect sine and tangent in RAD,
sine in DEG, an unfamiliar inverse with pan, RAD→DEG invalidation, BACK/HOME
and repeated reopening. This implementation makes no fresh board heap, stack,
LCD-latency or manual visual claim and leaves the connected PCB unchanged.

Proposed future boundary: the two theorem/planner includes, typed periodic
model and projection/messages, the narrowly required Giac constant fix with
provenance, focused regression entry points/fixtures and this report.
Suggested subject: `feat(tutor): explain trigonometric periodic families`.

## Physical closeout — 2026-09-19

This section supersedes only the implementation-phase pending-board status above.
Historical host evidence is not relabeled as physical evidence. The exact 32-path
boundary listed above remains the feature commit boundary; private probes,
images, logs and backups are excluded.

### Preserved baseline and accepted identities

Repository: `C:/Users/Juan Ramón/Documents/Calculadora`, `main`.
Starting HEAD `fb8a2b329c3cf14547a7ba4cbc46e4125211edb6` legitimately adds the
editor ignore rule above accepted 02B `8b10bfbefbb2df558f471052990b65e76fc93e02`.
The empty index, binary-capable patches, readable 32-file archive and path/hash
manifest were preserved under `out/tutor-engine-02c-closeout/`. No reset, stash,
clean or history rewrite was used. Unrelated `.vscode/settings.json` remains
byte-identical (SHA-256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`).

The initial ordinary rebuild exactly reproduced image
`6ef7e3ccdc50d6f96b3fafd9ba565c749dac0b831bc253bff69a7bf606caf8f7`
and runtime fingerprint
`d4c83f32543c0f2f69d577efe83c32652d65efbe9d4a247b28bf747da626dd20`.
After the narrow correction below, final runtime fingerprint is
`58425d222bb1a8ea370ea0c72a6b766f972524bacace4c5b5c4ba8b6d6a10346`.
Documentation appended after testing does not change runtime identity.

| Artifact | SHA-256 | Image bytes |
| --- | --- | ---: |
| Final ordinary WROOM, installed and read back | `f93beb9b21345690fb6224b1c5fcbca73458f5938c9bd5cce94db3de71a2d211` | 5,582,448 |
| Temporary measurement probe, removed | `c17b898d89ab2015200e359029ce5f443ef07b73802ddad81cd1fd959e7b2217` | 5,586,928 |

Final ordinary linked flash **5,582,089 B**, static RAM **119,016 B**, IRAM text
**60,407 B**: +596 B linked flash, +592 B image, unchanged static RAM/IRAM versus
the immediate candidate. This does not imply zero dynamic cost. Production
WROOM configuration, display clocks and fixed 64 KiB LVGL pool are unchanged.
Pinned PlatformIO 6.1.19, SCons 4.8.1, espressif32 6.12.0, Arduino 2.0.17 /
IDF 4.4.7 and Xtensa GCC 8.4.0 (esp-2021r2-patch5) were used. A concurrent VS Code
installation replaced shared SCons; failed imports are retained. Final builds
used a private copy of the same pinned SCons with unchanged compiler/framework
paths. Exact commands and build outputs are in `admission-fix/accepted/`.

### Correction and independent checks

A closeout challenge, `sin(x)=0*x/x`, exposed a tutor admission defect: destructive
normalization admitted a constant target and a family containing undefined
`x=0`. The independent ordinary answer retained `pi`. `GiacTutorTrig.inc` now
checks authored constant-target and affine-argument structure before binding,
then rechecks the trig node after binding. Domain-bearing variable targets are
outside this constant-target theorem and are refused honestly. Supported stored
rational targets remain accepted. No exclusion-family solver was added.

Only that implementation file and `tests/host/tutor_trig_checks.cpp` changed
during closeout, plus this report. Preserved old host and board reproducers fail
the detecting gate; final host and board reject the unsupported domain-bearing
form while retaining the ordinary answer. Intermediate fixes first missed a
stored-zero cancellation, then needed the post-binding node guard for undefined
arguments. Both failures and detecting tests remain recorded; no intermediate
image is presented as final acceptance.

The vendored fix was audited separately: five pi-derived/conversion constants
change ownership/initialization only, with no solve-algorithm change. The old
probe has `two=0`, `half=0`, incorrect `acos(1/2)` and a failed DEG path. The fixed
public probe and adapter give correct RAD/DEG values. Final hardware principal
states contain `pi/6`, `pi/3`, 30 and 60, independently of final family display.
Provenance remains in `lib/giac/NUMOS_CHANGES.md`.

Final replay compared **761 traces**: 180 historical, 151 02A, 233 02B and 197
02C. Thus the actual old corpus is 564, not the request's historical count of
563. States, steps, snapshots, validity, completeness, candidates and
reconciliation have **zero mathematical differences**. Admission call counters
changed in 93 traces (-1 to +20 calls); these are recorded separately, not
ignored proof fields. Final trig checker: **889 checks / 166 mutations**;
historical math mutations: 248. **880 C++ allocation-failure points** recovered;
this does not cover every Giac/malloc failure.

### Automated physical scope

User authorization was recorded once. Rediscovered device: **COM9**, USB
303A:1001, serial/MAC **44:B1:76:A7:B7:2C**. Active OTA application: subtype 16,
offset `0x10000`, capacity `0x640000`. Existing application recovery bytes,
security readings and boot/partition prefix were preserved before writing.
Every write used the reviewed application-only route and independent
verification. Final ordinary firmware was rebuilt byte-identically, written,
fully read back and SHA-256 verified, then booted normally. Prefix/partition
bytes were unchanged. Boot reported PSRAM and the production SAFE display profile.

Fifteen canonical production-input cases passed: RAD sine 1/2, affine sine
`sin(2x)=1/2`, unfamiliar `sin(3x-1)=1/3`, impossible sine 2, cosine 1/2,
cosine endpoint 1, tangent 1, affine tangent `tan(3x)=1`; old quadratic, radical
and logarithm; DEG sine 1/2, tangent 1, affine sine 1/2 and cosine 1/2.
Exact principal states, both offsets and periods, endpoint normalization,
integer-binder metadata, authored pole condition and completeness replay were
compared against independently checked host traces. No roots were enumerated.
Ordinary Giac finite representatives passed exact family-membership checks;
this does **not** establish equality with the infinite family union. The
ordinary-results/complete-Steps product mismatch remains a follow-up.

TOOLBOX, LEFT/RIGHT, all guided pages and vertical extents, summary/guided,
wide-formula VAR pan, BACK/reopen, edit/cancel and HOME/re-entry were exercised.
Valid reopenings retained one proof build. Committed edits cleared the old proof.
Real Settings RAD→DEG→RAD passes through HOME and destroys app ownership; new
entries build once in the new mode. A separate private retained-session angle
probe verified stale formula suppression and rebuild counts 1→2→3. The first
edit harness accidentally authored `0*sin(x)=1/2`; its correct Unsupported
result and failed harness log are retained. Re-authoring a supported affine
fixture passed; no production change was needed.

The product exposes stored A–F, not a physical user-k entry. Binder collision
uses the existing host-backed `k:=27` fixture, not an invented physical variable
feature; hardware verifies binder identity within valid snapshots and no
family/binder retention after HOME. **Human LCD review deferred.** Emulator
screenshots were inspected, but telemetry/geometry is not human LCD approval.

### Instrumented board timing

Milliseconds; first-open and first-final preparation are different workloads.
Cached final reopen has **n=3** per fixture. Scopes are nested function timings,
not LCD first-pixel measurements, and must not be added together. No Giac reset
between cases or cached samples.

| Fixture | Ordinary | Tutor | First open | First final prep | Cached median | Cached max |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| sine | 45.701 | 450.932 | 45.830 | 46.876 | 72.288 | 72.307 |
| sine-affine | 52.815 | 516.704 | 45.617 | 46.881 | 70.988 | 71.006 |
| sine-unfamiliar | 56.415 | 1015.706 | 45.806 | 59.256 | 84.242 | 84.355 |
| sine-impossible | 11.544 | 34.530 | 44.970 | 28.089 | 44.894 | 44.903 |
| cosine | 43.357 | 452.713 | 45.617 | 46.874 | 72.310 | 72.388 |
| cosine-endpoint | 15.050 | 177.765 | 42.729 | 24.764 | 50.188 | 50.246 |
| tangent | 49.703 | 252.099 | 40.836 | 32.926 | 57.962 | 57.969 |
| tangent-affine | 54.040 | 369.498 | 41.021 | 35.807 | 60.846 | 60.931 |
| old-quadratic | 48.523 | 463.903 | 60.196 | 33.827 | 58.065 | 58.161 |
| old-radical | 35.858 | 1276.334 | 39.880 | 17.517 | 42.456 | 42.501 |
| old-logarithm | 23.479 | 362.613 | 39.708 | 18.365 | 43.824 | 43.856 |
| sine-deg | 158.883 | 234.177 | 45.461 | 38.702 | 65.071 | 65.108 |
| tangent-deg | 164.672 | 212.648 | 40.793 | 29.963 | 55.298 | 55.320 |
| sine-affine-deg | 169.938 | 267.184 | 45.453 | 39.705 | 65.170 | 65.192 |
| cosine-deg | 181.199 | 249.421 | 45.448 | 39.005 | 65.351 | 65.423 |

The old quadratic retains one proof build and zero redundant polynomial capture
in the focused page-counter regression. The unfamiliar inverse is slower to
construct; no performance project or new cache was introduced.

### Memory, lifecycle and limitations

All values below are bytes. Initial probe HOME: internal free/largest
144,560 / 102,388; PSRAM free/largest 8,227,843 / 8,126,452. LVGL monitored
payload/free/largest 61,684 / 41,252 / 40,168; stack minimum-unused 60,028.
After fifteen mixed cases and twelve additional unfamiliar-inverse cycles:
internal 144,024 / 102,388; PSRAM 8,224,459 / 8,126,452; LVGL monitored
payload/free/largest 61,500 / 40,800 / 33,032, max-used 28,992;
**71 objects / 5 screens / 3 timers**, no retained tutor trace/families.
Stack minimum-unused **51,792 bytes**. The pinned IDF header specifies bytes
and its stack element is uint8_t; no factor-of-four conversion was used.

Warm-up retains 536 internal bytes and 3,384 PSRAM bytes relative to the initial
sample; ownership is not attributed from free counters. Subsequent post-HOME
samples show bounded small variations, not sustained decline. Both strict
six-repeat equality tests FAILED on the same reversible 4-byte PSRAM dip; those
exit records and assertions remain unchanged. Each series recovered completely
for its final three samples. The separate `stability-assessment.json` accepts
the requested no-sustained-decline criterion, **not** exact sample equality.
Largest blocks and object/screen/timer counts are fixed across these repeats.
This is twelve repeated cycles without Giac reset, not a claim of zero leaks
under every workload. No crash or unexpected reset occurred on final source.

The fifteen-case boundary-sampled minima were internal free 144,024, PSRAM free
8,209,551 and LVGL free 33,692. These are neither simultaneous nor total
allocation peaks. Lifetime low counters include unsampled work; do not sum
independent minima. The fixed-pool native 50-cycle run passed with sampled free
minimum 7,208 B and stable post-HOME 67 objects / 3 timers; native and board
object totals are different configurations, not a shared baseline.

### Final regression and restoration

Final-source host/checker/corpus, notation/STIX/±/Δ, teaching/provenance/locales,
angle/binder guards, Equations, allocation recovery, fixed-pool50 and page
counters PASS. Native, production WROOM and CAM normal builds PASS. Web
build/package/smoke and teaching in Chromium/Firefox/WebKit PASS; WASM-MATH
Release PASS. Debug initially failed with port 8793 already owned by the
simultaneous Release run; the same Debug package passed in an isolated rerun.
The ordinary-image smoke initially missed GUI focus events arriving between
key acknowledgement and the subsequent barrier; its private runner now collects
that full interval. Original logs remain, and unchanged firmware passed the
corrected smoke. The first failed WASM log is retained. The accepted software ledger records this
as PASS_ON_ISOLATED_RERUN, not an uninterrupted pass.

Earlier full implementation build/app matrices remain historical evidence;
unaffected bring-up/demo/CAM-validation and other-app replays were not all
repeated during closeout. No golden, mask, dependency or compiler setting was
changed to obtain a pass. Screenshots, raw serial logs, exact commands, samples
and hashes remain under ignored output paths, especially `physical-accepted/`,
`angle-accepted-retry/`, `admission-fix/accepted/` and `board/ordinary-final/`.

Final ordinary non-instrumented smoke exercised RAD sine, DEG sine, RAD tangent,
a linear equation, Steps controls and HOME. This smoke establishes canonical
ordinary dispatch/responsiveness; internal proof semantics were observed on the
matching-runtime probe. The board is left **at HOME in RAD**, with no measurement
overlay installed. No direct NVS/LittleFS erase, format, provisioning, eFuse,
security, partition or bootloader write occurred; ordinary Settings angle-mode
persistence is distinct from those prohibited operations. No push occurred.

The single local feature boundary is
`feat(tutor): explain trigonometric periodic families`, containing exactly the
32 paths listed above. Commit/tree receipts and final status are recorded in
the ignored closeout output after committing. No next tutor phase is started.
