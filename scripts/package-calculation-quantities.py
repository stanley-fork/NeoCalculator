#!/usr/bin/env python3
"""Package original native quantity renders with relative resources and hashes."""
import argparse,hashlib,html,json,shutil,zipfile
from pathlib import Path
from PIL import Image
p=argparse.ArgumentParser();p.add_argument('--screens',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();a.out.mkdir(parents=True,exist_ok=True)
items=[('user-example','Ejemplo compuesto','100 s⁻¹; frecuencia solo al elegir Hz.'),('user-example-hz','Presentación elegida: Hz','La selección conserva la cantidad canónica.'),('sum-metres','Suma m + cm','2 m + 30 cm = 23/10 m.'),('sum-centimetres','Salida en centímetros','230 cm, calculados desde 23/10 m.'),('area','Producto de centímetros','2 cm × 3 cm = 3/5000 m².'),('area-cm2','Salida cm²','El prefijo se eleva al cuadrado: 6 cm².'),('velocity','Velocidad coherente','6 km / 300 s = 20 m/s.'),('velocity-kmh','Velocidad km/h','72 km/h.'),('electricity','Resultado eléctrico','12 V / 3 A = 4 Ω.'),('energy-joule','Energía','100 W × 2 h = 720000 J.'),('energy-kwh','Energía kW·h','1/5 kW·h; kilo modifica W.'),('components-menu','Unidades por componentes','Cada componente mantiene su exponente.'),('components-length','Selector de componente','Elegir longitud compatible y prefijo.'),('components-kmh','Composición de salida','Cambiar m → km y s → h conserva 20 m/s.'),('quantity-scientific','SCI','Formato numérico aplicado al coeficiente mostrado.'),('quantity-engineering','ENG','Conserva las unidades seleccionadas.'),('quantity-fixed','FIX','Conserva el valor canónico.'),('dimension-error','Error dimensional','No publica un número ni sobrescribe Ans.'),('recovered-ans','Recuperación','Ans continúa siendo la cantidad válida anterior.'),('ans-after-output','Ans tras convertir a cm','Ans / (2 s) = 1 m/s, sin aplicar el prefijo otra vez.')]
for locale in ('en-US','en-GB','es-ES','es-419'):
 for suffix,label in [('result','Resultado'),('output-menu','FORMAT'),('context','Escala contextual pendiente'),('physical-reference','Referencia física pendiente')]:items.append((locale+'-'+suffix,locale+' · '+label,'Misma identidad, matemática y símbolos; texto regional.'))
manifest=[];cards=[]
for name,title,caption in items:
 src=a.screens/(name+'.png');assert src.is_file(),src
 with Image.open(src) as im:assert im.size==(320,240),src
 dst=a.out/src.name;shutil.copy2(src,dst);digest=hashlib.sha256(dst.read_bytes()).hexdigest();manifest.append(dict(file=dst.name,source=str(src),width=320,height=240,sha256=digest))
 cards.append(f'<figure><img src="{html.escape(dst.name)}" width="320" height="240" alt="{html.escape(title)}"><figcaption><strong>{html.escape(title)}</strong><p>{html.escape(caption)}</p></figcaption></figure>')
page='<!doctype html><html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>NumOS · CALC-UNITS-01</title><style>body{font:16px system-ui;background:#f1f4f7;color:#17283c;margin:24px auto;max-width:1450px;padding:0 20px}header{max-width:900px;margin-bottom:32px}main{display:grid;grid-template-columns:repeat(auto-fit,minmax(320px,1fr));gap:24px}figure{margin:0;background:white;border:1px solid #d4dde7;border-radius:8px;overflow:hidden}img{display:block;image-rendering:pixelated;max-width:100%;height:auto;margin:auto}figcaption{padding:16px}p{line-height:1.5}figure p{font-size:14px;color:#465b73}</style><header><h1>NumOS · Cantidades y unidades de salida</h1><p>Capturas originales de 320×240 del editor real y de su Toolbox. La cantidad canónica se mantiene al cambiar la presentación. Las temperaturas afines, referencias físicas, escalas contextuales e incertidumbres siguen pendientes.</p><p>Calculation → SHIFT → ALPHA → FORMAT → Unidad de salida.</p></header><main>'+''.join(cards)+'</main></html>'
(a.out/'index.html').write_text(page,encoding='utf8');(a.out/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),encoding='utf8')
for record in manifest:assert (a.out/record['file']).is_file()
archive=a.out.with_suffix('.zip')
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for p in [a.out/'index.html',a.out/'manifest.json',*[a.out/r['file'] for r in manifest]]:z.write(p,p.name)
with zipfile.ZipFile(archive) as z:assert all(Path(n).suffix in ('.png','.html','.json') for n in z.namelist())
print('PASS',len(manifest),'originals; relative resources checked; no font files;',archive)
