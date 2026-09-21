# TUTOR-ENGINE-03A — Giac-origin periodic Results

Implementation candidate, September 2026. No commit, push or PCB flash is part
of this phase. Evidence is under ignored `out/tutor-engine-03a/`.

## Preserved baseline

Repository `C:\Users\Juan Ramón\Documents\Calculadora`, `main`, starting HEAD
`b5ef38c3f97a16ab3df1370ab3bc5a49ef4ed6f2`. The index and working tree were
clean. The immediate runtime fingerprint was
`58425d222bb1a8ea370ea0c72a6b766f972524bacace4c5b5c4ba8b6d6a10346`, matching
accepted 02C. Full path/hash fingerprint:
`b0fa9591d81845b705e1344ae5226b93c4fdbd2dab982f76e5edd8052c9d4c3a`.

`baseline.json`, separate binary-capable patches, `worktrees.txt` and the
read-verified `feature-baseline.zip` preserve that state. The ignored editor
file has SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.
The historical image/linked-flash/RAM/IRAM values are
5,582,448 / 5,582,089 / 119,016 / 60,407 bytes; they are not measurements of
this new candidate. No earlier physical acceptance is claimed for 03A.

## Pinned Giac findings and independent origin

The vendored KhiCAS Giac 1.4.9+khicas.57 public `_solve` entry passes
`complex_mode | (all_trig_sol << 1)` to the existing solver. Direct `solve`
used by the previous ordinary adapter supplies its own finite mode instead.
`all_trig_sol(ctx)` is context-local for this non-null context; the generated
integer/real counters are legacy globals.

`tests/host/giac_periodic_probe.cpp` constructs real authored VPAM trees and
records canonical serialization, actual gen type/subtype/operator trees,
ordinary and public output, identifiers, flags and symbol-table restoration.
The baseline executable/evidence predates the adapter; see `probe-2.log`.
The first probe run used an invalid negative Number node; it is retained as
failed setup evidence, not used for representation conclusions.

Observed RAD examples (the printed names below are diagnostics only):

| Input | Public all-solution structure |
| --- | --- |
| sin(x)=0 | one affine expression `pi*n_0` |
| sin(x)=1/2 | two affine expressions with offsets pi/6, 5pi/6 and period 2pi |
| cos(x)=-1 | two affine expressions differing by one whole period |
| tan(3x)=1 | affine expression with offset pi/12, period pi/3 |
| sin(3x-1)=1/3 | two affine expressions containing exact asin(1/3) |

No parity expression occurred in these probes. Unknown/non-affine output is
declined, not sampled, scraped or truncated. DEG uses the existing explicit
argument conversion to radians before solving; offset **and** period thus
come directly from the same solver result. Inverse scalars remain explicitly
radian-valued, preserving 02C's DEG display convention.

The integer domain is a **producer contract**, not a name heuristic. In this
snapshot the three real trig isolation functions construct a fresh identifier
and use it as an arbitrary integer in their full-turn/half-turn formulas.
There is no integer type annotation on that returned identifier. The narrow
`numos_periodic_solve` wrapper observes those exact construction sites. It
quotes each observed identifier before use, records it, and restores the
prior quoted list and both global counters on every exit. It never assigns,
purges or clears a user value. A user `n_0=27` or `k=19` survives unchanged.
Unwrapped vendor operations retain their original behavior. See
`lib/giac/NUMOS_CHANGES.md`; no external source or language resource was copied.

The engine separately restores all-trig, angle and complex flags through an
RAII request scope and retains the existing serialized single-context call
guard and diagnostic capture. The reviewed producer path adds no assumptions.
Public entries still synchronize the selected product angle before that scope.
This is not a thread-safe vendor API or a second permanent context.

## Owned result and completeness contract

`math/PeriodicMath.h` is the neutral payload shared by ordinary results and
tutor families: exact offset/period, solve variable, unit semantics and a
scoped all-integers binder. Tutor-only theorem/check fields stay in its derived
type. No `giac::gen`, context reference or parser parameter survives in UI data.

The ordinary result carries origin, coverage, generation, referenced rational
bindings, family union, structured offset/period trees and the authored tangent
pole exclusion. `SolutionSetKind::Periodic` is distinct from finite groups.
`PeriodicComplete`, `Representatives`, `Conditional` and `Unconverted` are
distinct from the legacy finite-result representation; status and set kind
remain significant. These flags do not award a tutor verification badge.

