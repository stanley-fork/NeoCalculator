#!/usr/bin/env python3
"""Package review candidates with original pixels and self-contained HTML."""
import html, json, re, shutil, zipfile
from pathlib import Path
from PIL import Image

root=Path(__file__).resolve().parents[1]
out=root/'out/calculation-format-01';dest=out/'visual';assets=dest/'assets'
assets.mkdir(parents=True,exist_ok=True)
groups=[];manifest=[]
def picture(source,label):
 assert source.exists(),source
 stem='image-'+str(len(manifest)).zfill(3)
 image=Image.open(source).convert('RGB');assert image.size==(320,240),source
 normal=assets/(stem+'.png');zoom=assets/(stem+'-4x.png')
 image.save(normal);image.resize((1280,960),Image.Resampling.NEAREST).save(zoom)
 manifest.append(dict(source=str(source.relative_to(root)),original=normal.name,zoom=zoom.name,label=label))
 return '<figure><figcaption>'+html.escape(label)+'</figcaption><div class="frame"><img width="320" height="240" src="assets/'+normal.name+'" alt="'+html.escape(label,quote=True)+'"></div><p><a href="assets/'+normal.name+'">320 × 240</a> · <a href="assets/'+zoom.name+'">4× nearest-neighbor</a></p></figure>'
def group(title,note,images):
 groups.append('<section><h2>'+html.escape(title)+'</h2><p class="note">'+html.escape(note)+'</p><div class="comparison">'+''.join(picture(p,l) for p,l in images)+'</div></section>')
group('Choose a useful format / Elige un formato pertinente','S + A + FORMAT · EXE apply/aplicar · BACK close/cerrar',[
 (out/'format-pool/en-complex.png','English · complex'),(out/'format-pool/es-complex.png','Español · complejo'),
 (out/'format-pool/en-fraction.png','English · fraction'),(out/'format-pool/es-precision.png','Español · FIX precision')])
for name,title,stages in [
 ('decimal','0.1 → 1/10 ↔ 0.1',['exact','decimal','back']),
 ('pi','2π ↔ decimal',['exact','decimal','back']),
 ('complex-phase-deg','1+i → √2 ∠ θ',['exact','polar','degrees']),
 ('complex-from-deg','Complex formats from DEG input / Entrada DEG',['exact','polar','exponential']),
 ('angle-units','asin(0.5) · units of the result / unidades del resultado',['exact','degrees','gradians','radians']),
 ('plain-half','1/2 · no angle formats / sin formatos angulares',['exact','menu']),
 ('mixed','7/3 · mixed fraction / fracción mixta',['exact','mixed']),
 ('fix-round','FIX · 9.995 → 10.00',['exact','digits','fixed']),
 ('engineering','ENG · exponent in steps of 3 / exponente en pasos de 3',['exact','eng','eng-shift','eng-left']),
 ('prime','360 · prime factors / factores primos',['exact','prime']),
]:
 group(title,'Same expression and canonical value / Misma expresión y valor canónico.',[(out/'formats-final'/name/(s+'.png'),s) for s in stages])
for count in [4,7,20,39,40]:
 group(str(count)+' nested square roots of 2 / raíces cuadradas anidadas de 2',
       'Same key sequence / Misma secuencia de teclas. 40 exceeds the declared semantic budget / 40 supera el límite semántico.',[
  (out/'roots-before'/str(count)/'result.png','Before / Antes'),
  (out/'roots-after'/str(count)/'result.png','After / Después'),
  (out/'roots-after'/str(count)/'input.png','Editable / Entrada')])
detectors=json.loads((out/'result-detectors.json').read_text())
fixtures=json.loads((root/'tests/fixtures/calculation-results-breaking.json').read_text())
for record in detectors['detectors']:
 name=record['id'];fixture=next(f for f in fixtures if f['id']==name);i=fixture['round'];stage='tail' if name=='ans-complex' else 'evaluated'
 group(name,'Expected / Esperado: '+str(record['expected'] if record['expected'] is not None else 'Undefined'),[
  (root/'out/renderer-recovery-02'/('results-final-r'+str(i))/name/(stage+'.png'),'Before / Antes'),
  (out/('results-final-'+str(i))/name/(stage+'.png'),'After / Después')])
