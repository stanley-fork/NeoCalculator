#!/usr/bin/env python3
"""Package real unscaled framebuffer captures; no font files or synthetic UI."""
import argparse,hashlib,html,json,re,zipfile
from pathlib import Path
from PIL import Image
p=argparse.ArgumentParser();p.add_argument('--events',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
a.out.mkdir(parents=True,exist_ok=True);assets=a.out/'images';assets.mkdir(exist_ok=True)
names=['units','length','metre-centred','km','micrometre','quectometre','quettametre','electricity','resistance','kohm','microfarad','square-cm','cubic-cm','edited-quantity','unavailable','both-infinities']
captions={'units':'Unidades / Units','length':'Longitud / Length','metre-centred':'Metro centrado / Metre centred','km':'Kilometro / Kilometre','micrometre':'Micrometro / Micrometre','quectometre':'Extremo inferior / Smallest prefix','quettametre':'Extremo superior / Largest prefix','electricity':'Electricidad / Electricity','resistance':'Resistencia / Resistance','kohm':'kΩ','microfarad':'µF','square-cm':'cm² = (cm)²','cubic-cm':'cm³ = (cm)³','edited-quantity':'Entrada editada / Edited input','unavailable':'Evaluacion pendiente / Calculation deliberately unavailable','both-infinities':'Dos valores, ±∞ / Two values, ±∞'}
sections=[];manifest=[]
for locale in ['en','es']:
    cards=[]
    for name in names:
        source=a.events/f'{locale}-{name}.ppm';image=Image.open(source);assert image.size==(320,240)
        target=assets/f'{locale}-{name}.png';image.save(target)
        manifest.append({'file':target.relative_to(a.out).as_posix(),'source':source.as_posix(),'width':320,'height':240,'sha256':hashlib.sha256(target.read_bytes()).hexdigest()})
        cards.append(f'<figure><img src="images/{target.name}" width="320" height="240" alt="{html.escape(captions[name])}"><figcaption>{html.escape(captions[name])}</figcaption></figure>')
    sections.append(f'<section id="{locale}"><h2>{"English" if locale=="en" else "Español"}</h2><div class="grid">'+''.join(cards)+'</div></section>')
cards=[]
for name,label in [('favorite-mm','Favorito mm: variante exacta'),('favorite-kmh','Favorito km/h: componentes tipados'),('search-4-15','mA: miliamperio'),('search-4-9','MA: megaamperio'),('search-10-9','MHz'),('search-10-15','mHz'),('search-12-0','Pa: pascal'),('search-4-18','pA: picoamperio'),('search-18-0','Ω: letra y unidad desambiguadas')]:
    source=a.events/(name+'.ppm');image=Image.open(source);assert image.size==(320,240);target=assets/(name+'.png');image.save(target)
    manifest.append({'file':target.relative_to(a.out).as_posix(),'source':source.as_posix(),'width':320,'height':240,'sha256':hashlib.sha256(target.read_bytes()).hexdigest()})
    cards.append(f'<figure><img src="images/{target.name}" width="320" height="240" alt="{html.escape(label)}"><figcaption>{html.escape(label)}</figcaption></figure>')
sections.append('<section id="identity"><h2>Identidades y favoritos</h2><div class="grid">'+''.join(cards)+'</div></section>')
document='''<!doctype html><html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>NumOS — UNIT-CATALOG-01</title><style>
*{box-sizing:border-box}body{margin:0;background:#f3f5f7;color:#1c2e42;font:16px system-ui,sans-serif}main{max-width:1120px;margin:auto;padding:32px}header{border-bottom:3px solid #d48b18;padding-bottom:20px}h1{font-size:30px;margin:0 0 12px}p{line-height:1.55;max-width:850px}nav{display:flex;gap:20px;margin-top:18px}a{color:#185581}section{margin-top:40px}.grid{display:grid;grid-template-columns:repeat(auto-fit, minmax(320px,1fr));gap:20px}figure{margin:0;padding:12px;background:white;border:1px solid #cdd5dd;border-radius:6px}img{display:block;width:320px;height:240px;max-width:100%;object-fit:contain;image-rendering:pixelated}figcaption{padding-top:12px;font-size:14px;min-height:44px}footer{margin-top:32px;color:#566779}</style><main><header><h1>NumOS · Catálogo de unidades</h1><p>Capturas reales del emulador, en su resolución original de 320 × 240. El catálogo permite insertar y editar unidades tipadas. El cálculo dimensional y las conversiones de resultados corresponden a CALC-UNITS-01 y todavía no están disponibles.</p><nav><a href="#en">English</a><a href="#es">Español</a><a href="#identity">Identidades y favoritos</a></nav></header>'''+''.join(sections)+'''<footer>Recursos locales y relativos. Sin fuentes adjuntas ni dependencias de red. Las capturas se obtienen mediante eventos de teclado sobre el modal y el editor reales.</footer></main></html>'''
(a.out/'index.html').write_text(document,encoding='utf8');(a.out/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),encoding='utf8')
for link in re.findall(r'src="([^"]+)"',document):assert (a.out/link).is_file()
archive=a.out.with_suffix('.zip')
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
    for file in sorted(a.out.rglob('*')):
        if file.is_file():
            assert file.suffix.lower() not in ('.ttf','.otf','.woff','.woff2','.c')
            z.write(file,file.relative_to(a.out).as_posix())
print(f'PASS {len(manifest)} original 320x240 images; relative resources verified; {archive}')