Complete adaptation admits one authored sin/cos/tan equal to an exact rational
constant, with a nonzero rational affine slope and a domain-safe exact
intercept. Either side can hold the function. Referenced A–F values must be
exact rationals. The whole authored equality is walked **before** evaluation:
variable denominators, nested functions and undefined targets cannot disappear
through cancellation or stored-zero substitution. Existing pi intercepts are
accepted. A bound/assumed solve variable declines this unrestricted contract.

Every solver branch must be affine in the single observed producer parameter.
Only exact arithmetic, pi, roots and inverse trig nodes are accepted in its
coefficients. Floats, unknown identifiers/operators, extra parameters, output
overflow or failed structured conversion decline complete adaptation. Negative
periods are reversed by the integer-parameter bijection. Equivalent endpoint
branches are removed only after proving an integer offset shift.

Before publication, a separate conversion replay checks coverage in both
directions against **all original solver branches**, binder identity, units,
positive periods, exact coefficient trees and restrictions. A complete empty
answer additionally requires the admitted exact impossible sine/cosine range.
The source is always public Giac output; no tutor planner/state is read here.

| Form | Ordinary scope | Tutor / reconciliation |
| --- | --- | --- |
| Admitted real affine trig, RAD/DEG | Complete Giac periodic union | Independent complete theorem; exact set comparison |
| Admitted impossible sine/cosine range | Complete empty Giac result | Independently checked empty theorem |
| Authored canceled variable denominator, including stored zero | Existing ordinary behavior, no complete adaptation | Existing honest tutor refusal |
| Non-affine/multiple trig, nested/domain-bearing arguments | Existing representatives/failure | No added method |
| Complex policy, inexact or unresolved targets, unknown producer output | Existing ordinary path | No new complex or numerical theorem |

## Exact bounded set comparison

The comparator returns Equivalent, Different or Unknown, ignoring binder
spelling/ID and branch order but requiring matching variables and angle units.
It normalizes negative period orientation and proves rational commensurability
against one positive base period. For ratios p/q, a checked bounded LCM of
numerators gives a common period. Each input family becomes a finite collection
of residue **classes modulo that proven common period**. Equality is checked
in both directions by exact integer differences divided by the common period.
These classes are not sampled roots.

This proves whole-period shifts, sign reparameterization and, specifically,
`pi*k = (2*pi*k) union (pi+2*pi*k)`. A rational noninteger quotient proves a
nonmatch. An unsimplified symbolic quotient is Unknown, not Different. Different
requires an uncovered residue for which every possible match is disproved.

Limits: two families per union; ratio numerator/denominator <=64; checked LCM
<=64; at most 64 total expanded classes; 256 explicit symbolic work operations;
512 walked nodes and depth 32; 16 KiB accounted retained answer payload and
comparison input text. The work limit can return Unknown before the residue
limit. No float tolerance, unbounded integer search or general set CAS is used.

An independently complete tutor remains usable if comparison is Unknown or
allocation fails only in reconciliation. It does not fabricate agreement.
Finite representative membership remains Unknown for set equality.

## Results, Steps and export

Results uses the existing four reusable canvases for at most two families,
`k ∈ ℤ`, and a tangent restriction. It says **Periodic solutions**, never
“2 solutions”. Preparation owns all requested formulas before publication;
the fallback clears partially attached views. BACK, scroll/pan, locale change,
edit/cancel, committed edit, angle/generation guards and HOME retain their
existing ownership contracts. No periodic family is converted to scalar Ans/STO.
No additional page cache, per-frame Giac work or layout allocation was added.

The final Steps warning is conditional: it acknowledges independently equal
complete Giac sets only after exact reconciliation; representative-only or
unknown-comparison wording is retained where applicable.

The headless WASM API exports `setKind: periodic`, coverage/origin, structured
offset and period, restrictions, units, and an integer parameter with a scoped
ID. Its uint64 scope is a decimal **string** to avoid JSON-number rounding.
Serialization-limit failures return Unsupported rather than truncating a
complete answer. The API and browser contain no JS solver substitute.

## Detecting tests and evidence

