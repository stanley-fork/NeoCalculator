# TUTOR-ENGINE-02A — real absolute values and principal roots

## Baseline and scope

This candidate starts from clean `main` at
`76b238e17237cc3e834350cc4d89867e9ec54f0e`, including committed notation/TeX
delta (`76b238e`), page performance (`62a4dc3`), checked teaching (`90fa81e`)
and Equations editing (`6665d38`). The immediate 1,227-file SHA-256 manifest
fingerprint is `07f2ddc912a59c816874a9f2afef8ce345c6c67cc28d9cd4e757e3c819a65c59`.
Readable archive, separate binary-capable index/worktree patches, status,
worktrees and the unchanged editor configuration are preserved under ignored
`out/tutor-engine-02a/`. No history/index/storage/hardware operation is required.

| Real-domain equation | Checked coverage |
|---|---|
| One `abs(u)` with nonzero rational coefficient | Isolate; require the isolated other side nonnegative; split into `u=v` and `u=-v` |
| `abs(u)=0` / exact negative RHS | One zero case / direct empty classification |
| One principal `sqrt(u)` with rational coefficient | Preserve authored radicand domain; isolate; require other side nonnegative; square to candidates |
| Polynomial cases | Existing exact degree-at-most-two planner, including real irrational roots and empty branches |
| Finite union | Check each candidate against conditions and the authored equation; reject, deduplicate, finish |
| Stored rational values | Existing snapshot/substitution and invalidation contract |
| Complex policy, nested/multiple functions, nth roots, special-function denominators, degree >2 | Tutor unsupported; ordinary Giac result remains independent |
| Conditional infinite branches such as `sqrt(x^2)=x` or `abs(x)=x` | Unsupported: needs an inequality/family method |

There is no inequality solver, numerical proof, runtime prose generation,
second parser/CAS, page cache, new nonlinear search or hardware change.

## Representation and checker

The authored VPAM absolute value is `NodeParen(Bar)`, not a text `abs` label;
the radical is `NodeRoot`. Existing serialization produces `abs(...)` and
`sqrt(...)`. Giac parsing retains `at_abs`/`at_sqrt`; evaluation can turn a root
into a fractional power or simplify `sqrt(x^2)` to `abs(x)`. Admission and domain
capture therefore inspect the **parsed authored tree before evaluation**.
`tests/host/tutor_nonlinear_probe.cpp` records these representations.

Conditions now have a kind (`Nonzero`, `Nonnegative`, reserved `Positive`),
role (denominator, authored radicand, isolated range), exact canonical expression,
source equation/side/path/state, verdict, and an immutable engine-neutral
structured expression. The legacy canonical member name `nonzero` is retained
for compatibility; the kind determines its relation. New conditions render
directly through VPAM `≥`; they are never ordinary equation branches or English
strings. They participate in equality, fingerprints, budgets and replay.

| Stable rule | Preconditions and independent replay | Relation |
|---|---|---|
| `domain.radicand_nonnegative` | Recapture the authored real radical and its exact path; verify the typed condition | Annotation |
| `condition.range` | Actual isolated special node and declared RHS; exact nonnegative condition and source state | Annotation |
| `abs.cases` | One real isolated abs; retained range; exactly `u=v`, `u=-v`, or one `u=0`; exact branch identities | Exhaustive split |
| `radical.candidates` | Principal real root; original radicand and isolated range retained; exactly `u=v^2` | Candidate implication |
| `terminal.nonnegative` | Actual isolated special node and exact negative rational RHS | Terminal classification |
| Existing add/divide/swap | Mask the special subtree as one opaque term; independently replay the actual rational operation | Equivalent |
| Existing polynomial primitives in a case | Project that case, replay the old verifier, lift its result and prove unrelated cases unchanged | Original primitive relation |
| `candidate.original_valid` | Exact substitution into all conditions and original authored equality | Annotation |
| `candidate.original_invalid` | Recompute the actual failed sign, radicand/domain or original equality | Rejection |
| `candidate.duplicate` | Equal earlier accepted and original-verified value | Rejection of duplicate entry |
| Existing finish | No unresolved cases; accepted values original-verified and unique; exhaustive preceding theorem/branch replay | Terminal classification |

