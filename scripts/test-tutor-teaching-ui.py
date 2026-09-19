#!/usr/bin/env python3
"""Review complete native teaching pages, with dynamically measured scroll bounds."""
import argparse
import hashlib
import importlib.util
import json
import math
import os
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location("eq_guided_review", ROOT / "scripts/test-equations-rebuild.py")
eq = importlib.util.module_from_spec(spec)
spec.loader.exec_module(eq)
CASES = {
    "trig-sine": ["sin x RIGHT = 1 / 2 RIGHT"],
    "trig-affine": ["sin 2 x RIGHT = 1 / 2 RIGHT"],
    "trig-impossible": ["sin x RIGHT = 2"],
    "trig-impossible-negative": ["cos x RIGHT = 0 - 2"],
    "trig-cosine": ["cos x RIGHT = 1 / 2 RIGHT"],
    "trig-tangent": ["tan x RIGHT = 1"],
    "trig-tangent-affine": ["tan 3 x RIGHT = 1"],
    "trig-unfamiliar": ["sin 3 x - 1 RIGHT = 1 / 3 RIGHT"],
    "trig-sine-deg": ["sin x RIGHT = 1 / 2 RIGHT"],
    "trig-cosine-deg": ["cos x RIGHT = 1 / 2 RIGHT"],
    "trig-tangent-deg": ["tan x RIGHT = 1"],
    "trig-affine-deg": ["sin 2 x RIGHT = 1 / 2 RIGHT"],
    "trig-unfamiliar-deg": ["sin 3 x - 1 RIGHT = 1 / 3 RIGHT"],
    "exp-injective": ["SHIFT ln 2 x - 1 RIGHT = SHIFT ln 3 RIGHT"],
    "exp-common-base": ["2 ^ x RIGHT = 8"],
    "exp-affine-identity": ["2 ^ x + 0 RIGHT = 8"],
    "exp-negative": ["SHIFT ln x RIGHT = - 1"],
    "exp-isolate": ["2 SHIFT ln x RIGHT + 1 = 7"],
    "log-domain": ["ln x - 1 RIGHT = 2"],
    "log-isolate": ["3 ln x RIGHT - 6 = 0"],
    "log-injective": ["ln x RIGHT = ln 5 RIGHT"],
    "log-base": ["logbase 2 RIGHT x RIGHT = 3"],
    "abs-linear": ["SHIFT sqrt 2 x - 3 RIGHT = 5"],
    "abs-variable": ["SHIFT sqrt x - 1 RIGHT = x + 3"],
    "abs-negative": ["SHIFT sqrt x RIGHT = - 3"],
    "abs-four-roots": ["SHIFT sqrt x ^ 2 RIGHT - 5 RIGHT = 4"],
    "radical-extraneous": ["sqrt x + 1 RIGHT = x - 1"],
    "radical-linear": ["sqrt 2 x + 3 RIGHT = x"],
    "radical-isolate": ["2 sqrt x + 1 RIGHT + 1 = 7"],
    "radical-wide": ["sqrt 3 1 2 3 4 5 x + 2 1 2 3 4 5 RIGHT = x"],
    "notation-factor": ["2 x ^ 2 RIGHT + 3 x - 5 = 0"],
    "isolated": ["x = 1"],
    "reversed": ["1 = x"],
    "physical-negative": ["x = - 3"],
    "linear": ["3 * x + 5 = 2 0"],
    "both-sides": ["2 * x + 3 = x - 4"],
    "distribution": ["3 * ( 2 * x - 1 ) = 9"],
    "fraction": ["( x - 1 ) / 2 RIGHT + ( x + 1 ) / 3 RIGHT = 5"],
    "square": ["x ^ 2 RIGHT = 9"],
    "zero-product": ["x ^ 2 RIGHT - x = 0"],
    "factoring": ["x ^ 2 RIGHT - 5 * x + 6 = 0"],
    "quadratic": ["2 * x ^ 2 RIGHT + 3 * x - 4 = 0"],
    "no-real": ["x ^ 2 RIGHT + 1 = 0"],
    "rational": ["( x ^ 2 RIGHT - 1 ) / ( x - 1 ) RIGHT = 0"],
    "conditional": ["x / x RIGHT = 1"],
    "system": ["x + y = 3", "x - y = 1"],
    "dependent": ["x + y = 2", "2 * x + 2 * y = 4"],
    "inconsistent": ["x + y = 2", "2 * x + 2 * y = 5"],
    "system3": ["x + y + ALPHA y = 6", "x - y + ALPHA y = 2", "x + y - ALPHA y = 0"],
    "complex": ["x ^ 2 RIGHT + 1 = 0"],
    "quadratic-complex": ["2 * x ^ 2 RIGHT + 3 * x + 4 = 0"],
    "linear-unfamiliar": ["7 - 4 * x = 2 * x + 1"],
    "quadratic-unfamiliar": ["3 * x ^ 2 RIGHT + 2 * x - 2 = 0"],
    "quadratic-negative-b": ["2 * x ^ 2 RIGHT - 3 * x - 4 = 0"],
    "rational-unfamiliar": ["( ( 3 * x - 2 ) * ( x + 4 ) ) / ( x + 4 ) RIGHT = 0"],
    "system-unfamiliar": ["2 * x + 3 * y = 1 3", "5 * x - 2 * y = 4"],
    # Legacy tutor UI coverage retained by this complete-sequence successor.
    "negative": ["3 * x = - 9"],
    "isolated-negative": ["x = - 3"],
    "repeated": ["( x - 1 ) ^ 2 RIGHT = 0"],
}

