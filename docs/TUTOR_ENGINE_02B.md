# TUTOR-ENGINE-02B — real exponential and logarithmic equations

The implementation-phase evidence below predates physical closeout. That phase did not authorize commits or flashing. The appended closeout records separately authorized application-only board testing and the local closeout commit.

## Baseline and ownership

The immediate baseline is `main` at
`57cbb0b585d70539f9e38650055f9e3725066589`. The index was empty; the only
untracked file was `.vscode/settings.json`, preserved separately and untouched.
The baseline manifest is `out/tutor-engine-02b/baseline.json`:

- Full tracked-source fingerprint: `775eb50a72f1f9cbc30a8a87acfaac25107d761c1802be9684a4e4b9269cccc8`.
- Runtime fingerprint: `d348d09690845706b1cd2a9f7b5587a60b0ec20138a02f0e5ad70bcb8ebf8658`.
- Separate index/working patches, readable unrelated-file copies and an ASCII
  baseline snapshot were retained before editing. Evidence remains ignored.

All new mathematics stays inside the existing Giac boundary and context. The
planner constructs a derivation before candidate reconciliation; it never uses
ordinary roots to select or construct its steps. Giac retains ordinary-answer
authority. No vendor implementation, persisted expression, editor, clock,
allocator setting, font or display profile changes.

## Representations and supported methods

The host probe `tests/host/tutor_transcendental_probe.cpp` inspects actual authored
nodes, serialization, vendor parsed nodes and structured results. Its baseline
output is `out/tutor-engine-02b/representation-baseline.log`.

| Authored form | Existing serialization / vendor representation | Checked method |
|---|---|---|
| Euler constant power | `((exp(1))^(x))`; vendor parser produces `at_exp(x)` | Natural exponential |
| `exp(x)` or machine `e^x` | `at_exp(x)` | Same natural exponential theorem |
| Fixed rational base power | `at_pow([base, exponent])` | Valid positive base different from one |
| LN key | `ln((x))`; `at_ln` | Natural logarithm |
| Machine `log(x)` | Vendor alias of `ln(x)` | Natural logarithm; **not** the LOG key |
| LOG key | `log10((x))`; raw `at_log10` | Base-ten logarithm |
| Existing explicit-base node | `logb((x),(2))`; raw argument-first `at_logb([x,2])` | Explicit rational base logarithm |

Arguments/exponents must be affine over exact rational coefficients after
expansion. The outside coefficient is a nonzero rational; additive isolation,
division and side swapping reuse independently checked existing primitives.
The inverse target must be a rational constant. An isolated equality of two
matching functions permits affine expressions on both sides. Referenced stored
A–F rational values use the existing snapshot/substitution contract.

Unsupported: complex-policy exp/log, unresolved or nonpositive bases, base one,
multiple independent terms on a side, nested functions (including inverse
compositions in this release), variable targets requiring transcendental child
algebra, non-affine arguments, variable denominators in arguments, inequalities,
Lambert W, numerical inversion and conditional infinite families. Refusal leaves
the ordinary result usable. These limits describe the tutor, not mathematical
solvability.

The inherited authored-tree budget also bounds closed integer powers (normally
exponents −2 through 4); use an exact rational target for larger recognized
powers. The separate exact-power recognizer is bounded by the limits below.

## Rules and independent checking

`GiacTutorTranscendental.inc` handles bounded structural admission; the planner
and theorem replay live in separate `.inc` files. Original authored structure is
checked before normalizing: a canceled variable denominator cannot erase a pole.
An opaque subtree protects the function during rational isolation, preventing
normalization from changing its base or exponent while balancing.

| Stable rule | Prerequisites and replay |
|---|---|
| `domain.positive` | Reconstruct the authored logarithm argument and its exact source provenance; introduce its `Positive` condition. |
| `domain.base` | Reconstruct an exact rational base; independently require positive and unequal to one; retain both typed facts. |
| `exponential.range` | Verify the isolated exponential/power and exact other side; introduce target positivity. |
| `terminal.positive_range` | Valid real exponential base and exact nonpositive rational target imply an empty set, without invented candidates. |
| `exponential.exact_power` | Bounded rational exponent `p/q`, `-16≤p≤16`, `1≤q≤8`; verify `a^p=c^q` exactly and positive `a,c`. Uniqueness of the positive qth root proves the rewrite. |
| `exponential.injective` | Actual matching real exponential nodes, equal valid bases and exact exponents; replay `u=v`. |
| `exponential.inverse` | Valid base, exact positive target and preserved range condition; replay `u=ln(c)` or `u=ln(c)/ln(a)`. |
| `logarithm.inverse` | Actual logarithm, exact argument/base/RHS and argument positivity; replay `u=e^v` or `u=a^v`. |
| `logarithm.injective` | Same valid base and **both** positive arguments; replay `u=v`. |

