#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later
"""Build a private observation copy of the real renderer against matching objects.

No probe symbols, overlay drawing, test parser, or reference engine enter the
firmware. Baseline mode uses the preserved input-repair source and objects;
--expect-check-failure preserves position checks for a later incremental fix.
"""
import argparse, hashlib, json, os, subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--source', type=Path, required=True)
    p.add_argument('--build', type=Path, required=True)
    p.add_argument('--lvgl', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    p.add_argument('--baseline', action='store_true')
    p.add_argument('--fixtures', type=Path, default=ROOT/'tests/fixtures/math-spacing.json')
    p.add_argument('--checks-source', type=Path, default=ROOT/'tests/host/math_spacing_checks.cpp')
    p.add_argument('--brackets', action='store_true', help='Enable bracket editor fixtures')
    p.add_argument('--expect-check-failure', action='store_true', help='Run all current-position checks against a preserved pre-fix renderer')
    a = p.parse_args()
    source, build, out = a.source.resolve(), a.build.resolve(), a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    text = (source/'src/ui/MathRenderer.cpp').read_text(encoding='utf-8')
    declarations = '''namespace vpam {
void mathSpacingDrawProbe(const MathNode*, int, int, const FontMetrics&);
void mathSpacingTextProbe(int, int, unsigned, const char*, int);
void mathSpacingGlyphProbe(int, int, unsigned, uint32_t, const lv_font_glyph_dsc_t&);
void mathSpacingCursorProbe(const NodeRow*, int, int, int, int, const FontMetrics&);
'''
    assert text.count('namespace vpam {') == 1
    text = text.replace('namespace vpam {', declarations)
    if a.brackets:
        text = text.replace('namespace vpam {', 'namespace vpam {\nvoid mathBracketPieceProbe(int,int,int,uint32_t,const lv_font_t*);',1)
        anchor = '            lv_draw_letter(layer, &dsc, &pos);\n        };'
        assert text.count(anchor) == 1
        text = text.replace(anchor,'            mathBracketPieceProbe(pos.x, pos.y, emSizePx, dsc.unicode, dsc.font);\n' + anchor)
    anchor = '    if (fm.scriptLevel >= 2) {\n        font = _fontScriptScript;'
    assert text.count(anchor) == 1
    text = text.replace(anchor, '    if (layer) mathSpacingDrawProbe(node, x, yBaseline, fm);\n' + anchor)
    anchor = '        p += step;\n    }\n}'
    assert text.count(anchor) == 1
    text = text.replace(anchor, '        p += step;\n    }\n    mathSpacingTextProbe(x, yBaseline, scriptLevel, renderText, penX - x);\n}')
    anchor = '            lv_draw_letter(layer, &dsc, &letterPos);\n            penX += glyph.adv_w;'
    assert text.count(anchor) == 1
    text = text.replace(anchor, '            mathSpacingGlyphProbe(penX, yBaseline, scriptLevel, dsc.unicode, glyph);\n' + anchor)
    anchor = '        _cursorX = static_cast<int16_t>(finder.result.x + offsetX);'
    if anchor in text:
        assert text.count(anchor) == 1
        text = text.replace(anchor, '        mathSpacingCursorProbe(cur.row, cur.index, finder.result.x, finder.result.yBaseline, offsetX, finder.result.fm);\n' + anchor)
    else:
        anchor = '    _cursorX = static_cast<int16_t>(rowOrigin + offsetX);'
        assert text.count(anchor) == 1
        text = text.replace(anchor, '    mathSpacingCursorProbe(cur.row, cur.index, rowOrigin, rowBaseline, offsetX, fm);\n' + anchor)
    private = out/'MathRenderer-probe.cpp'; private.write_text(text, encoding='utf-8')
    env = dict(os.environ, PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ.get('PATH', ''))
    compiler = 'g++'
    flags = ['-std=c++17', '-O1', '-ffunction-sections', '-fdata-sections', '-DNATIVE_SIM', '-DSDL_MAIN_HANDLED',
             '-DLV_CONF_INCLUDE_SIMPLE', '-DLV_USE_STDLIB_MALLOC=LV_STDLIB_CLIB',
             '-I'+str(source/'src/ui'), '-I'+str(source/'src'), '-I'+str(source), '-I'+str(a.lvgl.resolve())]
    if not a.baseline: flags += ['-DNUMOS_SPACING_POSITIONS']
    if a.brackets: flags += ['-DNUMOS_BRACKETS']
    records = []
    def run(name, cmd):
        result = subprocess.run(cmd, cwd=source, env=env, capture_output=True)
        (out/(name+'.log')).write_bytes(result.stdout+result.stderr)
        records.append(dict(name=name,command=cmd,exit=result.returncode))
        (out/'commands.json').write_text(json.dumps(records,indent=2))
        result.check_returncode()
    own = []
    for name, unit in [('probe', ROOT/'tests/host/math_spacing_probe.cpp'), ('renderer', private)]:
        obj = out/(name+'.o'); run(name+'-compile', [compiler,*flags,'-c',str(unit),'-o',str(obj)]); own.append(obj)
    objects = [x for x in (build/'src').rglob('*.o') if 'math' in x.parts or 'fonts' in x.parts or x.name in ['FileSystem.o','MathTypography.o']]
    libraries = [*build.rglob('liblvgl.a'), *build.rglob('libgiac.a'), *build.rglob('libtommath.a')]
    assert len(libraries)==3
    rsp = out/'probe.rsp'; rsp.write_text('\n'.join('"'+x.as_posix()+'"' for x in [*own,*objects,*libraries]))
    binary = out/'probe.exe'; run('link',[compiler,'@'+str(rsp),'-Wl,--gc-sections','-o',str(binary)])
    for fixture in json.loads(a.fixtures.read_text(encoding='utf-8')):
        run(fixture['id'], [str(binary), fixture['keys'], str(out/(fixture['id']+'.ppm'))])
    check_obj=out/'checks.o'
    run('checks-compile',[compiler,*flags,'-c',str(a.checks_source.resolve()),'-o',str(check_obj)])
    allocator=out/'MathAST-allocator.o'
    run('allocator-compile',[compiler,*flags,'-DNUMOS_MATH_AST_TEST_ALLOCATOR','-c',str(source/'src/math/MathAST.cpp'),'-o',str(allocator)])
    check_objects=[allocator if x.name=='MathAST.o' else x for x in objects]
    rsp=out/'checks.rsp';rsp.write_text('\n'.join('"'+x.as_posix()+'"' for x in [check_obj,own[1],*check_objects,*libraries]))
    check_binary=out/'checks.exe';run('checks-link',[compiler,'@'+str(rsp),'-Wl,--gc-sections','-o',str(check_binary)])
    result=subprocess.run([str(check_binary)],cwd=source,env=env,capture_output=True)
    (out/'checks.log').write_bytes(result.stdout+result.stderr)
    assert result.returncode==(1 if a.baseline or a.expect_check_failure else 0),result.stdout+result.stderr
    (out/'objects.json').write_text(json.dumps([dict(path=str(x),sha256=hashlib.sha256(x.read_bytes()).hexdigest()) for x in [*objects,*libraries]],indent=2))
    print('PASS',sum(r['name'] not in ['probe-compile','renderer-compile','link','checks-compile','allocator-compile','checks-link'] for r in records),'fixtures and independent checks')

if __name__ == '__main__': main()
