# Giac/KhiCAS provenance and NumOS changes

This directory is a modified vendored snapshot of Giac/KhiCAS. It is not
upstream Giac 1.9.0, as the former `library.json` value suggested. Embedded
metadata in `src/config.h` reports Giac 1.4.9 (`VERSION` is `1.4.9-57`).

## Best-established upstream

The closest public source located by this audit is the KhiCAS
`ti-ce-giac` repository at commit
`8d24f392f3edcb4fbf44b11325e92ca37edee470` (2026-02-24, "revert to giac
printing for double"). At NumOS import commit `a52832ed` (2026-04-09), 144
paths were shared with that snapshot and 126 were byte-identical. Eighteen
shared files already contained differences and nine compatibility files had
no counterpart. The public snapshot is therefore the strongest reproducible
baseline found, not a claim of exact archive identity.

The precise download URL, original archive checksum, and authorship/date of
the 18 pre-import changes are not recoverable from this repository's history.
Those facts remain explicitly unresolved; no version or author was guessed.

Giac and these modifications are distributed under GPL-3.0-or-later. The
complete GPLv3 text is at [`../../LICENSE-SOFTWARE`](../../LICENSE-SOFTWARE).
Modified upstream files carry a short notice referring here. Files added for
the port carry an equivalent added-file notice.

## File classification

Compared with the baseline above, these 18 shared files were already modified
when the full tree entered NumOS on 2026-04-09:

`config.h`, `debug.h`, `first.h`, `gen.h`, `giacPCH.h`, `global.h`,
`gmp_replacements.h`, `identificateur.h`, `input_lexer.cc`, `kgen.cc`,
`kglobal.cc`, `kidentificateur.cc`, `kifactor.cc`, `kmaple.cc`, `memmgr.h`,
`monomial.h`, `myostream.h`, and `poly.h`.

These 17 shared files were byte-identical at import and first acquired
functional NumOS differences after import:

`input_lexer.h`, `input_parser.cc`, `kgausspol.cc`, `kintg.cc`, `kintgab.cc`,
`kmisc.cc`, `kmodfactor.cc`, `kmodpoly.cc`, `kplot.cc`, `kprog.cc`, `krpn.cc`,
`kseries.cc`, `ksolve.cc`, `ksubst.cc`, `kthreaded.cc`, `kusual.cc`, and
`usual.h`.

Eight of the 18 already-divergent files were also changed again after import:

`first.h`, `gen.h`, `global.h`, `input_lexer.cc`, `kgen.cc`, `kglobal.cc`,
`kidentificateur.cc`, and `kmaple.cc`.

These nine port/integration files have no path in the `ti-ce-giac` snapshot:

`console.h`, `k_csdk.h`, `keypadc.h`, `main.h`, `menuGUI.h`,
`platform_stubs.cpp`, `sys/rtc.h`, `textGUI.h`, and `umap.h`.

"Local-only" here means absent from `ti-ce-giac`; it is not an authorship
claim. Five shims (`console.h`, `k_csdk.h`, `main.h`, `menuGUI.h`, and
`textGUI.h`) implement interfaces also exposed by the sibling KhiCAS
`ti-ce` front end. They are not byte copies of those files, but their API
names and compatibility purpose derive from that integration surface. The
other four are NumOS-side platform/build shims. All nine are treated
conservatively as GPL-3.0-or-later integration code.

The classifications describe functional differences before the uniform 2026
licensing notices were added. They are disjoint by state at import: 18
already-divergent shared files, 17 shared files that first diverged later,
and nine paths absent from `ti-ce-giac`. The eight-file list above records
later work inside the first category and therefore is intentionally not a
fourth disjoint category.

Generated `input_lexer.cc` and `input_parser.cc` are authoritative vendored
build inputs here and contain NumOS changes. Their referenced
`input_lexer.ll` and `input_parser.yy` inputs are absent both from this vendor
snapshot and from the identified `ti-ce-giac` baseline; this repository has
no Flex/Bison regeneration workflow for them. Replacing either generated
file from another source would require reapplying both the port changes and
the top-of-file modification notice. The `.bak` files are unmodified
baseline artifacts and are not used as regeneration inputs.

## Change history reconstructed from Git

TUTOR-ENGINE-02A closeout (2026-09-14): in the fractional-power solver's
auxiliary-variable loop, `ksolve.cc` constructs the generated `c__N` directly
as an `identificateur` instead of parsing its name. Parsing interned every
new name in the permanent lexer symbol table; `purgenoassume` removed its
context assumption but could not release that interned identifier. Repeated
ordinary radical solves retained 164 PSRAM bytes per call on the WROOM
candidate (157 requested C++ bytes / four allocations in the host probe).
The change keeps the same counter, identifier spelling, equations, assumptions
and cleanup; it changes ownership only. No counter/context reset is used.
`tests/host/giac_radical_lifetime.cpp` detects the old repeated growth, and the
closeout report records the physical failure, correction and retest. No
upstream code or language resources were imported for this correction.

TUTOR-ENGINE-01 (2026-09-10, local candidate): `kgen.cc` now obtains the
complex display flag through `offsetof(ref_complex,re)` and the actual
`display` field. Subtracting one `int` from `re` read padding on LLP64,
where the reference count is 64 bits; repeated native exact complex results
randomly printed polar syntax. The explicit field address is unchanged in
meaning on ESP32/WASM. No upstream code or language resources were imported.

| NumOS commit | Date | Relevant change |
| --- | --- | --- |
| `070630be` | 2026-04-07 | Initial Giac integration metadata and `umap.h`. |
| `a52832ed` | 2026-04-09 | Full vendored tree imported with port work already present. |
| `0aa2ef3b` | 2026-04-09 | First portability, plotting, program, substitution, and stub changes. |
| `d5399b1a` | 2026-04-09 | ESP32 compile port and build integration. |
| `ad5cdb3e` | 2026-04-10 | `kgen.cc` factor-related changes. |
| `13f5aaca` | 2026-04-10 | Lexer, global/program, and platform-stub changes. |
| `3b04f2aa` | 2026-04-11 | Algebra, calculus, factorization, series, and solve porting. |
| `b479e79d` | 2026-04-12 | Infinity/constants and limits/series work. |
| `321aac87` | 2026-04-12 | Geometry/limits work and LibTomMath integration. |
| `f4157aa0` | 2026-04-12 | Further series/limit handling. |
| `a39a887b` | 2026-04-14 | Final main port pass: value representation, parser, integration, infinity, RPN, and solve changes. |
| `43296361` | 2026-07-13 | Native build seam and `extra_script.py`. |
| `36cb67eb` | 2026-07-24 | SDL2/WebAssembly integration. |

The functional changes fall into five broad groups: ESP32/native portability
and stubs; `DOUBLEVAL`/pointer representation; lexer/parser treatment of
infinity; symbolic limits, integration, summation, solve, and factorization;
and PlatformIO/native build integration. Consult `git show <commit> --
lib/giac` for the authoritative patch at each step.

## Reproducing the comparison

```sh
git clone https://github.com/KhiCAS/ti-ce-giac.git
git -C ti-ce-giac checkout 8d24f392f3edcb4fbf44b11325e92ca37edee470
git diff --no-index -- ti-ce-giac lib/giac/src
git log --follow -- lib/giac/src/<file>
```

Directory-wide diffs include upstream files intentionally omitted from the
embedded subset. Compare shared paths or blob hashes when reproducing the
126-of-144 import result.

## TUTOR-ENGINE-02C: exact trig constants and degree conversions

The pinned `kusual.cc` static `cst_two_pi`, `cst_pi_over_2` and
`cst_inv_pi` owned values referenced the dynamically initialized `cst_pi`
in another translation unit. In the tested native link order, the first two
were initialized from zero: `acos(1/2)` became `-pi/6`, and periodic solve
lost full-turn terms. Build those three values directly from the existing
function-local `_IDNT_pi()` identifier, avoiding cross-TU initialization order.

The same embedded branch retained alias-layout casts for `rad2deg_e` and
`deg2rad_e`, including raw function addresses. The direct public DEG solve
probe crashed. Replace just those two references with owned symbolic products,
matching the already-established owned-value portability correction in this
vendor snapshot. No solve algorithm or tutor answer injection is added.

Reproducer: `tests/host/tutor_trig_probe.cpp`; pre/post evidence under ignored
`out/tutor-engine-02c/representation-*.log`. Post-fix public and adapter paths
agree on pi/3 for acos(1/2), 60 in DEG; all-period public solutions contain the
appropriate 2*pi/360 or pi/180 terms. GPL-3.0-or-later provenance is unchanged.

## TUTOR-ENGINE-03A: scoped periodic parameter provenance

`numos_periodic_solve` wraps the existing public `_solve` entry. Only during
that call, the three real sine/cosine/tangent isolation producers report the
fresh identifier that they construct as their integer parameter. The wrapper
quotes it before use (without assigning or purging user values) and restores
the prior quoted-variable list and both legacy global solve counters on every
exit, including C++ unwinding. A near-overflow counter declines the request.
Ordinary unwrapped Giac operations have no observer and retain their behavior.

The integer domain comes from these specific producers' all-solution formulas,
not the spelling `n_N`: this port has no typed integer annotation on the returned
identifier. No solve algorithm, inverse value, period, or branch construction
is changed. NumOS admits only one real sin/cos/tan with a nonconstant affine
argument and an authored, domain-safe rational constant target, then checks
that every output is affine in the recorded parameter. Parity/non-affine or
unobserved parameter output declines complete conversion.

The wrapper is serialized by the existing GiacEngine call guard, not a new
context or a thread-safe vendor API. `all_trig_sol`, angle and complex flags
are scoped separately by the engine. `tests/host/giac_periodic_probe.cpp` and
`tests/host/periodic_results_checks.cpp` cover the producer, collisions,
restoration, complete branch conversion and exact set comparison. No external
source was imported; GPL-3.0-or-later provenance is unchanged.