New inverse/injectivity transformations are equivalent under their retained
conditions, not candidate-producing squaring. Existing rational AddBoth and
DivideBoth replay both sides; symbolic constants stay exact on the right.
Every finite candidate still passes `candidate.original_valid` or an explicit
rejection, followed by terminal completeness and ordinary-answer reconciliation.
Completeness follows from the valid inverse/injective theorem and complete
affine child algebra, not from agreement alone.

Exact signs in new condition roles use rational signs and bounded positivity
theorems for exponentials, positive-base rational powers, reciprocals and natural
logs of rational constants. Undecidable signs refuse the method. No floating
point exponent recognition or numerical sampling is used as a proof.

The condition model uses `Positive` and a new `NotOne` kind. Roles identify log
arguments, exponential/logarithmic bases and exponential targets. Conditions
retain structured values, authored paths, source states and checker verdicts.
`NotOne` is never treated as evidence of nonzero in denominator checking.

Replay now rejects a removed legacy domain/range condition before message
selection reads `conditions.back()`. This narrow guard fixes an out-of-bounds
access exposed by running existing mutation tests with checked C++ containers;
it does not relax mathematical verification.

## Teaching and exact notation

All English templates are in the existing validated catalog, with representative
Spanish/French entries and explicit English fallback. Mathematical parameters
remain checked operands/auxiliaries and structured formulas. No localized text
is parsed. Guided/summary and pseudolocale reuse the identical derivation.

The generated converter preserves function/power structure for both equation
sides: a common-base page must show `2^x=2^3`, not evaluate the right side back
to 8. `logb` maps to the existing base-log node. Natural products, Δ, ± and
authentic STIX parentheses are unchanged. The two base-admissibility primitives
share one teaching page while retaining both step references. Restrictions
appear on their domain/range pages and the conclusion, rather than every page.

The existing scalar-notation classifier now recognizes an Euler **constant**
power as a scalar symbol factor: `2eˣ`. A user identifier named `e` is not inferred
to be the Euler constant; repeated Euler factors keep explicit multiplication.
The added fixtures check canonical serialization and geometry, including the
existing `3ln(x)` spacing. Layout and rendering code are unchanged.

The browser logical-key boundary now resolves Equations' modified LN through
the same production semantic resolver already used there for modified SQRT.
Without this narrow correction SHIFT+LN inserted a logarithm in web, whereas
the canonical physical native event correctly inserted an Euler power. This
change is under `__EMSCRIPTEN__`; it does not change the PCB key map.

Representative complete traces (each arrow is independently checked):

- `e^(2x−1)=e³` → `2x−1=3` → `2x=4` → `x=2` → original check → complete.
- `2^x=8`: `2>0`, `2≠1`, `8>0`; `2^x=2³` → `x=3` → original check → complete.
- `4^x=2`: same base prerequisites; exact exponent `1/2` → `x=1/2` → original check. Giac's exact square-root alias is recognized structurally.
- `e^x=−1`: positive exponential range directly implies no real solution.
- `ln(x−1)=2`: retain `x−1>0`; `x−1=e²` → `x=e²+1` → domain/original check → complete.
- `3ln(x)−6=0`: retain `x>0`; add 6 → `3ln(x)=6`; divide by 3 → `ln(x)=2`; invert → `x=e²`; check → complete.
- `ln(x)=ln(5)`: retain `x>0` and `5>0`; injectivity → `x=5`; check → complete.
- `ln(x−1)=ln(1−x)`: retain both domains; affine candidate `x=1` fails positivity and is rejected; no solution.
- `log_2(x)=3`: `2>0`, `2≠1`, `x>0`; invert → `x=8`; check → complete.

## Reproduction and evidence

Build the pinned `emulator_pc` source snapshot first. Then:

```text
python scripts/test-tutor-transcendental-host.py --source <snapshot> --build <emulator_pc objects> --out <ignored ASCII directory> --compiler <pinned g++>
python scripts/test-tutor-teaching-ui.py --bin <emulator> --out <ignored directory>
python scripts/test-tutor-nonlinear-lifecycle.py --transcendental --bin <fixed-pool emulator> --out <ignored directory>
```

