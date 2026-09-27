#!/usr/bin/env python3
"""Build a private, read-only Calculation observer from matching native objects.

Use an ASCII output path with the Windows MinGW toolchain. The generated
translation unit only logs existing fields; it adds no evaluation or input API.
Never link baseline headers to candidate objects (or conversely).
"""
from pathlib import Path
import argparse,json,subprocess,os,ctypes
p=argparse.ArgumentParser(description=__doc__)
for flag in ['source','build','compile-commands','out']:p.add_argument('--'+flag,type=Path,required=True)
a=p.parse_args();source=a.source.resolve();build=a.build.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
unit=(source/'src/apps/CalculationApp.cpp').read_text(encoding='utf-8');anchor='    if (expected == "dump") {';assert unit.count(anchor)==1
observe=r'''    if (expected == "dump") {
        std::printf("[RESULT-EXACT] %s\n[RESULT-APPROX] %s\n[RESULT-STATUS] %d\n",_exactText.c_str(),_approxText.c_str(),int(_lastStatus));
        std::printf("[FORMAT-CAPABILITIES]");
        for (unsigned i=0;i<unsigned(numos::CalculationFormat::Count);++i)
            if (formatAvailable(static_cast<numos::CalculationFormat>(i))) std::printf(" %u",i);
        std::printf("\n");
        if (_formatMenu) for (unsigned slot=0;slot<5;++slot) {
            auto* label=_formatMenuLabels[slot];
            if (!label || lv_obj_has_flag(_formatMenuRows[slot],LV_OBJ_FLAG_HIDDEN)) continue;
            const auto* font=lv_obj_get_style_text_font(label,LV_PART_MAIN);
            const auto* text=lv_label_get_text(label);
            std::printf("[FORMAT-LABEL] %s\n",text);
            // Private observer: check the actual label font, not a separate
            // font chosen by the test. Current EN/ES catalog uses UTF-8 Latin.
            for (const unsigned char* p=reinterpret_cast<const unsigned char*>(text);*p;) {
                uint32_t cp=*p++;
                if ((cp&0xe0)==0xc0 && *p) cp=((cp&0x1f)<<6)|(*p++&0x3f);
                lv_font_glyph_dsc_t glyph{};
                const bool found=lv_font_get_glyph_dsc(font,&glyph,cp,0);
                std::printf("[FORMAT-GLYPH] cp=%u present=%u\n",unsigned(cp),unsigned(found&&!glyph.is_placeholder));
            }
        }
        lv_area_t box{}, caret{};lv_obj_get_coords(_mathCanvas.obj(), &box);
        const bool cursorVisible=_mathCanvas.cursorBounds(caret);
        std::printf("[BREAK-GEOMETRY] canvas=%d,%d,%d,%d cursor=%d,%d,%d,%d editable=%d root=%d,%d resultMode=%d\n",box.x1,box.y1,box.x2,box.y2,caret.x1,caret.y1,caret.x2,caret.y2,cursorVisible,_rootRow->layout().width,_rootRow->layout().height(),int(_resultMode));
        if(_resultRow){lv_obj_get_coords(_resultCanvas.obj(),&box);std::printf("[BREAK-RESULT] canvas=%d,%d,%d,%d root=%d,%d\n%s",box.x1,box.y1,box.x2,box.y2,_resultRow->layout().width,_resultRow->layout().height(),vpam::dumpTree(_resultRow).c_str());}
'''
unit=unit.replace(anchor,observe);private=out/'CalculationApp-observer.cpp';private.write_text(unit,encoding='utf-8')
db=json.loads(a.compile_commands.read_text());entry=next(e for e in db if e['file'].endswith('CalculationApp.cpp'))
parse=ctypes.windll.shell32.CommandLineToArgvW;parse.argtypes=[ctypes.c_wchar_p,ctypes.POINTER(ctypes.c_int)];parse.restype=ctypes.POINTER(ctypes.c_wchar_p);n=ctypes.c_int();argv=parse(entry['command'],ctypes.byref(n));cmd=[argv[i] for i in range(n.value)];ctypes.windll.kernel32.LocalFree(argv)
obj=out/'CalculationApp.o';cmd[cmd.index('-o')+1]=str(obj);cmd[-1]=str(private);cmd.insert(1,'-I'+str(source/'src/apps'))
env=dict(os.environ,PATH='C:/mingw64/bin;C:/SDL2/x86_64-w64-mingw32/bin;'+os.environ['PATH'])
def run(name,cmd):
 r=subprocess.run(cmd,cwd=source,env=env,capture_output=True);(out/(name+'.log')).write_bytes(r.stdout+r.stderr);r.check_returncode()
run('compile',cmd)
objects=[obj if f.name=='CalculationApp.o' else f for f in (build/'src').rglob('*.o')];libraries=[*build.rglob('libgiac.a'),*build.rglob('libtommath.a'),*build.rglob('liblvgl.a')]
rsp=out/'link.rsp';rsp.write_text('\n'.join('"'+f.as_posix()+'"' for f in [*objects,*libraries]))
run('link',['C:/mingw64/bin/g++.exe','@'+str(rsp),'-LC:/SDL2/x86_64-w64-mingw32/lib','-lmingw32','-lSDL2main','-lSDL2','-Wl,--gc-sections','-o',str(out/'observer.exe')]);print('Private observer built',out/'observer.exe')