Reproducible entry points (pass matching `--source`, `--build`, `--out`):

- `scripts/test-periodic-results-host.py`: direct conversion, comparator,
  producer probe, collision and persistent allocation tests.
- `scripts/test-periodic-results-corpus.py`: seeded/challenge ordinary answers
  and independent tutor reconciliation, repeated with tutor construction off.
- `scripts/compare-periodic-tutor-traces.py`: exact old proof-field comparison.
- `scripts/test-periodic-results-ui.py`: real Results/Steps and ownership flows.
- `scripts/build-tutor-ui-allocation-probe.py --results` followed by
  `scripts/test-periodic-results-ui-failure.py`: private Results failure overlay.

The corpus has 204 cases and no failures. All 761 prior traces retain their
mathematical states, steps, conditions, parameters and fingerprints. Exactly
163 periodic reconciliations upgrade Unknown→Verified; `invariance-final.json`
enumerates them and separately records operation-count/resource differences.
No entire trace section is ignored. Old/new mutations remain separate from
answer agreement.

Publication mutations cover wrong offset/period, missing/spurious branches,
lost tangent condition, invalid integer domain/binder, inconsistent sign/tree,
malformed parity expression and truncation. Comparator tests distinguish
harmless sign/shift reparameterization from wrong period, missing residues and
noninteger shifts, and enforce Unknown at unsupported/budget boundaries.

192 persistent C++ allocation fault samples restore context and allow later
healthy ordinary solves without resetting Giac. 64 Results view fault samples
cover one-shot and persistent failures, transactional publication and reopening.
This does **not** exhaust Giac malloc, LVGL or every allocator failure mode.

Two real recovery defects found by these tests were fixed narrowly: ordinary
solve attempted to allocate its error diagnostic after bad_alloc; periodic
Results attempted an allocating fallback string. The original failing logs are
retained. The old tutor allocation harness now permits only an independently
Verified/Complete theorem with Unknown reconciliation if that comparison fails.

Exact 320×240 images: `ui-before3/`, `final-gates/periodic-ui/`, and complete
guided/locale/scrolled sequences in `final-gates/teaching/`. Contact sheets are
review aids; original frames remain available. Results is intentionally taller
where two infinite families replace two finite representatives. No font shrink,
negative offsets, golden promotion or mask changes were used. Images were
reviewed directly by the implementing agent; no independent reviewer or human
LCD approval is claimed.

## Final measurements and regression ledger

The closing measurements and exact changed-file manifest follow below. Host
times are diagnostic, not ESP32 latency or LCD first-pixel measurements. Hardware
heap, PSRAM, stack minimum-unused and physical responsiveness are NOT RUN in
03A. A future optional board check should inspect RAD/DEG sine, affine sine,
tangent restrictions, Results↔Steps reopening, and HOME cleanup on ordinary
production firmware after separate authorization.


### Candidate identity and resources

Runtime fingerprint (all src/lib/boards paths plus platformio.ini, sorted
path/NUL/SHA-256/newline): `3b4269b899d9c565b180d06e694a3cbf10d36c59b4407c38b3dcdfe34ef612b0`. The ASCII build snapshot's runtime bytes
match the working source. `final-manifest.json` records all individual hashes,
the full-source fingerprint and exact feature boundary. HEAD/index remain at
the preserved baseline; the ignored editor hash remains unchanged.

Ordinary WROOM image SHA-256: `d4e3a4ac81ef28ad7fc69912deea7eade6df986848a1e5e5efad7cc7e8123eb7`.

| Bytes | Accepted 02C | 03A ordinary | Delta |
| --- | ---: | ---: | ---: |
| Linked flash | 5,582,089 | 5,602,477 | +20,388 |
| Image | 5,582,448 | 5,602,848 | +20,400 |
| Static RAM | 119,016 | 119,024 | +8 |
| IRAM text | 60,407 | 60,407 | 0 |

Pinned PlatformIO 6.1.19, private SCons 4.8.1, Xtensa GCC
8.4.0+2021r2-patch5, esptool 4.9.0, LVGL 9.5.0 and Giac
1.4.9+khicas.57 were retained. Native GCC is 15.2.0. Per-target separate build
directories, exact commands and exit codes are retained in the build JSON/logs.
No production configuration, clock, allocator or font changed.