The host entry point records commands/object hashes and runs historical/new
corpora, independent replay, mutations, allocation recovery and conclusion
guards. Separate challenge JSON never participates in method selection. UI fault
tests require the existing isolated allocation probe, not an ordinary emulator.
The page profiler remains a default-off source overlay; nested timings must not
be summed. No instrumentation is enabled in ordinary firmware.

The measurement/regression ledger and exact changed-file manifest are
recorded with this candidate under `out/tutor-engine-02b/`. First failures and corrected reruns are
retained, including the container assertion, the old ln-refusal expectation,
the hidden exact-power display and browser ordinary exact-spelling differences.

The allocation overlay can be reproduced with
`scripts/build-tutor-ui-allocation-probe.py --source ... --build ... --database <native compile database> --out ... --scratch ...`;
then run `scripts/test-tutor-transcendental-ui-failure.py --bin <probe> --case logarithm --quick`
(also `logbase`, `exponential`). The database must be for **native**, not the
production Xtensa environment. Failed attempts with the wrong database and a
native link attempted while the previous executable was open remain in the
ignored evidence; neither is recorded as a passing build.

## Checked and visual results

| Gate | Result / evidence |
|---|---|
| Historical corpus | 180/180; two previous ln/exp refusals intentionally become supported. |
| 02A corpus | 140 seeded + 11 challenge, all pass. |
| 02B corpus | 208 seeded + 25 separate challenge, all pass; challenge includes 8 explicit refusals and both-domain explicit-base injectivity. |
| Exact old trace invariance | 329 unchanged graphs, including states, steps, snapshots, conditions, verdicts, reconciliation and symbolic-call counts; `invariance-final.json`. |
| Independent checker | 319 new checks / 19 rejecting mutations; 235 02A checks / 23 mutations; 248 historical mutations; 35 composite-conclusion guard mutations. Epoch/generation, stored-value and context guards pass. |
| Allocation | 560 planner C++ failure points; 96 selected page failures, including persistent faults, successful later preparation and ordinary-answer availability. |
| Notation / typesetting | 33 structured cases, base-log argument/base round-trip, MathEnginePhaseRegression, STIX parentheses, ± and existing Δ coverage pass. |
| Native teaching | 67 sequences, 359 pages / 745 scroll frames, guided/summary, ES/FR/pseudo and wide content; `native-complete/teaching`, with final new-method replay in `teaching-final`. |
| Equations / adjacent apps | Equations including 50 editing cycles; Calculus/F7; Grapher Templates; ordinary Giac 179/179; cross-app 14/14; Calculus engine 56/56. |
| Fixed-pool lifecycle | 50 mixed cycles / 300 observations; stable HOME restoration; `pool50-final/result.json`. |
| Page-performance counters | One proof build; zero redundant polynomial captures on old quadratic/complex navigation; `perf-last`. |
| Firmware | Native, production WROOM normal/bring-up/demo and CAM normal/validation pass; commands and artifact hashes in `build-identities.json` and `guard-*.json`. |
| Web | Final package/validation/smoke and complete Chromium/Firefox/WebKit teaching pass; `web-final/results.json`. Each browser retains 201 exact 320×240 frames, including 02A and 02B. |
| WASM-MATH | Release and Debug configure/build/package/validation/regression pass on the final checker; `wasm-final/guard-final`. |
| Whitespace | `git diff --check` passes. Existing line-ending notices are not suppressed. |

The ordinary Giac suite's first RSS run failed its historical replot threshold;
a fresh baseline also failed, and candidate reruns (including the final kernel)
passed 179/179 with 0 KiB reported replot growth. First failures and both scopes
are retained in `giac-rss-*` and `final-core-*`; no threshold was changed.
The browser's exact ordinary spelling `ln(8)/ln(2)` is accepted alongside `3`
only after symbolic reconciliation by C++; no ordinary adapter/root rewrite
was introduced. Browser modifier regressions and their corrected runs are
retained under `wasm-final`.

The final exact-power bound compares signed endpoints directly; it does not
take `abs(INT_MIN)` from a forged operand. The extra boundary mutation rejects
that value without attempting a huge power. Final host and WASM math reruns
include this guard. One PlatformIO run failed importing an installed SCons
module; the unchanged pinned-tool retry passed, with both logs retained.