def stable(d):
    return {k: ([{x: y for x, y in s.items() if x != "text"} for s in v] if k == "steps" else v)
            for k, v in d.items() if k not in ("micros",)}

def ast_nodes(node):
    """Inspect the existing typed display AST; this does not parse mathematics."""
    yield node
    for child in node["children"]:
        yield from ast_nodes(child)

def ast_text(node):
    children = [ast_text(child) for child in node["children"]]
    if node["type"] == 4:  # existing VPAM Fraction
        return "(" + children[0] + ")/(" + children[1] + ")"
    if node["type"] == 5:  # existing VPAM Power
        return "(" + children[0] + ")^(" + children[1] + ")"
    if node["type"] == 7:  # existing VPAM Paren
        return "(" + "".join(children) + ")"
    return node["text"] + "".join(children)

def scalar_text(text):
    # Only compare exact scalar spellings from typed coefficient nodes.
    return re.sub(r"[()\s]", "", text)

def render_contacts(out, records):
    from PIL import Image, ImageDraw
    for record in records:
        rows = record["pages"]
        width = max(len(row["frames"]) for row in rows) * 320
        sheet = Image.new("RGB", (width, len(rows) * 264), "#dddddd")
        draw = ImageDraw.Draw(sheet)
        for y, row in enumerate(rows):
            for x, name in enumerate(row["frames"]):
                frame = Image.open(out / (name + ".ppm"))
                assert frame.size == (320, 240)
                frame.save(out / (name + ".png"))
                draw.text((x * 320 + 4, y * 264 + 4), name, fill="black")
                sheet.paste(frame, (x * 320, y * 264 + 24))
        prefix = record["case"] + "-" + record["mode"]
        sheet.save(out / (prefix + "-contact.png"))
        for part, top in enumerate(range(0, sheet.height, 264 * 6)):
            sheet.crop((0, top, sheet.width, min(sheet.height, top + 264 * 6))).save(
                out / (prefix + f"-contact-part{part + 1}.png"))