The observed owned answer payload for the six timing fixtures is 0–3,338 B;
this accounts for C++ objects, vector capacities, strings and owned value trees,
not Giac internals or allocator headers. Its hard ceiling is 16,384 B.
Persistent C++ allocation instrumentation measured requested construction peak
increments of 6,969 B (unfamiliar sine), 4,591 B (tangent), and 1,817 B
(impossible range), separately—not summed as a simultaneous peak. Thirty warm
ordinary solves ended at zero tracked C++ delta. Giac/GMP malloc, allocator
fragmentation and physical internal-versus-PSRAM placement are not measured.

The final 50-cycle native fixed-pool run passes without Giac resets. Minimum
sampled LVGL free payload: 7,256 B. Every final HOME stabilizes at 67 objects,
3 timers, zero retained engine handles, monitored total 57,472/free 19,160 B.
The pool remains 65,536 B; monitored allocatable payload excludes overhead.
Results reuse at most four formula canvases. No per-family hidden page tree or
global cache is retained. Physical task-stack minimum-unused and total recursive
stack peak are NOT MEASURED; neither static RAM nor C++ accounting proves them.

### Host timing diagnostics

These measurements ran alongside builds, so scheduling outliers are retained.
Ordinary solve uses one cold sample, five warm-ups, then 30 samples in one
process without resetting Giac. Tutor generation is the single initial sample.
Units below are **microseconds**, not board or LCD latency. Reproduce with
`periodic_result_main.exe EQUATION --benchmark [--degrees]`; raw samples are in
`ordinary-timings/`. The work counter counts designated bounded operations,
not every internal Giac predicate/allocation.

| Fixture | Cold ordinary | Warm median / p95 / max (n=30) | Initial tutor | Retained payload | Work ticks |
| --- | ---: | ---: | ---: | ---: | ---: |
| sine | 2898 | 1351.5 / 2896 / 2918 | 3154 | 1986 | 34 |
| affine | 2299 | 1637 / 9252 / 9438 | 3236 | 1636 | 34 |
| unfamiliar | 6471 | 2455.5 / 7493 / 11684 | 18572 | 3338 | 33 |
| tangent | 1829 | 1090 / 8642 / 10706 | 2950 | 1558 | 16 |
| degree | 6902 | 4604.5 / 13573 / 16000 | 3525 | 1047 | 34 |
| quadratic | 1192 | 328 / 391 / 392 | 3606 | 0 | 0 |

Cached final Steps reopen uses the existing private page overlay, one warm-up
and three samples. Median/max microseconds: sine 1,447.9/2,014.6; affine sine
1,408.8/1,496.7; tangent 1,534.4/3,570; unfamiliar sine 2,896.2/6,279.4.
Raw first-page/final-page and all revisit/mode samples are retained in
`remaining/periodic-page-costs/`. These different pages are not treated as the
same workload. Old quadratic/complex counters still show one proof construction
and zero redundant polynomial captures. No timing threshold is used as proof. The old ordinary finite answer and the new
complete periodic conversion are different workloads; no ordinary-solve speedup
is claimed. A matched board performance comparison remains a later task.

### Final regression ledger