All native contacts and their source frames are in `native-complete/teaching`.
Individual images are exactly 320×240; contact sheets preserve the complete
scroll sequences. Direct review covered exp injectivity, common base, impossible
range, exponential isolation, log domain/isolation/injectivity, explicit base,
and pseudolocale. `2eˣ` now uses natural notation; conditions and exact powers
remain readable at the original font size. No goldens or masks were changed.
This is direct agent review and executable replay, not independent human or PCB
visual acceptance. There is no claim of an independent reviewer approval.

The last new-method native rerun after the integer-bound guard contains 13
guided/summary/locale sequences, 62 pages and 86 scroll frames in
`teaching-final`. The final browser rerun uses separate local ports solely for
parallel test isolation; the calculator still runs the same offline C++ WASM.

## Measured host resources and timing

Pinned tools: PlatformIO 6.1.19, espressif32 6.12.0, Arduino ESP32 2.0.17
(IDF 4.4.7), Xtensa GCC 8.4.0+2021r2-patch5, native MinGW GCC 15.2,
LVGL 9.5.0, Giac 1.4.9+khicas.57, Emscripten 6.0.3. No upgrades.
Build directories are isolated under `C:/.codex-cache/tutor-engine-02b/`.

Ordinary WROOM: linked flash **5,549,661 B** (+32,372 B against 02A), image
**5,550,032 B**, static RAM **118,984 B** (unchanged), `.iram0.text`
**60,407 B** (unchanged); vectors add 1,027 B, for **61,434 B total IRAM**.
Image SHA-256: `1e196f1a67049dc20e673923801fba6bb4606f050faa9f91ec815a5936f7b06e`.
This is a newly built image, not the previously hardware-tested 02A binary.

New seeded traces peak at **18,934 B retained**, **10,869 B tracked vector
construction storage**, **1,309 counted symbolic operations**, 3 conditions and
12 steps. These remain below existing limits; the old 02A corpus still has the
larger retained maximum (30,043 B). The expanded explicit-base challenge reaches
25,453 B retained, 16,008 B tracked vectors, 1,839 symbolic operations and 6
conditions. These are separate maxima, not a simultaneous
total allocator peak. Strings, vendor allocations and allocator overhead have
different accounting; unchanged static RAM does not imply zero dynamic cost.

Xtensa disassembly own frames: planner 1,344 B; admission form 192 B; exact
positivity 160 B; display conversion 144 B; exact-power recognizer 112 B.
Recursive source/display walks retain the existing depth bound. Own frames
exclude callees/vendor recursion and are not task-stack high-water measurements.
No real-board stack or internal/PSRAM free-block measurement was performed.

| Final build target | Linked flash B | Static RAM B |
|---|---:|---:|
| WROOM ordinary | 5,549,661 | 118,984 |
| WROOM bring-up | 5,561,037 | 119,168 |
| WROOM demo | 5,489,093 | 119,128 |
| CAM ordinary | 5,462,333 | 117,576 |
| CAM validation | 5,506,093 | 117,584 |

Fixed 64 KiB LVGL: minimum **sampled** free payload 7,280 B; after HOME,
19,160 B free, 67 objects, 3 timers and no retained Giac handles. The monitor's
57,472 B HOME pool payload excludes allocator bookkeeping. The earlier 02A
mixed fixture minimum was 7,352 B, but that mix differs; the 72 B difference is
not a matched peak-allocation delta. No Giac resets were used between cycles.

Host function timings below are milliseconds, with 5 warmups and 30 steady
samples. Runs share one context; first-per-fixture samples are **not** separate
cold boots. Compilation and other host work can affect wall-clock tails.

| Fixture | Ordinary median | Generation median / p95 / max | First generation |
|---|---:|---:|---:|
| `e^(2x−1)=e³` | 0.163 | 1.298 / 1.819 / 1.849 | 1.212 |
| `2^x=8` | 0.134 | 1.316 / 1.948 / 2.029 | 1.223 |
| `ln(x−1)=2` | 0.119 | 1.667 / 2.271 / 2.347 | 1.934 |
| `log_2(x)=3` | 0.168 | 1.041 / 1.461 / 1.525 | 1.697 |