The generator cannot certify a new theorem by writing `Verified`. Full replay
recaptures conditions, verifies typed trees/provenance, operands, messages,
relations, branch transitions and terminal classification. Inconclusive exact
sign/equality checks yield unsupported/partial, never a trusted guess.
A rejected candidate may be discharged by an exact violation of a necessary
condition derived from the original equation; it need not then be evaluated
again. Every accepted candidate is explicitly substituted into the authored
equation, never only into the squared polynomial.

The child planner uses the same Giac context and existing polynomial rules. A
`caseStart` reference binds each lifted primitive to the checked theorem state;
`caseBranch` and branch origin preserve parent cases across child factor splits.
Empty cases, rejected candidates and duplicate entries remain in the proof but
are excluded from the final accepted set. There is one bounded level of case
lifting, not recursive nonlinear branch expansion. The opaque symbol is local,
quoted with RAII and never assigned; the prior context quote list is restored.

### Ordinary Giac reconciliation defect found by the probe

The pinned vendor's direct `solve(equal(sqrt(x),3),x)` returned `[]`, whereas
`solve(sqrt(x)-3,x)` and public `_solve` returned `[9]`. In `ksolve.cc`, direct
solve initially applies `exp2pow`, then overwrites that work when converting an
equality; public `_solve_uncompressed` calls `equal2diff` first. The adapter now
uses that existing Giac conversion **only for an authored `at_sqrt` equation**.
No tutor roots are fed into the primary answer. The minimized probe and both
outputs are in `vendor-radical-probe.cpp`, `vendor-probe.log` and the answer-probe
logs. Vendor source is unchanged. Explicit fractional-power input is outside
this narrow adapter correction and is not admitted as a principal-root theorem.

## Teaching and complete examples

The existing four formula slots, transactional publication, guided/summary,
scroll, VAR pan and cached derivation remain. Case labels distinguish nested
abs/quadratic cases; polynomial roots inside these methods are candidates until
original checking. Rejected values use the existing excluded styling. The final
page contains accepted values and restrictions; a structured solution-set row
handles four roots without allocating another widget. Δ/natural products and
the optimized closed-root conversion path are preserved.

`|2x-3|=5`: retain `5≥0`; split `2x-3=5` and `2x-3=-5`; case 1 adds 3,
giving `2x=8`, then divides by 2, giving `x=4`; case 2 adds 3, giving `2x=-2`,
then divides by 2, giving `x=-1`; independently check both in the original;
finish with `{4,-1}`. No branch operation is attributed to an unchanged case.

`|x-1|=x+3`: retain `x+3≥0`; split `x-1=x+3` (contradiction) and
`x-1=-x-3`; add `x`, add 1, divide by 2; `x=-1` passes the original and sign
condition; finish with only `-1`. `|x|=-3` is a single no-solution explanation.

`sqrt(x+1)=x-1`: preserve `x+1≥0`, require `x-1≥0`; square to
`x+1=(x-1)^2`; expand the right side; subtract it from both sides, obtaining
`-x^2+3x=0`; factor and split; solve `-x+3=0` and `x=0`; accept 3;
reject 0 because the isolated other side is negative; finish with 3 and both
authored/isolated conditions. Squaring is explicitly candidate-producing.

`sqrt(2x+3)=x`: retain `2x+3≥0` and `x≥0`; square to `2x+3=x^2`;
subtract `x^2`, factor `(-x+3)(x+1)`, solve the two cases; accept 3 and reject
-1 by the sign requirement; finish with 3.

`2sqrt(x+1)+1=7`: retain `x+1≥0`; subtract 1; divide by 2;
`sqrt(x+1)=3`; retain `3≥0`; square to `x+1=9`; subtract 1;
check `x=8` in the authored equation; finish with 8 and the conditions.

English templates and typed message verification cover every new rule.
Representative Spanish/French templates use the same keys; missing translations
explicitly fall back to English. Pseudolocale and locale switches preserve the
mathematical graph. Scalar operands are structured formula widgets where the
existing message schema calls for a displayed amount.

## Evidence and reproducible gates