| Gate | Result and evidence |
| --- | --- |
| Prior tutor corpus and proof invariance | PASS 761; `final-gates/host.json`, `invariance-final.json`: exactly 163 reconciliation upgrades |
| Ordinary periodic corpus | PASS 204 seeded/challenge/boundary cases; independent no-tutor run for every supported case |
| Adaptation/comparison/state/mutations | PASS 2,215 assertions; `host-final-comparison/periodic_results_checks.log` in the ASCII cache |
| Allocation recovery | PASS 192 ordinary persistent samples, 880 tutor fault samples, 64 Results publication samples; Steps sine/DEG overlay checks pass |
| Notation/STIX/Delta/integer/serialization | PASS notation and existing MathEnginePhaseRegression; owned JSON binder scope is a decimal string to retain 64-bit identity |
| Teaching/provenance/locales | PASS guided/summary, original restrictions, final conclusion; Results English/Spanish/French/pseudolocale preserve identical families |
| Equations transactions and 50 fixed-pool cycles | PASS; `final-gates/equations/`, `final-gates/pool50-final/` |
| Ordinary Giac / Calculus / Neo / cross-app | PASS 179 / 56 / 44 / 14 checks, matching official host macro profile |
| Calculus/F7 and Grapher Templates | PASS focused flows and Calculus 50 cycles; `remaining/calculus50-env/`, `remaining/grapher/` |
| Page-performance counters | PASS old quadratic/complex plus new trig navigation; no proof regeneration or redundant polynomial capture |
| Native emulator | PASS final2 build and focused real-input flows |
| Production WROOM normal / bring-up / demo | PASS; final2 ordinary and final1 variants |
| CAM normal / validation | PASS; final1 variants |
| Whitespace/index | PASS git diff --check; index remains empty |
| Web | PASS package/smoke and Results/Steps Chromium, Firefox, WebKit; `wasm/results-recovery/web.json` |
| WASM-MATH Release and Debug | PASS build/package/regression including actual periodic API export; `wasm/periodic-api/` |
| Existing 42 application replays | PASS execution; strict pixel comparisons reported separately below |
| Hardware | NOT RUN; no PCB operation authorized/performed in this phase |

Strict comparisons retain all 18 historical golden mismatches. Against the
immediate accepted emulator, 2/42 images match exactly and 40/42 differ only in
the existing clock area x=220..249, y=8..16. No new body difference was found;
this diagnostic does not turn failed strict pixel comparisons into PASS.
`visual-ledger.json` retains every comparator exit and bounding box. Existing
Grapher Templates historical body drift remains; no golden/mask was changed.
The readable six-frame contact sheet is `results-steps-contact.png`; each
underlying original is exactly 320×240, with all scroll frames kept separately.

First failures are preserved: allocating error diagnostics (fixed); bad probe
negative-node setup; initial exact-comparison parsing (fixed); allocation test
context pre-synchronization; Unicode absolute replay path; Calculus runner with
fewer than its ten warm-up cycles and then missing DLL PATH. Corrected reruns
are recorded separately. An initial final-invariance command accidentally used
the pre-closeout 02C implementation corpus; the accepted closeout corpus proves
all 761 graphs unchanged. No failure exit was suppressed or expectation relaxed
to allow an invalid proof.

### Exact proposed feature boundary

One future commit: `feat(equations): preserve Giac periodic solution families`.
No commit has been created. The boundary contains these 32 files:

- `docs/TUTOR_ENGINE_03A.md`
- `lib/giac/NUMOS_CHANGES.md`
- `lib/giac/src/ksolve.cc`
- `lib/giac/src/numos_periodic.h`
- `scripts/build-tutor-ui-allocation-probe.py`
- `scripts/compare-periodic-tutor-traces.py`
- `scripts/test-periodic-results-corpus.py`
- `scripts/test-periodic-results-host.py`
- `scripts/test-periodic-results-ui-failure.py`
- `scripts/test-periodic-results-ui.py`
- `scripts/test-tutor-engine.py`
- `scripts/tutor-teaching-web.mjs`
- `src/apps/EquationsApp.cpp`
- `src/apps/EquationsApp.h`
- `src/apps/TutorPresentation.inc`
- `src/apps/TutorStepsView.inc`
- `src/hal/NativeHal.cpp`
- `src/math/PeriodicMath.h`
- `src/math/giac/GiacEngine.cpp`
- `src/math/giac/GiacEngine.h`
- `src/math/giac/GiacPeriodic.inc`
- `src/math/giac/GiacTutor.inc`
- `src/math/tutor/Derivation.h`
- `src/math/tutor/Messages.inc`
- `tests/host/giac_periodic_probe.cpp`
- `tests/host/periodic_result_main.cpp`
- `tests/host/periodic_results_allocation.cpp`
- `tests/host/periodic_results_checks.cpp`
- `tests/host/tutor_nonlinear_allocation.cpp`
- `tests/wasm/math.mjs`
- `wasm/math/NumosMathSession.cpp`
- `wasm/math/numos-math.d.ts`

## Combined 03A/Spanish closeout — software snapshot (2026-09-21)

This is additional evidence; it does not change the scope of the implementation
measurements above. The starting branch is still `main` at
`b5ef38c3f97a16ab3df1370ab3bc5a49ef4ed6f2`, with an empty index and both feature
layers uncommitted. The ignored editor settings were preserved byte-for-byte.