| Page fixture | First page / first final | Warm final reopen median / p95 / max |
|---|---:|---:|
| Old quadratic | 1.348 / 0.704 | 1.141 / 1.386 / 1.459 |
| Old complex | 0.784 / 0.607 | 0.895 / 1.305 / 1.527 |
| Fixed-base exp | 0.726 / 0.262 | 0.840 / 1.110 / 1.454 |
| Log domain | 0.781 / 0.340 | 0.953 / 1.258 / 1.462 |
| Log isolation | 0.749 / 0.543 | 0.894 / 1.106 / 1.150 |

Matched baseline final reopen medians were 1.085 ms (quadratic) and 0.835 ms
(complex). The final host medians rose about 5–7% in these two old fixtures;
the unchanged hard operation counts are the regression oracle,
these noisy host timings do not establish an ESP32 speedup. Slow samples remain
included, including earlier 4.979 ms and 2.969 ms tails in `perf-complete`.
Raw nested scopes, initial/revisit/mode-switch samples and profiler
identities are in `perf-baseline`, `perf-last`, `profile-last` and
`measurements.json`. Nested scope durations must not be added together.

## Resource and physical scope

The existing limits remain: 48 steps, 4 branches, 8 conditions, 64 KiB retained
trace payload, 128 KiB tracked construction vectors, and 4096 symbolic boundary
calls. This phase produces one branch and performs bounded rational-power
recognition. No new permanent page cache, hidden widget trees, per-frame Giac
calls or layout/draw allocations are introduced. The LVGL pool stays 64 KiB.

Host C++ allocation injection does not cover every Giac malloc. Retained payload,
tracked vectors, sampled LVGL free blocks, process RSS and stack frame sizes are
different measurements, not additive heap peaks. No new ESP32 timing, heap or
stack claim is made: physical acceptance is a future closeout.

Future physical checklist: ordinary result and Steps for an exponential
injectivity case, fixed-base exact power, impossible exponential range,
logarithm domain/inversion and explicit-base logarithm; scroll every restriction;
exercise TOOLBOX, guided/summary, pan, BACK, edit/cancel, HOME and reopening;
sample mixed old/02A/02B lifecycle and cached quadratic page timings without
resetting Giac. Flash authorization must be obtained for that separate task.

Next mathematical work should address exact non-affine child constants and
conditional families through explicit checked extensions; it must not silently
expand this release into general transcendental solving.

## Final source and proposed boundary

Runtime source fingerprint (sorted `path + NUL + file SHA-256 + LF` over
`src/`, `lib/`, `boards/`, `platformio.ini`):
`4ac35c17d79efa97167a6e3f88c2afb00fcc176104b4fc2c257d5228f0f3e41c`.
The full manifest, before/after hashes and unrelated-file check are in
`out/tutor-engine-02b/final-source.json`. The compiled snapshot has no runtime
source differences from the working candidate. Historical physical 02A evidence
remains attached to its original source/image, never to this new binary.

One future feature boundary is appropriate:
`feat(tutor): explain exponential and logarithmic equations`.
It includes the narrow generated-notation and browser semantic-boundary fixes,
required to present these methods faithfully. No commit was created; HEAD stays
`57cbb0b585d70539f9e38650055f9e3725066589`, the index is empty, and the unrelated
`.vscode/settings.json` is untouched. No push, flash or storage operation occurred.

Exact changed paths (ignored evidence and unrelated editor files excluded):

```text
docs/TUTOR_ENGINE_02B.md
scripts/benchmark-tutor-pages.py
scripts/build-tutor-ui-allocation-probe.py
scripts/profile-tutor-pages-native.py
scripts/test-equations-rebuild.py
scripts/test-tutor-engine.py
scripts/test-tutor-nonlinear-lifecycle.py
scripts/test-tutor-teaching-ui.py
scripts/test-tutor-transcendental-host.py
scripts/test-tutor-transcendental-ui-failure.py
scripts/test-tutor-transcendental.py
scripts/tutor-teaching-web.mjs
src/apps/EquationsApp.cpp
src/apps/TutorPresentation.h
src/apps/TutorPresentation.inc
src/apps/TutorStepsView.inc
src/hal/NativeHal.cpp
src/math/CalculationEngine.cpp
src/math/GeneratedMathNotation.h
src/math/giac/GiacEngine.cpp
src/math/giac/GiacTutor.inc
src/math/giac/GiacTutorNonlinear.inc
src/math/giac/GiacTutorTranscendental.inc
src/math/giac/GiacTutorTranscendentalChecks.inc
src/math/giac/GiacTutorTranscendentalPlanner.inc
src/math/tutor/Derivation.h
src/math/tutor/Messages.inc
src/math/tutor/TeachingPlan.h
tests/fixtures/tutor-transcendental-challenge.json
tests/host/math_notation_fixtures.h
tests/host/math_notation_main.cpp
tests/host/tutor_nonlinear_allocation.cpp
tests/host/tutor_teaching_math.cpp
tests/host/tutor_transcendental_checks.cpp
tests/host/tutor_transcendental_probe.cpp
tests/host/tutor_transcendental_timing.cpp
```