page='''<!doctype html><html lang="en"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>NumOS · Calculation FORMAT 01</title><style>
:root{--scale:1;color:#152b34;background:#f2f4f1;font:16px/1.55 system-ui,sans-serif}*{box-sizing:border-box}body{margin:0}header{padding:52px max(24px,calc((100vw - 1400px)/2));background:#153e46;color:#f7faf6}header p{max-width:850px;color:#d2e6df}h1{font-size:clamp(28px,4vw,50px);font-weight:600;letter-spacing:-.04em;line-height:1.15;margin:12px 0 24px}.eyebrow{font-size:12px;letter-spacing:.16em;text-transform:uppercase}main{max-width:1480px;margin:auto;padding:32px}nav{display:flex;align-items:center;gap:8px;position:sticky;top:0;background:#f2f4f1ef;padding:12px 0;z-index:2}button{font:inherit;cursor:pointer;border:1px solid #b3c5c2;border-radius:8px;padding:8px 18px;background:#fff;color:inherit}button.active{background:#153e46;color:white}section{margin:32px 0 52px}h2{font-size:23px;font-weight:600;margin:0 0 6px}.note{color:#516b73;max-width:1000px;margin:0 0 18px}.comparison{display:flex;gap:20px;overflow-x:auto;align-items:flex-start;padding:2px 0 16px}figure{flex:none;margin:0;background:white;border:1px solid #d3dcd8;border-radius:10px;overflow:hidden}figcaption{padding:12px 14px;font-weight:600;font-size:14px}.frame{padding:10px;background:#e7ece8}img{display:block;width:calc(320px * var(--scale));height:calc(240px * var(--scale));image-rendering:pixelated}figure p{margin:10px 14px;font-size:12px}a{color:#17637a}footer{border-top:1px solid #c7d3cd;padding:24px 0;color:#516b73;font-size:14px}@media(max-width:700px){main{padding:16px}header{padding:30px 20px}}
</style><header><span class="eyebrow">NumOS · Native C++ / LVGL · Review candidates</span><h1>One answer. The right representation.<br>Una respuesta. El formato adecuado.</h1><p>Exact values, readable roots and choices that fit the result. Valores exactos, raíces legibles y opciones según el resultado.</p><p>FORMAT: exact ↔ decimal · S+A+FORMAT: selector · SHIFT+×10^x: ENG</p></header><main>
<nav aria-label="Integer pixel scale"><span>Pixels / Píxeles</span><button class="active" data-scale="1">1×</button><button data-scale="2">2×</button><button data-scale="4">4×</button></nav>'''+''.join(groups)+'''<footer>Originals 320×240; integer nearest-neighbor enlargement only. Originales 320×240; ampliación entera.<br>Native captures are not a physical LCD inspection. Las capturas native no prueban el LCD físico.<br>Prepared locally; no deployment or flash. Preparado localmente; sin publicación ni flash.<br><a href="manifest.json">Capture manifest / Manifiesto de capturas</a></footer></main>
<script>document.querySelectorAll('[data-scale]').forEach(b=>b.onclick=()=>{document.documentElement.style.setProperty('--scale',b.dataset.scale);document.querySelectorAll('[data-scale]').forEach(x=>x.classList.toggle('active',x===b));});</script></html>'''
(dest/'index.html').write_text(page,encoding='utf-8')
(dest/'manifest.json').write_text(json.dumps(manifest,indent=2,ensure_ascii=False),encoding='utf-8')
for link in re.findall(r'(?:src|href)="([^"]+)"',page):
 target=(dest/link).resolve();assert target.is_relative_to(dest.resolve()) and target.is_file(),link
zipPath=out/'calculation-format-01-visual.zip'
with zipfile.ZipFile(zipPath,'w',zipfile.ZIP_DEFLATED) as z:
 for file in sorted(dest.rglob('*')):
  if file.is_file():
   assert file.suffix in {'.html','.json','.png'},file
   z.write(file,file.relative_to(dest))
print(len(manifest),'screens; relative resources verified; no font files;',zipPath)