def check_presentation(name, mode, trace, views):
    """Check evidence links and instructional content, independently of LVGL layout."""
    if name.startswith("abs-"):
        assert "abs(" in trace["authored"][0][0], (name, "physical absolute-value template was not entered")
    if name.startswith("radical-"):
        assert "sqrt(" in trace["authored"][0][0], (name, "principal-root template was not entered")
    states, steps = trace["states"], trace["steps"]
    equation_refs = []
    for page in views:
        assert 0 <= page["step"] <= page["lastStep"] < len(steps), (name, "invalid page step")
        assert len(page["formulas"]) <= 4, (name, "unbounded formula widgets")
        assert "[invalid message parameters]" not in json.dumps(page), (name, "invalid title, prose or formula caption")
        assert not any(marker in page["prose"] for marker in ("sqrt(", "^", "*", "+/-", "!=")), (name, "source math in prose")
        for formula in page["formulas"]:
            assert steps[formula["step"]]["verdict"] == 1, (name, "formula from unverified step")
            assert formula["ast"] and formula["ast"]["children"], (name, "missing structured formula")
            kind = formula["kind"]
            if kind == "equation":
                state = states[formula["state"]]
                branch = state["branches"][formula["branch"]]
                assert formula["row"] < len(branch["equations"])
                equation_refs.append((formula["state"], formula["branch"], formula["row"]))
                if page["kind"] == "final":
                    assert not branch["rejected"], (name, "excluded candidate displayed as final solution")
            elif kind == "authored":
                assert formula["row"] < len(trace["authored"])
            elif kind == "conditions":
                conditions = states[formula["state"]]["conditions"]
                assert conditions
                typed = states[formula["state"]].get("typedConditions", [{"kind": 0} for _ in conditions])
                for kinds, symbol in [((0, 3), "≠"), ((1,), "≥"), ((2,), ">")]:
                    assert sum(n["text"] == symbol for n in ast_nodes(formula["ast"])) == sum(c["kind"] in kinds for c in typed), (name, "condition missing structured relation")
            elif kind in ("periodic_family", "family_operation"):
                family = states[formula["state"]]["families"][formula["branch"]]
                assert family["binderId"] == 1 and family["domain"] == 0
                assert any(n["text"] == "k" for n in ast_nodes(formula["ast"]))
                if kind == "family_operation": assert formula["state"] == steps[formula["step"]]["before"]
            elif kind == "integer_parameter":
                assert "k" in ast_text(formula["ast"]) and "\u2208" in ast_text(formula["ast"]) and "\u2124" in ast_text(formula["ast"])
            elif kind == "trig_principal":
                assert steps[formula["step"]]["rule"] == "trig.principal"
            elif kind == "trig_range":
                assert steps[formula["step"]]["rule"] in ("trig.range", "trig.impossible")
                if steps[formula["step"]]["rule"] == "trig.impossible":
                    values=[n['text'] for n in ast_nodes(formula['ast'])]
                    assert ('>' in values or '<' in values) and '\u2264' not in values, (name,'false range inequality displayed')
            elif kind == "solution_set":
                assert page["kind"] == "final" and states[formula["state"]]["conclusion"] == 1
                accepted = [b for b in states[formula["state"]]["branches"] if b.get("status", 0) == 0]
                assert sum(n["text"] == "=" for n in ast_nodes(formula["ast"])) == len(accepted)
            elif kind in ("operand", "row_operation"):
                assert steps[formula["step"]]["operand"] or steps[formula["step"]]["rule"] == "system.swap"
            elif kind == "balanced_operation":
                step = steps[formula["step"]]
                assert step["rule"] in ("equation.add", "equation.divide")
                assert formula["state"] == step["before"] and formula["branch"] == step["branch"]
                tree = formula["ast"]
                while len(tree["children"]) == 1:
                    tree = tree["children"][0]
                left, eq, right = tree["children"]
                assert eq["text"] == "=", (name, "operation lost equality")
                if step["rule"] == "equation.divide":
                    assert left["type"] == right["type"] == 4, (name, "division not shown on both sides")
                    assert scalar_text(ast_text(left["children"][1])) == scalar_text(step["operand"])
                    assert left["children"][1] == right["children"][1], (name, "different divisors")
                else:
                    assert len(left["children"]) == len(right["children"]) == 3
                    assert left["children"][1:] == right["children"][1:], (name, "unequal balancing operations")
                    sign, amount = left["children"][1:]
                    assert sign["text"] in ("+", "-", "\u2212")
                    if re.fullmatch(r"-?\d+", step["operand"]):
                        negative = int(step["operand"]) < 0
                        assert (sign["text"] != "+") == negative, (name, "wrong operation sign")
                        assert int(scalar_text(ast_text(amount))) == abs(int(step["operand"]))
            else:
                step = steps[formula["step"]]
                assert step["rule"] == "quadratic.formula" and len(step["auxiliaries"]) == 4, (name, "quadratic facts lack checked source")
                a, b, c, disc = step["auxiliaries"]
                if kind == "coefficients":
                    assert scalar_text(ast_text(formula["ast"])) == scalar_text(f"a={a},b={b},c={c}"), (name, "coefficient values differ from checked auxiliaries")
                if kind == "discriminant_values":
                    assert scalar_text(ast_text(formula["ast"])).endswith("=" + scalar_text(disc)), (name, "wrong displayed discriminant")
                    if b.startswith("-"):
                        powers = [n for n in ast_nodes(formula["ast"]) if n["type"] == 5 and scalar_text(ast_text(n["children"][0])) == scalar_text(b)]
                        assert powers and all(n["children"][0]["type"] == 7 for n in powers), (name, "negative squared coefficient lost parentheses")
                    if c.startswith("-"):
                        assert any(n["type"] == 7 and scalar_text(ast_text(n)) == scalar_text(c) for n in ast_nodes(formula["ast"])), (name, "negative product coefficient lost parentheses")
                if kind in ("general_formula", "substituted_formula"):
                    assert any(n["text"] == "±" for n in ast_nodes(formula["ast"])), (name, "missing plus-minus AST")
                    assert any(n["type"] == 4 for n in ast_nodes(formula["ast"]))
                    assert any(n["type"] == 6 for n in ast_nodes(formula["ast"]))
                    # The glyph pixels are separately captured and reviewed:
                    # an AST assertion alone cannot establish visible ± ink.
                if kind == "substituted_formula":
                    roots = [n for n in ast_nodes(formula["ast"]) if n["type"] == 6]
                    assert any(scalar_text(ast_text(n)) == scalar_text(disc) for n in roots), (name, "substitution lost actual discriminant")
        final = states[steps[page["lastStep"]]["after"]]
        if page["kind"] == "transition" and steps[page["step"]]["rule"] in ("equation.add", "equation.divide"):
            assert sum(f["kind"] == "balanced_operation" for f in page["formulas"]) == 1, (name, "balancing arithmetic hidden")
            assert not any(f["kind"] == "operand" for f in page["formulas"]), (name, "detached operand tile")
            assert "Amount used" not in str(page["formulas"])
        if page["kind"] == "final":
            if final["conclusion"] == 5:
                families = [f for f in page["formulas"] if f["kind"] == "periodic_family"]
                assert len(families) == len(final["families"]), "missing final family"
                assert any(f["kind"] == "integer_parameter" for f in page["formulas"])
            if page["step"] != page["lastStep"]:
                assert steps[page["step"]]["rule"] == "quadratic.formula"
                assert steps[page["lastStep"]]["rule"] == "terminal.finish"
                assert steps[page["lastStep"]]["text"] in page["prose"], (name, "lost terminal explanation")
                assert all(f["step"] == page["lastStep"] for f in page["formulas"]), (name, "lost terminal formula provenance")
            shown = {(f["state"], f["branch"], f["row"]) for f in page["formulas"] if f["kind"] == "equation"}
            if final["conclusion"] in (1, 3, 4):
                expected = {(steps[page["lastStep"]]["after"], bi, ri)
                            for bi, branch in enumerate(final["branches"]) if not branch["rejected"] and branch.get("status", 0) == 0
                            for ri in range(len(branch["equations"]))}
                if any(f["kind"] == "solution_set" for f in page["formulas"]):
                    assert not shown and len(expected) > 3
                else:
                    assert shown == expected, (name, "final omitted accepted solution or system relationship")
            if final["conditions"]:
                assert any(f["kind"] == "conditions" and f["state"] == steps[page["lastStep"]]["after"] for f in page["formulas"]), (name, "final omitted original restrictions")
        if page["kind"] == "coefficients" and states[steps[page["step"]]["before"]]["branches"][0]["equations"][0][1] != "0":
            assert any(f["kind"] == "coefficient_equation" for f in page["formulas"]), (name, "coefficient normalization bridge missing")
    if name == "linear":
        displayed = [states[s]["branches"][b]["equations"][r] for s, b, r in equation_refs]
        assert ["3*x", "15"] in displayed, (mode, "linear intermediate hidden")
        assert ["x", "5"] in displayed
    if name in ("quadratic", "quadratic-complex", "quadratic-negative-b", "quadratic-unfamiliar"):
        assert len(views) == 4 and views[-1]["kind"] == "final", (name, "redundant quadratic conclusion")
        assert not any(v["kind"] == "roots" for v in views), (name, "duplicate evaluated roots page")
    if name in ("isolated", "physical-negative", "isolated-negative"):
        assert len(views) == 1, (name, "artificial isolated-equation operations")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--bin", default="C:/.piobuild/numOS/emulator_pc/program.exe")
    parser.add_argument("--out", type=Path, default=Path("out/tutor-teaching-ux-01/ui"))
    parser.add_argument("--cases", nargs="*")
    parser.add_argument("--render-only", action="store_true", help="Render existing framebuffer evidence; do not rerun or report tests")
    args = parser.parse_args()
    binary = Path(args.bin).resolve()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    if args.render_only:
        render_contacts(out, json.loads((out / "results.json").read_text(encoding="utf-8")))
        return
    os.chdir(ROOT)
    env = dict(os.environ, NUMOS_EQUATIONS_BOUNDS="1")
    dll = eq.helper.sdl2_dll_dir(str(binary))
    if dll:
        env["PATH"] = dll + os.pathsep + env.get("PATH", "")

    def path_arg(path):
        try:
            return path.relative_to(ROOT).as_posix()
        except ValueError:
            return path.as_posix()
    mapping = {}
    data = (ROOT / "src/input/generated/ProductionKeypadMap.generated.h").read_text(encoding="utf-8")
    data = data.split("kProductionKeypadMap = {{", 1)[1].split("}};", 1)[0]
    for line in data.splitlines():
        match = re.search(r"\{(\d+), (\d+),.*KeyCode::(\w+),", line)
        if match:
            mapping[match[3]] = (match[1], match[2])

    def physical(*codes):
        return "".join("equations_physical " + " ".join(mapping[c]) + "\n" for c in codes)

    def run(name, script):
        script += "log TEACHING_GUIDED_COMPLETE\n"
        path = out / (name + ".numos")
        path.write_text(script, encoding="utf-8")
        frames = sum(int(line.split()[1]) if line.startswith("wait ") else 1 for line in script.splitlines()) + 100
        proc = subprocess.run([str(binary), "--headless", "--deterministic", "--quiet", "--frames", str(frames),
                               "--script", path_arg(path)], cwd=ROOT, env=env,
                              stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=180)
        (out / (name + ".log")).write_bytes(proc.stdout)
        if proc.returncode or b"TEACHING_GUIDED_COMPLETE" not in proc.stdout:
            raise RuntimeError((name, proc.returncode, proc.stdout[-3000:].decode(errors="replace")))
        rows = proc.stdout.decode(errors="replace").splitlines()
        return {key: [json.loads(line.split(prefix, 1)[1]) for line in rows if prefix in line]
                for key, prefix in [("views", "[TUTOR_VIEW] "), ("traces", "[TUTOR_TRACE] ")]}

    def shot(name):
        return ("wait 4\nassert_equations view bounded\nassert_equations trace formulas\n"
                "assert_equations view dump\nscreenshot " + path_arg(out / (name + ".ppm")) + "\n")

    records = []
    english_traces = {}
    pan_evidence = None
    for name, equations in CASES.items():
        if args.cases and name not in args.cases:
            continue
        start = (eq.single(equations[0]) if len(equations) == 1 else eq.system(equations)) + eq.keys("tools")
        if name.startswith("abs-"):
            # Legacy script key aliases do not carry the production semantic
            # modifier event. Exercise the actual SHIFT + square-root contacts.
            start = start.replace(eq.keys("SHIFT sqrt"), physical("SHIFT", "SQRT"))
        if name.startswith("exp-"):
            start = start.replace(eq.keys("SHIFT ln"), physical("SHIFT", "LN"))
        if name == "physical-negative":
            start = eq.OPEN + physical("EXE", "EXE", "VAR_X", "EQUAL", "NEGATE", "NUM_3", "EXE", "DOWN", "DOWN", "EXE", "TOOLBOX")
        if name in ("complex", "quadratic-complex"):
            start = start.replace("policy real", "policy complex")
        if name.startswith("trig-"):
            start = "set_angle_mode " + ("deg" if name.endswith("-deg") else "rad") + "\n" + start
        modes = [("guided", "")]
        if name in ("linear", "quadratic", "rational", "system", "linear-unfamiliar", "abs-variable", "radical-extraneous", "exp-injective", "log-domain", "exp-isolate", "log-isolate", "trig-unfamiliar", "trig-unfamiliar-deg"):
            modes.append(("summary", eq.keys("EXE")))
        if name in ("linear", "abs-variable", "radical-extraneous", "log-domain", "exp-isolate", "log-isolate", "trig-unfamiliar", "trig-unfamiliar-deg"):
            modes.extend([(locale, "assert_equations locale " + locale + "\n") for locale in ("es", "fr", "pseudo")])
        if name == "quadratic":
            modes.append(("pseudo", "assert_equations locale pseudo\n"))
        for mode, switch in modes:
            label = name + "-" + mode
            begin = start + switch
            found = run(label + "-discover", begin + "assert_equations trace complete\nassert_equations trace check\nassert_equations trace dump\nassert_equations view dump\n")
            count = found["views"][0]["count"]
            trace = found["traces"][0]
            if mode == "guided":
                english_traces[name] = stable(trace)
            else:
                assert stable(trace) == english_traces[name], (name, mode, "language/mode changed derivation")
            script = begin
            for page in range(count):
                script += f"assert_equations view page {page}\nassert_equations view dump\n" + eq.keys("RIGHT")
            metadata = run(label + "-metadata", script)["views"]
            assert len(metadata) == count
            check_presentation(name, mode, trace, metadata)
            script = begin + "assert_equations trace dump\n"
            captures = []
            for page, view in enumerate(metadata):
                script += f"assert_equations view page {page}\n"
                max_scroll = view["maxScroll"]
                names = []
                for scroll in range(math.ceil(max_scroll / 84) + 1):
                    frame = f"{label}-{page:02}-scroll{scroll}"
                    names.append(frame)
                    script += (eq.keys("DOWN DOWN DOWN") if scroll else "") + shot(frame)
                script += eq.keys("DOWN " * 12) + "assert_equations view bounded\nassert_equations view dump\n" + eq.keys("RIGHT")
                captures.append({"page": page, "kind": view["kind"], "metadata": view, "frames": names})
            script += "assert_equations trace dump\n" + eq.keys("EXE") + "assert_equations trace dump\n" + eq.keys("EXE")
            script += "assert_equations trace dump\nassert_equations trace check\nassert_equations trace builds 1\n"
            if mode == "guided":
                # Retain legacy open/close/cache/HOME coverage on every family.
                for _ in range(12):
                    script += eq.keys("BACK tools") + "assert_equations trace builds 1\n"
                script += eq.keys("BACK BACK") + "assert_equations epochs current\n"
                script += eq.keys("HOME") + "wait 30\nassert_equations closed\nopen_app Equations\nwait 30\nassert_equations count 0\nassert_equations trace builds 0\n"
            result = run(label + "-complete", script)
            assert all(stable(t) == stable(result["traces"][0]) for t in result["traces"])
            cursor = 0
            for page in captures:
                views = result["views"][cursor:cursor + len(page["frames"]) + 1]
                assert len(views) == len(page["frames"]) + 1
                assert views[0]["scrollY"] == 0, (label, page["page"], "new page did not start at top")
                assert views[-1]["scrollY"] == views[-1]["maxScroll"], (label, page["page"], "bottom not reached")
                assert all(v["page"] == page["page"] for v in views), (label, "scroll unexpectedly changed page")
                cursor += len(views)
            assert cursor == len(result["views"])
            (out / (label + "-trace.json")).write_text(json.dumps(trace, indent=2), encoding="utf-8")
            (out / (label + "-views.json")).write_text(json.dumps(result["views"], indent=2), encoding="utf-8")
            records.append({"case": name, "mode": mode, "page_count": count, "pages": captures, "pass": True})
            (out / "results.json").write_text(json.dumps(records, indent=2), encoding="utf-8")
            print(label, count, "pages", flush=True)
    if not args.cases or any(n.startswith('trig-') for n in args.cases):
        script='set_angle_mode rad\n'+eq.single(CASES['trig-sine'][0])+eq.keys('tools BACK')
        script+='set_angle_mode deg\nassert_equations trace stale\n'+eq.keys('tools')
        script+='assert_equations trace hidden\nassert_equations trace builds 1\n'+eq.keys('BACK BACK ENTER')
        script+='assert_equations trace complete\nassert_equations trace builds 2\n'+eq.keys('tools')
        script+='assert_equations trace formulas\nassert_equations trace dump\n'+eq.keys('HOME')
        changed=run('angle-change-reopen',script)
        assert changed['traces'][-1]['degrees'] == 1
        # Reach both ends of a wide exact periodic family without rebuilding
        # its proof. Use the existing VAR pan contract and unchanged viewport.
        script='set_angle_mode deg\n'+eq.single(CASES['trig-unfamiliar-deg'][0])+eq.keys('tools RIGHT RIGHT RIGHT RIGHT RIGHT DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN DOWN')
        script+='assert_equations trace dump\n'
        for i in range(33):
            script+=(eq.keys('VAR') if i else '')+shot(f'trig-pan-{i:02}')
        script+='assert_equations trace dump\nassert_equations trace builds 1\n'
        panned=run('trig-wide-pan',script)
        assert stable(panned['traces'][0])==stable(panned['traces'][1])
        from PIL import Image
        pixels=[Image.open(out/f'trig-pan-{i:02}.ppm').crop((0,54,320,220)).tobytes() for i in range(33)]
        assert any(p!=pixels[0] for p in pixels[1:]) and pixels[0] in pixels[1:]
    from PIL import Image, ImageDraw
    if not args.cases or 'radical-wide' in args.cases:
        script=eq.single(CASES['radical-wide'][0])+eq.keys('tools RIGHT RIGHT RIGHT RIGHT RIGHT')
        script+='assert_equations trace complete\nassert_equations trace dump\n'
        for i in range(33):
            script+=(eq.keys('VAR') if i else '')+'assert_equations view page 5\n'+shot(f'radical-pan-{i:02}')
        script+='assert_equations trace dump\nassert_equations trace builds 1\n'
        result=run('radical-pan',script)
        assert stable(result['traces'][0])==stable(result['traces'][1])
        pixels=[]
        for i in range(33):
            frame=Image.open(out/f'radical-pan-{i:02}.ppm');frame.save(out/f'radical-pan-{i:02}.png')
            pixels.append(frame.crop((0,54,320,220)).tobytes())
        assert any(p!=pixels[0] for p in pixels[1:]),'radical formula did not pan'
        assert pixels[0] in pixels[1:],'radical pan did not return'
        (out/'radical-pan-result.json').write_text(json.dumps({'pass':True,'frames':33,'return_at':pixels[1:].index(pixels[0])+1}))
    if not args.cases or "wide-pan" in args.cases:
        wide = "( ( 3 1 2 3 4 5 * x - 2 1 2 3 4 5 ) * ( x + 4 1 2 3 4 5 ) ) / ( x + 4 1 2 3 4 5 ) RIGHT = 0"
        script = eq.single(wide) + eq.keys("tools") + "assert_equations trace complete\nassert_equations trace dump\n"
        pan_names = []
        for i in range(33):
            name = f"wide-pan-{i:02}"
            pan_names.append(name)
            script += (eq.keys("VAR") if i else "") + "assert_equations view page 0\n" + shot(name)
        script += "assert_equations trace dump\nassert_equations trace builds 1\n"
        result = run("wide-pan", script)
        assert stable(result["traces"][0]) == stable(result["traces"][1])
        pixels = []
        for name in pan_names:
            frame = Image.open(out / (name + ".ppm"))
            frame.save(out / (name + ".png"))
            # Exclude the clock; inspect the unchanged actual content viewport.
            pixels.append(frame.crop((0, 54, 320, 220)).tobytes())
        assert any(p != pixels[0] for p in pixels[1:]), "VAR did not pan the wide formula"
        restored = next((i for i in range(1, len(pixels)) if pixels[i] == pixels[0]), None)
        assert restored is not None, "VAR cannot recover the starting viewport"
        pan_evidence = {"frames": pan_names, "restored_at": restored, "pass": True}
        (out / "wide-pan-results.json").write_text(json.dumps(pan_evidence, indent=2), encoding="utf-8")
    render_contacts(out, records)
    (out / "manifest.json").write_text(json.dumps({"binary": str(binary),
        "binary_sha256": hashlib.sha256(binary.read_bytes()).hexdigest(),
        "sequences": len(records), "pages": sum(r["page_count"] for r in records),
        "frames": sum(len(p["frames"]) for r in records for p in r["pages"]),
        "horizontal_pan": pan_evidence,
        "visual_review_required": "The captured plus-minus glyph ink and complete teaching sequences require visual review; AST assertions alone do not establish readable pixels."
    }, indent=2), encoding="utf-8")

if __name__ == "__main__":
    main()