## Physical closeout: hardware finding and teaching feedback

The closeout started on `57cbb0b585d70539f9e38650055f9e3725066589` with an empty index and the uncommitted 02B candidate. Readable binary patches, feature-file ZIP and hashes are retained under ignored `out/tutor-engine-02b-closeout/preservation/`. The unrelated `.vscode/settings.json` remains unchanged (SHA-256 `00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`). No commit had been made at that preservation checkpoint.

A canonical physical edit exposed `2^(x+0)=8`: the original inverse theorem retained `x+0`, but the planner skipped collection when its coefficients were already 1 and 0. The original-equation checker correctly rejected that unisolated candidate. The planner now explicitly emits the existing independently checked Collect rule before candidate validation. Six affine-identity regressions and six wrong-collection mutations cover this boundary. The checker and primary answer adapter are unchanged. This correction has its own source/image identities and post-fix replay evidence in `correction/`; earlier acceptance logs are not post-fix evidence.

The human exponential review confirmed readable formulas but rejected the generic "Subtract the displayed term" plus "Amount used" presentation. The subsequent narrow view correction adds a `BalancedOperation` formula reference, naming the verified step, its before state, active branch and row. It composes the checked addition or division on both sides **after** Giac conversion, so simplification cannot erase the operation:

- `2e^x+1=7`, then `2e^x+1-1=7-1`, then `2e^x=6`.
- `2e^x=6`, then `(2e^x)/2=6/2`, then `e^x=3`.
- `3ln(x)-6=0`, then `3ln(x)-6+6=0+6`, then `3ln(x)=6`.

These are structured VPAM equations/fractions. Every side owns separate AST nodes. No proof state, operation, condition or verification flag is changed. Numeric view captions use typed localized templates derived from the verified integer operand; compound operands remain structured mathematics. Guided view shows the arithmetic and resulting state. Summary keeps the existing checked transition chain and uses the same operand-aware captions. No detached operand tile remains for addition/division; denominator-clearing and system notation retain their existing separate presentation.

There are still four reusable formula widgets, the 640-node preparation bound and the 16-conversion bound. Construction remains transactional, outside layout/drawing. A balancing page adds one equation conversion compared with the former operand tile; quadratic formula and final-page paths are unchanged. No global cache, renderer change or LVGL-pool increase was introduced.

Current evidence for this presentation correction is under `out/tutor-engine-02b-closeout/balancing/`: complete 320x240 native sequences (including scrolling), typed formula/operand assertions, locale checks, full 564-trace equality (states, steps, snapshot, verdicts and call counts), notation/phase tests, Equations regressions and 50 mixed fixed-pool cycles. The minimum sampled pool payload was 7,296 bytes free; post-HOME returned to 19,160 bytes free, 67 objects and 3 timers, matching the previous restoration state. This is sampled pool payload, not a C++/Giac allocation peak.

Presentation runtime source fingerprint: `671ee924b1dcd7bd8fd5d24a23675ae0754f14372c58fe5ed2a9295fb15639eb`. Ordinary image SHA-256: `8bbbeeca31c4bebe82769943cb2362d06ff6e1938ff0cc2b07e586da262d9d38`, image 5,552,576 bytes, linked flash 5,552,209 bytes, static RAM 118,984 bytes, IRAM text 60,407 bytes (vectors separately 1,027). Against the immediate affine-correction build: image +2,080 bytes, linked flash +2,072 bytes, unchanged static RAM/IRAM. Dynamic AST construction is additional bounded work, not zero heap cost. Toolchain/configuration remain pinned and unchanged.

The additional committed-source boundary needed for the presentation reference/diagnostic name is `src/apps/TutorPresentation.h` and `src/apps/EquationsApp.cpp`; all other changes stay within the documented 02B paths. The latest PCB measurements and final ordinary restoration are recorded separately below.