Evidence is kept under ignored `out/tutor-engine-02a/`; ASCII build snapshots
and separate output directories are under `C:/.codex-cache/tutor-engine-02a/`.
Pinned PIO 6.1.19, espressif32 6.12.0, Arduino 2.0.17/IDF 4.4.7, Xtensa 8.4,
MinGW 15.2 and Emscripten 6.0.3 are unchanged. Build command/result manifests
record the exact configuration, source closure and executable identities.

Entry points (pass a matching executable/build):

```text
scripts/test-tutor-engine.py --bin <host-tutor> --out <output>
scripts/test-tutor-nonlinear.py --bin <host-tutor> --out <output>
scripts/test-tutor-nonlinear.py --bin <host-tutor> --out <output> --challenge tests/fixtures/tutor-nonlinear-challenge.json
tests/host/tutor_nonlinear_checks.cpp
tests/host/tutor_nonlinear_allocation.cpp
scripts/test-tutor-teaching-ui.py --bin <native> --out <output>
scripts/test-tutor-nonlinear-lifecycle.py --bin <fixed-pool-native> --out <output>
scripts/benchmark-tutor-pages.py --bin <page-probe> --out <output> --expect-no-polynomial-scan
scripts/tutor-teaching-web.mjs <ascii-snapshot> --nonlinear --browser=chromium
```