The saved `candidate-03a.zip`, I18N baseline and path hashes were verified before
materializing a separate 03A source tree. Every source file matches the recorded
03A snapshot; later language files are absent. Its runtime fingerprint remains
`3b4269b899d9c565b180d06e694a3cbf10d36c59b4407c38b3dcdfe34ef612b0`.
Native and ordinary WROOM builds use their own clean build directory under
`C:/.codex-cache/tutor-03a-es-closeout/03a/`.

Isolated checks pass: periodic producer/comparator/mutations and allocation
recovery, 204 periodic corpus cases including tutor-disabled independence,
9 Results/Steps flows, and 92 Equations checks including lifecycle. The normal
image is 5,602,848 B, linked flash 5,602,477 B, static RAM 119,024 B and IRAM text
60,407 B. SHA-256:
`b8300b3cf5e2ce70c3b7a85fe70daac6815f02b44c154e620e62110c3959f33f`.
Commands, toolchain output and per-file identities are retained in
`out/tutor-03a-es-closeout/03a/`, `snapshots.json` and `firmware.json`.

The original I18N-only patch had duplicated CRLF line endings and failed a
direct applicability check. That failure is retained. A separate normalized
copy passes `git apply --check --ignore-space-change`; the verified byte archives
and manifests, rather than the malformed text patch, define the split. Neither
patch was applied over the combined working tree.

The authorized physical closeout below tests the combined runtime. This
isolated 03A binary has **not** been physically tested. The first commit's
software evidence belongs to its isolated snapshot; hardware evidence belongs
to the combined 03A/Spanish source and the separately identified images.

## Joint physical closeout (2026-09-21)

After explicit authorization for this task, USB discovery identified the
production WROOM at COM9, USB 303a:1001, serial/MAC `44:B1:76:A7:B7:2C`.
The active application is at `0x10000`, capacity `0x640000`. Only that
application was written. The prefix/partition bytes remained unchanged;
bootloader, NVS and security were not written or erased. Settings legitimately
saved preferences through its normal filesystem path. The prior full app-slot
backup and independent readbacks remain under ignored closeout evidence.

Thirteen main canonical-production-input cycles passed: sine zero, affine
sine EN/ES RAD, unfamiliar inverse sine EN/ES DEG, tangent EN/ES RAD,
quadratic EN/ES, radical rejection, logarithm, unique system and authored
`sin(x)=0*x/x` boundary. Giac-origin Results and independently checked Steps
reconciled exactly for admitted periodic cases, including `pi*k` versus the
two `2*pi` families. The domain-bearing canceled target retained representative
scope and unavailable complete tutoring; no unrestricted family was invented.
Tangent retained its pole restriction. State fingerprints, typed family
payloads and checked auxiliary parameters matched host replay. Reopening kept
one proof build; edit/cancel preserved it; HOME removed app-owned proof/families.
Binder collisions remain host evidence, not a fabricated physical input.

Combined runtime fingerprint:
`a5cc45687930d7a14c6b0feafeb0d2fbdbb3595702f28621c139d760fdbc5f50`.
Temporary default-off probe SHA-256:
`0bfebf982e16369be00eb1b708b108c5e3363034811e1ec1e286f555238ab5a7`.
Restored ordinary image SHA-256:
`12e1277cf0e54c0db004312391d30dede901d67b50dc0571a7063d6282ef847d`.
The complete application readback matched each installed image, and ordinary
boot plus canonical sine/logarithm/Steps/HOME smoke passed. The final board is
HOME, RAD, Spanish. No probe is installed. Ordinary image: 5,621,408 B;
linked flash 5,621,041 B; static RAM 119,160 B; IRAM text 60,407 B.
The installed artifact was built before the local commits. The committed
runtime bytes match that source; a later rebuild with new Git/build metadata
is not retroactively the physically tested binary.

Detailed matched timings, memory stabilization (including a retained failed
strict diagnostic), preference reset tests and evidence limitations are in
the companion Spanish report's joint-closeout section and ignored
`out/tutor-03a-es-closeout/physical/`. No production source fix was needed on
hardware. Human LCD review is deferred. This physical acceptance does not
claim that the isolated first-commit binary was flashed.