Additional post-feedback gates: 386 transcendental checks / 25 mutations passed; the original 564-case corpus remains exactly unchanged. New view assertions inspect the operator and operand on **both** sides, signed numeric captions, and the before-state/branch reference. Persistent and one-shot page allocation injection passed at 422 exponential and 36 logarithm points; subsequent healthy preparation matched the complete expected formula trees without proof regeneration. The host allocation overlay observed at most 36 displayed nodes and 2,960 bytes additional simultaneously tracked C++/node payload during the selected healthy page preparations. These counters exclude Giac malloc, allocator overhead and LVGL, and are not total process/board peaks.

The native matched navigation probe retained one proof build and zero redundant polynomial captures for quadratic, complex and logarithm workloads (30 samples after five warm-up iterations). Host cached-final medians were 738.10, 615.75 and 606.60 microseconds respectively, recorded as diagnostics under `balancing/page-benchmark/`; these are not hardware or LCD timing claims. WASM-MATH Release and Debug also passed after the presentation/catalog change.


The updated full browser sequences passed in Chromium, Firefox and WebKit, including package/smoke validation. Additional guided/summary and Spanish/French/pseudo walkthroughs for both isolation fixtures passed; screenshots are in `balancing/locales/`. Production disassembly reports own entry frames of 352 bytes for the formula builder, 64 bytes for its new side-composition helper, and 848 bytes for `drawStep`; these are individual function frames, not total recursive stack usage. The physical probe separately reports the FreeRTOS minimum-unused stack in bytes.


### Latest presentation source: automated physical replay
The authorized application-only write installed probe SHA-256 `a91b7054bee8033ed13c4b78a3eac38a8ce5e54d423b78378f6795b6aab6aea5` on the rediscovered WROOM COM9 / USB MAC `44:B1:76:A7:B7:2C`, active app0 at `0x10000`. Independent verify_flash passed and the 0..0x10000 prefix was unchanged. The flashing workflow wrote only the active application; no bootloader/partition/storage write, formatting, eFuse/security provisioning or display-profile change was requested. This temporary probe was subsequently replaced by the ordinary image as recorded below.
All 14 canonical production-input cycles passed on this source, without Giac reset: real exp injectivity, exact base-2 and rational base-4 exponents, negative exponential range, log domain/inverse, log isolation, log injectivity, old quadratic, ABS, radical, ordinary exp/log answers, physically accessible base-10 log, and exponential isolation. Reopening retained one proof construction; committed input edit invalidated/rebuilt it; cancel preserved it; HOME released the proof and restored the view resources. There were no unexpected resets, wrong roots or stale traces. Native/board state fingerprints matched exactly; 140 matched formula-reference/caption comparisons also passed. Telemetry is not a human LCD judgment.
Base-2 logarithm has a checked host/browser theorem but no current production matrix/template route; no special serial-only syntax was used. Its physical theorem case and timings remain NOT RUN. The existing physical LOG key authored base-10 logarithm and produced exact 100 for log10(x)=2.
Instrumented milliseconds (page timing is nested inside opening; not LCD first-pixel latency; three final-page reopen samples each):
| Fixture | Ordinary | Tutor | First open | Final reopen median / max |
|---|---:|---:|---:|---:|
| exp-injective | 24.006 | 289.705 | 44.672 | 39.147 / 39.187 |
| exp-common-base | 23.857 | 263.542 | 39.134 | 42.142 / 42.160 |
| exp-rational-power | 23.913 | 385.317 | 39.135 | 44.722 / 44.759 |
| exp-negative | 11.773 | 84.122 | 36.164 | 36.129 / 36.156 |
| log-domain | 23.099 | 359.372 | 39.658 | 43.915 / 43.987 |
| log-isolate | 21.070 | 349.870 | 39.593 | 43.514 / 43.516 |
| log-injective | 18.509 | 276.238 | 39.537 | 42.080 / 42.083 |
| old-quadratic | 48.045 | 460.666 | 60.073 | 57.850 / 57.872 |
| old-abs | 29.512 | 685.107 | 40.394 | 48.485 / 48.515 |
| old-radical | 35.554 | 1265.520 | 39.873 | 42.511 / 42.511 |
| ordinary-exp | 14.123 | 130.126 | 39.992 | 41.896 / 41.911 |
| ordinary-log | 13.648 | 135.991 | 39.483 | 41.930 / 41.960 |
| log10 | 30.579 | 224.173 | 39.045 | 42.201 / 42.215 |
| exp-isolate | 21.474 | 263.899 | 53.528 | 43.424 / 43.480 |