The new seeded corpus has 140 rational/variable-RHS/quadratic-inner/radical
members, plus 11 separate challenge equations. All Complete traces undergo C++
independent replay and ordinary-answer reconciliation. The checker harness has
235 assertions and 23 rejected mutations (including missing conditions/branches,
wrong squaring, incorrect rejection, original-check omission, duplicate zero,
provenance/tree corruption, branch identity, stale epoch and false Complete).
The old 180-member corpus still runs: 174 supported proof payloads are identical;
four old refusals differ only in their developer diagnostic; two historical
abs/root refusals are intentionally promoted to checked coverage. The precise
field comparison is retained in `trace-invariance.json`.
Twenty additional deterministic reruns compare every field except elapsed
microseconds. Historical replay against its own expected snapshot remains a
mathematical operation; current-page availability also requires
`tutorSnapshotCurrent`. The reset test verifies that guard and rejects the old
proof against a newly captured engine generation.
The final mutation deletes the entire duplicate-rejection step and repairs all
indices/hashes. A compiler-built negative control without the new terminal
uniqueness guard accepts it; the final kernel rejects it. Across all 151 new
fixtures the guard changes only elapsed time/symbolic-call counts: states,
messages, conditions, branches, verdicts and retained payload are identical.
All 54 view sequences and 659 scrolled frames below the global clock header
are also identical (only the view's preparation-time field is excluded).

Generation fault injection holds global `new` failure active through recovery:
240 sampled failure sites return safely, restore trace vectors, and allow a
later healthy proof without resetting Giac. This does not exhaust all malloc or
real-board failure modes. Page-construction failures are tested separately with
the existing isolated AST/allocation overlay.

Full 320×240 top-to-bottom native sequences cover the six required examples,
four-root union, wide formulas, ES/FR/pseudolocale and existing methods.
Contact sheets retain individual frames. Review was performed directly by the
implementing agent; no independent reviewer approval is claimed. The first
legacy SHIFT/SQRT script incorrectly entered roots for abs fixtures; retained
`ui-first` output is not abs evidence. Corrected tests use canonical physical
matrix events and assert the authored abs node. Browser tests select real
policy through Settings and route SHIFT/SQRT through the production semantic
resolver; other keyboard behavior remains unchanged.

Complete native walkthroughs (individual 320×240 PNGs beside each sheet):
[absolute affine](../out/tutor-engine-02a/guarded-ui-regression/teaching/abs-linear-guided-contact.png),
[absolute variable RHS](../out/tutor-engine-02a/guarded-ui-regression/teaching/abs-variable-guided-contact.png),
[absolute negative RHS](../out/tutor-engine-02a/guarded-ui-regression/teaching/abs-negative-guided-contact.png),
[extraneous radical candidate](../out/tutor-engine-02a/guarded-ui-regression/teaching/radical-extraneous-guided-contact.png),
[linear radicand](../out/tutor-engine-02a/guarded-ui-regression/teaching/radical-linear-guided-contact.png),
[radical isolation](../out/tutor-engine-02a/guarded-ui-regression/teaching/radical-isolate-guided-contact.png).
The numbered `contact-part` sheets preserve readable scale for long sequences.

## Resource contract and limitations

Bounds remain 48 steps, 8 conditions, depth 20, 160 source nodes, 512-byte source
and expression strings, 4,096 symbolic calls, 64 KiB accounted retained trace and
128 KiB construction trace-vector budget. Maximum branches increases from 2 to
4 for two quadratic abs cases. No LVGL reservation increase or hidden page tree.
The new 140-member corpus maximum is 30,043 accounted retained bytes, 12,184
live trace-vector bytes, 16,798 peak trace-vector bytes, and 2,395 symbolic calls.
These maxima can belong to different inputs and must not be summed as one peak.

Typed condition trees are shared immutable owners. Accounting conservatively
charges their payload for each retained state (including capacities and a
control-block allowance); it is not a measurement of total allocator/Giac heap.
Trace vectors use the existing ESP32 PSRAM-aware allocator; ordinary strings,
shared tree ownership, Giac temporaries and allocator overhead are outside the
trace-vector counter. Traversals occur during bounded construction, not draw.
No per-frame Giac call or layout allocation is introduced.

The 50 mixed fixed-pool cycles sample a minimum 7,352 bytes free. Every post-HOME
sample restores 67 objects, 3 timers and 19,160 free payload bytes. LVGL reports
57,472 payload bytes at that state: its monitor excludes allocator block headers,
so this is not a changed 64 KiB pool reservation. Sampling is not a total
peak-allocation measurement. Giac is not reset between cycles.

Hardware latency, PSRAM/internal-heap largest blocks and physical task-stack
high water are **NOT RUN** for this phase. Existing physical timings describe
earlier binaries. Host timings are diagnostic only; no new ESP32 claim is made.
An optional future physical check should inspect abs case labels, radical sign
rejection, wide-formula pan and warmed quadratic reopening before any authorized
measurement image is installed.

Known limits remain conditional infinite families, undecidable exact signs,
branch/node/byte budget exhaustion and the ordinary adapter's separate
fractional-power input behavior. Negative-leading-factor products may retain
the notation policy's conservative explicit multiplication fallback; no factor
reordering or renderer change was made. A future phase may add checked
inequality/family reasoning; logs, exponentials, trig and arbitrary roots are
not implemented here.

Suggested independent future commit boundary: all files listed in the final
phase manifest, including the narrow radical adapter correction and browser
input/diagnostic seam, on top of `76b238e`.
Subject: `feat(tutor): explain absolute-value and radical equations`.
No commit is created by this task.

## Final regression and measurements

| Gate | Evidence / scope |
|---|---|
| Existing 180 traces, legacy teaching mutations, 35 composite-conclusion guard mutations | `host-regression-guarded/` |
| New 140 seeds, 11 challenge cases, required acceptance inputs | `seeded-guarded/`, `challenge-guarded/`, `acceptance-final.json` |
| New 235 checks / 23 rejected mutations and context guards | `nonlinear-unique-guard-run.log`; negative control in `duplicate-negative-control.log` |
| Ordinary Giac, Calculus, cross-app, Neo math backend | `giac-guarded-run.log` (179/179), `host-regression-guarded/` |
| New generation allocation recovery / old recovery | 240 persistent sites / 96 old cases |
| New page allocation recovery | 616 radical + 550 four-root abs fault runs; final caption build additionally 72 + 66; all successful recovery |
| Equations, teaching/provenance/locales, Calculus/F7, Grapher Templates, STIX and ± | `guarded-ui-regression/`; 54 teaching sequences, 297 pages, 659 scrolled frames |
| Structured natural notation / MathEnginePhaseRegression / TeX Δ sizes | `C:/.codex-cache/tutor-engine-02a/notation-guarded/`; 30 notation cases, phase suite; `glyphs/` nine visual cases; 2,624 teaching formula/provenance checks |
| Wide radical page pan and return | `final-wide-pan/`, 33 pan frames, unchanged trace and one proof build |
| Final 50 mixed fixed-pool cycles, explicit changed-draft cancellation | `guarded-lifecycle50/` |
| Native and five firmware environments | `guarded-native.json`, `guarded-pool.json`, `guarded-firmware.json` |
| Web build/package/smoke, Chromium/Firefox/WebKit teaching | `wasm-final/guarded/web.json`; six nonlinear fixtures in each browser |
| Headless WASM-MATH Release and Debug | `wasm-final/guarded/math-release.json`, `math-debug.json` |
| Whitespace / retained index / source identities | final preservation manifest and `git diff --check` log |
| Physical flashing, LCD/keypad confirmation, board memory/latency | NOT RUN; not authorized for this phase |

The first ordinary Giac RSS check failed once (4,792 KiB growth versus the
1,024 KiB threshold). Its log is retained in `host-regression/`; a matched old
baseline rerun and both later candidate runs reported zero RSS delta and
179/179 passes. This is a retained intermittent host RSS limitation, not proof
of zero Giac heap cost. No failure exit was suppressed or golden promoted.
Initial browser runs failed because complex policy was still enabled and then
because the Settings harness sent EXE instead of its existing ENTER event;
the corrected actual Settings traversal passes in all three browsers.
The notation UI assertion was strengthened to inspect the final exact scalar
subtree (including unary minus), rather than finding its spelling in any leaf.
An initial stale-context test incorrectly expected historical mathematical
replay against the same old snapshot to perform the separate current-view
guard; the corrected test exercises both documented boundaries.

Host page timings use the same optimized baseline and final page probe,
five warm-ups and 30 samples for each workload. Timers include complete page
preparation/publication but not LCD first paint. Other host builds were active;
tail timings are diagnostic, not CI speed thresholds or ESP32 evidence.

| Final-page BACK/reopen, microseconds | Baseline median / p95 / max | Candidate median / p95 / max |
|---|---:|---:|
| General quadratic | 1279.65 / 1798.70 / 1812.30 | 923.90 / 1207.20 / 1250.20 |
| Complex square | 1045.05 / 1672.90 / 2190.50 | 750.70 / 1009.80 / 1047.40 |
| Absolute affine | — | 777.50 / 954.30 / 1112.60 |
| Radical with extraneous candidate | — | 723.80 / 938.80 / 995.20 |
| Isolated coefficient around radical | — | 709.40 / 947.40 / 1057.60 |

Both existing warmed quadratic paths retain **zero polynomial-capture calls**
and a single proof build. Raw first preparations, repeated displays, revisits,
guided/summary changes and all nested counters are in `perf-baseline/` and
`perf-guarded/`; overlapping timing scopes are not summed. The earlier
`perf-final/` run remains available; these host variations are not attributed
as a page-speed improvement from the terminal guard.
First Steps-page / first final-page preparations on the candidate were
708.2/497.4 µs (absolute affine), 1033.6/369.4 µs (extraneous radical), and
724.5/365.1 µs (radical isolation). Each is a single first-use sample, not a
median or a cold-process comparison.

Fresh host ordinary-answer / tutor-generation medians (30 samples after five
warm-ups) are 125.30 / 2172.40 µs for `abs(2*x-3)=5`, 178.65 / 3808.80 µs for
`sqrt(x+1)=x-1`, and 117.65 / 1255.55 µs for `2sqrt(x+1)+1=7`. First-use
answer/proof pairs were respectively 495.70/2516.70, 450.40/3972.60 and
127.70/1236.10 µs; only the first fixture starts the process/context cold.
`generation-timing-guarded.log` and `timing-summary-guarded.json` retain all
samples and p95/max.

Xtensa ABI sizes, measured with the pinned production compiler:
Condition 40→56 bytes, Branch 16→20, Step 132→136, State unchanged at 40.
GCC `-fstack-usage` reports own frames of 1,056 bytes for the new compound
planner, 400 for lifted verification, 656 for the shared step verifier,
272 for isolation, 160 for preserving display evaluation, 128 for original
candidate checking, and 96/64 for structural replacement/search. Traversals
are depth-bounded; these are **own frames**, not total call-chain usage or
physical FreeRTOS stack high water. IRAM text remains unchanged.

| Linked resource, bytes | Immediate baseline | Final candidate | Delta |
|---|---:|---:|---:|
| Production normal flash | 5,486,773 | 5,517,225 | +30,452 |
| Production normal static RAM | 118,984 | 118,984 | 0 |
| Production normal IRAM text | 60,407 | 60,407 | 0 |
| CAM normal flash | 5,399,397 | 5,429,837 | +30,440 |
| CAM normal static RAM | 117,576 | 117,576 | 0 |
| CAM normal IRAM text | 59,039 | 59,039 | 0 |

The final ordinary production image is 5,517,584 bytes, SHA-256
`cf5c453dd2be72f69a8c3824beddcca5d1f41af9725894219704c060581ac243`.
The preserved baseline image is 5,487,136 bytes, SHA-256
`3197d7b05a0bafe88fdb3fdc3bdebf403840ffe144857bf00f99191cf6084efe`.
Neither image was installed during this task. Separate ASCII build snapshots
do not contain `.git`; source provenance comes from the external file/hash
manifests and unchanged repository HEAD, not an inferred embedded commit label.
Static-RAM invariance does not imply zero dynamic cost.

## Exact task file boundary

```text
docs/TUTOR_ENGINE_02A.md
scripts/benchmark-tutor-pages.py
scripts/profile-tutor-pages-native.py
scripts/test-math-notation-ui.py
scripts/test-tutor-engine.py
scripts/test-tutor-nonlinear-lifecycle.py
scripts/test-tutor-nonlinear.py
scripts/test-tutor-teaching-ui.py
scripts/tutor-teaching-web.mjs
src/apps/EquationsApp.cpp
src/apps/EquationsApp.h
src/apps/TutorPresentation.h
src/apps/TutorPresentation.inc
src/apps/TutorStepsView.inc
src/hal/NativeHal.cpp
src/math/giac/GiacEngine.cpp
src/math/giac/GiacTutor.inc
src/math/giac/GiacTutorNonlinear.inc
src/math/giac/GiacTutorNonlinearChecks.inc
src/math/giac/GiacTutorNonlinearPlanner.inc
src/math/tutor/Derivation.h
src/math/tutor/Messages.inc
src/math/tutor/TeachingPlan.h
tests/fixtures/tutor-nonlinear-challenge.json
tests/host/tutor_nonlinear_allocation.cpp
tests/host/tutor_nonlinear_checks.cpp
tests/host/tutor_nonlinear_probe.cpp
```

## Physical closeout — 2026-09-14

The preceding sections describe the implementation-phase snapshot. This closeout
started at the same HEAD (`76b238e17237cc3e834350cc4d89867e9ec54f0e`)
with the accepted 27-file candidate, an empty index and an unrelated untracked
`.vscode/settings.json`. Full original source SHA-256:
`2a5783a849f46c4a0b7a82968ed3247877d55a5448a0d0fc1c0c25dc8faa3378`.
Read-verified binary-capable index/worktree patches, feature-file copies,
path/hash manifests and editor files remain under ignored
`out/tutor-engine-02a-closeout/preservation/`. Neither editor file was changed.

### Physical failure and narrow correction

The first ten board cycles passed mathematical/control assertions but FAILED
memory acceptance: repeated ordinary radical solves retained 164 bytes each
for `sqrt(x+1)=x-1`. The original run/logs remain in `physical/`; they are not
reported as a lifecycle pass. A host allocation probe isolated ordinary solving
from tutor generation: each solve retained 157 requested C++ payload bytes in
four allocations; repeated tutor generation after warm-up retained zero.

The vendored fractional-power solver parsed every fresh `c__N` temporary name
into the permanent lexer symbol map. Purging its context assumption did not
release that interned identifier. `ksolve.cc` now constructs the same fresh name
with the existing owning `identificateur` constructor. Counter progression,
assumptions, exact arithmetic and cleanup remain unchanged. This changes only
identifier ownership; provenance is recorded in `lib/giac/NUMOS_CHANGES.md`.

`tests/host/giac_radical_lifetime.cpp` checks eight real-domain fixtures, three
warm-ups and twenty repetitions each without resetting Giac: **160 PASS**.
The identical test linked to the preserved old vendor fails 100 checks,
detecting repeated growth. This measures requested C++ allocation payload,
not C/GMP allocations, allocator overhead or total physical heap peaks.
Compile/link commands and response-file identities are recorded in
`fixed-host-retest/commands.json`; the test uses the same production object
closure as `scripts/build-giac-host-harness.sh` and has its own `main`.

### Verification and presentation invariance

Fresh corrected-source gates: 180 existing traces; 140 seeded nonlinear and
11 challenge cases; 235 checker assertions / 23 rejected mutations; 240
persistent allocation-failure sites with recovery; 160 lifetime checks;
Giac 179, cross-app 14 and Calculus 56 assertions. All pass.
All 331 replay records are identical before/after, excluding only `trace.micros`.
States, branches, conditions, provenance, statuses, messages and operation
counters are compared rather than omitted.

Native teaching passes 54 sequences / 297 pages / 659 scrolled 320×240 frames.
All 297 page metadata records match except `micros`; all 659 content viewports
below the 24-pixel clock/header match pixel-for-pixel. The existing captures
and full contact sheets were inspected directly; no golden or mask was changed.
Notation passes 30 structural fixtures, MathEnginePhase and 2,624 formula checks.
The corrected 50-cycle fixed-pool native run restores 67 objects / 3 timers /
19,160 monitored payload bytes free after HOME; sampled minimum is 7,352 bytes.
Native and ordinary production builds pass. WASM-MATH Release and Debug,
web package/smoke and nonlinear teaching in Chromium/Firefox/WebKit pass.

The broad implementation-phase CAM/bring-up/demo results above remain
historical; they were not rerun for this ownership-only correction. There was
no renderer, display profile, CPU setting or LVGL pool change. Initial SCons
package initialization failed in a concurrent build; a sequential retry with
the same toolchain passed. Concurrent WASM Debug regression hit port 8793 in use
by Release; its sequential rerun passed. Original failure logs are retained.

### Automated board acceptance

This task received explicit application-flashing authorization. The intended
WROOM was rediscovered as USB 303A:1001, serial `44:B1:76:A7:B7:2C`, COM9.
Active app0 is at `0x10000`, capacity `0x640000`. Only application bytes were
written and independently verified. Bootloader, partition map and NVS prefix
remain byte-identical across every installation. No LittleFS formatting,
security/eFuse operation, storage provisioning or display change was performed.
An initial backup read stopped on a short USB packet before any write; the
retry verified the known current image first. All logs and recovery images remain.

Corrected probe: `70c890305af3b942555929f7984b709633ea98adfa44c92d225f09ef66944298`.
It uses the existing private, default-off acceptance overlay; none of its
instrumentation is included in the feature commit. Boot confirms USB, PSRAM,
320×240 LVGL and the unchanged SAFE 40 MHz write / 10 MHz read display profile.
LittleFS mounts; the pre-existing missing/unreadable/invalid variable-file
diagnostic remains explicit.

All six requested ABS/radical walkthroughs pass on the board, including visible
condition/formula references, correct branch ownership, rejected candidates
and complete accepted solution sets. `sqrt(x)=3` gives ordinary `x=9`; the old
linear gives `x=5`. An old quadratic and a repeated radical complete the ten
mixed cycles. Every board state fingerprint matches independent host replay;
183 guided formula/prose/provenance comparisons match native presentation.
Canonical production events cover TOOLBOX, LEFT/RIGHT, UP/DOWN, EXE modes,
VAR, BACK, edit/cancel, SHIFT/ALPHA, HOME and re-entry. Valid reopening keeps
the derivation build count at one; HOME releases it. No unexpected reset/crash
or stale proof was observed.

These are **automated physical** and **native visual** observations. No new
human LCD or switch-contact confirmation is claimed. Automated scrolling and
pan exercise production event handling; they do not establish perceived LCD
latency or the electrical reliability of each physical key.

### Board timing and memory

Times are instrumented function timings in milliseconds, with the engine kept
alive through the mixed sequence. First use of a fixture is not a cold boot of
Giac. Cached final reopening has three samples per fixture; p95 equals the
observed maximum at this sample count. Page time nests inside open time; do
not add them. Synchronous diagnostic refresh is separate from LCD perception.

| Fixture | Ordinary answer | Tutor generation | First Steps | Cached final median | p95/max |
|---|---:|---:|---:|---:|---:|
| abs-linear | 30.43 | 690.57 | 39.80 | 48.14 | 48.24 |
| abs-variable | 53.67 | 733.63 | 39.83 | 42.18 | 42.20 |
| abs-negative | 16.25 | 96.46 | 35.44 | 35.38 | 35.44 |
| radical-extraneous | 35.68 | 1272.82 | 39.37 | 42.17 | 42.19 |
| radical-linear | 33.77 | 1104.83 | 39.48 | 42.27 | 42.30 |
| radical-isolate | 28.64 | 496.87 | 39.37 | 41.83 | 41.84 |
| sqrt-ordinary | 24.21 | 211.94 | 39.22 | 42.22 | 42.23 |
| old-linear | 7.91 | 178.00 | 42.54 | 38.93 | 38.95 |
| old-quadratic | 48.29 | 463.22 | 59.68 | 57.64 | 57.68 |
| radical-repeat | 35.73 | 1273.52 | 39.40 | 42.14 | 42.17 |

After initial fixture use, 104 bytes remain retained in PSRAM (48 after
radical isolation and 56 after the old quadratic). This is reported, not called
zero heap cost. Three targeted repetitions of isolation/quadratic/radical,
without any Giac reset, show no further decline. The specific repeated radical
loss seen before the fix is absent. This is bounded-run evidence, not a proof
of universal allocation behavior. Post-warm HOME samples:

- Internal free/largest: 144,128 / 102,388 bytes.
- PSRAM free/largest: 8,226,055 / 8,126,452 bytes.
- LVGL monitored payload free: 41,240 bytes; 71 objects / 5 screens / 3 timers.
- Minimum sampled unused task stack: 51,904 bytes.

The pinned Xtensa port defines `StackType_t` as `uint8_t`; the high-water
figure is minimum unused bytes, not words multiplied by four. Heap/largest
block values are boundary samples, not total allocation peaks. LVGL monitor
payload and object counts have different scopes from the unchanged 64 KiB pool.
Raw per-page samples, min/max and timing CSVs are in `physical-fixed/`; warm
follow-up and the original failing run are retained separately.

### Restored ordinary image and commit boundary

Ordinary production image restored and verified:
`5a16628d463baec7bf3e214ff7e13389d47434851fed3d7b1745640a84911e8f`.
Image 5,517,648 bytes; linked flash 5,517,289 bytes; static RAM 118,984 bytes;
IRAM text 60,407 bytes. Ownership fix delta: image/flash +64 bytes, static
RAM +0, IRAM +0. Probe-only instrumentation adds 32 static bytes and is absent
from this restored ordinary image. Pinned build: PlatformIO Core 6.1.19,
espressif32 6.12.0, Arduino-ESP32 2.0.17 / IDF 4.4.7, Xtensa GCC
8.4.0+2021r2-patch5, esptool 4.9, LVGL 9.5. Host GCC 15.2; Emscripten 6.0.3.

Corrected runtime-source SHA-256:
`d348d09690845706b1cd2a9f7b5587a60b0ec20138a02f0e5ad70bcb8ebf8658`.
The ordinary-source build copy is hash-identical for runtime inputs. After
restoration, sqrt/linear/quadratic workflows pass ordinary main-loop-return
smoke checks. That binary has no mathematical probe telemetry: exact result
assertions belong to the matched probe plus host evidence. A later commit hash
does not retroactively change either tested binary identity.

The commit contains the 27 paths listed above plus these three closeout paths:

```text
lib/giac/NUMOS_CHANGES.md
lib/giac/src/ksolve.cc
tests/host/giac_radical_lifetime.cpp
```

No feature or teaching redesign was introduced. No push is performed. All
images, board backups, private overlays, snapshots and raw logs remain ignored.

### Remaining boundary

02A remains real-domain, one supported absolute value or principal square root,
with finite supported branch/candidate solvers. Complex abs/radical tutoring,
nested/multiple special functions and conditional infinite families remain
unsupported as described above. An additional diagnostic records a pre-existing
ordinary complex-policy limitation: `sqrt((x-1)^2)=3` returns only `4`; real policy
returns `-2,4`. This reproduces with the previous canonical-equality adapter as
well as both vendor versions; it was not introduced or changed by the closeout.
That separate complex solver limitation is not claimed fixed.
