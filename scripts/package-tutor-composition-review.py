#!/usr/bin/env python3
"""Portable review pack from real 320x240 emulator captures, not generated art."""
from pathlib import Path
import argparse, html, json, re, shutil, zipfile
from PIL import Image
p=argparse.ArgumentParser();p.add_argument('--ui',type=Path,required=True);p.add_argument('--pan',type=Path,required=True);p.add_argument('--results',type=Path,required=True);p.add_argument('--catalog',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
a.out.mkdir(parents=True,exist_ok=True);assets=a.out/'images';assets.mkdir(exist_ok=True)
parts=['<!doctype html><meta charset="utf-8"><title>NumOS 03B · EN / ES</title><style>body{font:16px system-ui;margin:24px;background:#eef1f4;color:#182230}section{display:flex;gap:16px;flex-wrap:wrap}figure{margin:0 0 16px;background:white;padding:8px}img{width:320px;height:240px}figcaption{max-width:320px;font-size:13px}h2{margin-top:40px}</style><h1>NumOS · Sustituciones comprobadas · EN / ES</h1><p>Capturas originales de 320×240. Cada secuencia conserva todos los desplazamientos verticales. Al final: desplazamiento horizontal y Results independientes.</p><p><a href="catalog.md">Catálogo bilingüe generado desde la fuente</a></p>']
count=0
def frame(path):
    global count
    assert path.is_file(),path
    with Image.open(path) as im:assert im.size==(320,240),(path,im.size)
    name=path.name;dest=assets/name
    if dest.exists():assert dest.read_bytes()==path.read_bytes(),name
    else:shutil.copyfile(path,dest)
    count+=1;parts.append(f'<figure><img loading="lazy" src="images/{html.escape(name)}"><figcaption>{html.escape(path.stem)}</figcaption></figure>')
for sequence in json.loads((a.ui/'results.json').read_text()):
    parts.append('<h2>'+html.escape(sequence['case']+' / '+sequence['mode'])+'</h2><section>')
    for page in sequence['pages']:
        for name in page['frames']:frame(a.ui/(name+'.png'))
    parts.append('</section>')
parts.append('<h2>Fórmulas anchas · recorrido horizontal</h2><section>')
for record in json.loads((a.pan/'results.json').read_text()):
    for name in record['frames'][:record['returnAt']+1]:frame(a.pan/(name+'.png'))
parts.append('</section><h2>Results · origen Giac independiente</h2><section>')
for path in sorted(a.results.glob('*.png')):
    with Image.open(path) as im:original=im.size==(320,240)
    if original:frame(path)
parts.append('</section>')
index='\n'.join(parts);(a.out/'index.html').write_text(index,encoding='utf-8')
shutil.copyfile(a.catalog,a.out/'catalog.md')
(a.out/'LEEME.txt').write_text('Extrae todo el ZIP y abre index.html. No necesita Internet. Las capturas son del emulador; no acreditan aceptación física ni aprobación humana del español. Las etiquetas guided corresponden a English; es, a Español.\n',encoding='utf-8')
for relative in re.findall(r'(?:src|href)="([^"]+)"',index):
    target=(a.out/html.unescape(relative)).resolve();assert target.is_relative_to(a.out.resolve()) and target.is_file(),relative
files=[a.out/'index.html',a.out/'catalog.md',a.out/'LEEME.txt',*assets.glob('*.png')]
archive=a.out.with_suffix('.zip')
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
    for file in files:z.write(file,file.relative_to(a.out).as_posix())
with zipfile.ZipFile(archive) as z:assert z.testzip() is None
(a.out/'manifest.json').write_text(json.dumps(dict(originalFrames=count,files=len(files),allRelativeResourcesPresent=True,zip=str(archive)),indent=2))
print(archive,count,'original frames; all relative resources present')