After warm-up, final HOME returned to internal free/largest 144,128/102,388 B and PSRAM free/largest 8,226,099/8,126,452 B; LVGL payload free 41,240 B, 71 objects, 5 screens and 3 timers. The initial HOME had internal free 144,592 B and PSRAM free 8,228,579 B. One-time retained differences stabilized; no continuing post-HOME decline appeared. Sampled minima: internal free 144,128 B; PSRAM free 8,211,375 B; LVGL free payload 34,120 B. These minima are separate observations, not a simultaneous allocation peak. Task stack minimum-unused high-water was 51,808 bytes; the pinned port reports byte-sized StackType_t. Raw per-boundary observations, lifetime heap minima and all timing samples remain in `physical-balancing/`.

Human feedback before this change confirmed exponential readability but requested the explicit balancing arithmetic. The six-page 3ln(x)-6=0 sequence was left open for the requested logarithm/LCD review. The user then explicitly instructed us to continue to completion and leave their visual review for later. That instruction changes the closeout gate; it is not an observation that those LCD pages were inspected.


### Ordinary restoration and local commit decision

On 2026-09-17 the user instructed: "Continua siempre, no me preguntes para seguir, luego cuando termines ya lo miraré". The remaining human review is therefore **deferred by the user**, not passed or fabricated. Human evidence remains limited to the earlier exponential readability observation and its requested balancing-presentation correction. Automatic semantic/physical tests, host images and human LCD observations have separate scopes.

The final installed image is the ordinary, non-instrumented `numos-esp32-s3-wroom-1u-n16r8` firmware, SHA-256 `8bbbeeca31c4bebe82769943cb2362d06ff6e1938ff0cc2b07e586da262d9d38`, 5,552,576 bytes. The workflow rediscovered COM9 and MAC `44:B1:76:A7:B7:2C`, verified the currently installed probe before preserving it, checked the active app0/OTA metadata, wrote only app0 at `0x10000`, independently verified the write, and confirmed the first 64 KiB unchanged. The earlier known-good ordinary and 02A recovery images remain preserved. See `out/tutor-engine-02b-closeout/board/ordinary-final/identity.json`, `verify.log` and `boot.log`.

The ordinary firmware booted successfully with PSRAM present and the unchanged SAFE production display configuration (40 MHz write / 10 MHz read). There was no probe-build banner. A final ordinary smoke entered `2^x=8` and `ln(x)=0`, opened Steps, navigated, returned to results and ended at HOME. Serial barriers demonstrate main-loop completion/no crash; this ordinary smoke does not provide proof telemetry or a new LCD observation. Exact ordinary answers and tutor independence were checked separately in the preceding 14-cycle probe run. The ordinary board is left at HOME.

The commit boundary is the 36 paths listed above, including the narrow affine-identity collection fix and the user-requested structured balancing presentation. No measurement overlay, binary, screenshot, log, backup or unrelated `.vscode/settings.json` belongs to the commit. One local commit is authorized with subject `feat(tutor): explain exponential and logarithmic equations`; its SHA and final status are recorded in the ignored `out/tutor-engine-02b-closeout/final-receipt.json` and the final handoff. No push, release tag or history rewrite is part of this closeout.

The committed runtime bytes match source fingerprint `671ee924b1dcd7bd8fd5d24a23675ae0754f14372c58fe5ed2a9295fb15639eb`, which was used for the latest board/probe and ordinary builds. Documentation/test-only updates do not change those runtime bytes. A subsequent rebuild carrying a different Git identity or build timestamp is not automatically the same tested binary; the installed binary is identified by the SHA-256 above.

Remaining limitations: manual post-correction LCD review is deferred; explicit base-2 logarithm is not reachable through the current production matrix/template path (host/browser theorem and independent ordinary answer verified, hardware case NOT RUN); generation remains bounded but can exceed one second for the old radical fixture. No performance or new-method work was started. Bring-up/demo/CAM and unrelated full application matrices retain their implementation-phase snapshot labels; they were not rerun after the narrow closeout changes. The affected native/production, complete corpus/replay/mutation, notation, teaching/locales, allocation, fixed-pool50 and web/WASM gates were rerun after the corrections and passed. First failures and the separate matched-fixture review of the reversible 8-byte HOME difference remain in the ignored failure ledger.

READY_FOR_NEXT_TUTOR_PHASE: YES - technical closeout accepted; manual LCD review deferred explicitly by the user.
