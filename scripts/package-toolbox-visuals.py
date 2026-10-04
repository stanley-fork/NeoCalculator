#!/usr/bin/env python3
"""Lossless framebuffer gallery, originals + integer nearest-neighbour enlargements."""
from pathlib import Path
from PIL import Image,ImageDraw
import argparse,hashlib,html,json,zipfile
p=argparse.ArgumentParser();p.add_argument('--root',type=Path,default=Path('out/toolbox-01'));p.add_argument('--popup',action='store_true');p.add_argument('--variables',action='store_true');a=p.parse_args()
gallery=a.root/'visual';gallery.mkdir(parents=True,exist_ok=True)
for sub in ('original','4x'): (gallery/sub).mkdir(exist_ok=True)
items=[
 ('root','Raíz sobre el editor','events-complete-final/root.ppm'),
 ('favorites-empty','Favoritos vacíos','events-complete-final/favorites-empty.ppm'),
 ('favorites','Favoritos poblados','events-complete-final/favorites-populated.ppm'),
 ('options','Opciones por FORMAT','events-complete-final/management.ppm'),
 ('reorder','Orden explícito','events-complete-final/favorites-reordered.ppm'),
 ('remove','Quitar favorito','events-complete-final/favorites-after-remove.ppm'),
 ('limit','Límite de 24, sin expulsión','events-complete-final/favorites-limit-warning.ppm'),
 ('recent','Recientes confirmados','events-complete-final/recents.ppm'),
 *[(f'category-{i}',title,f'events-complete-final/category-{i}.ppm') for i,title in enumerate(['Plantillas básicas','Trigonometría','Potencias y logaritmos','Cálculo','Complejos','Aritmética entera','Símbolos y constantes'])],
 ('long-category','Final de categoría larga','events-complete-final/category-1-end.ppm'),
 ('search-en','Búsqueda en interfaz EN, alias español','events-complete-final/search-es.ppm'),
 ('search-es','Búsqueda ES, consulta con tilde','events-complete-final/spanish-search.ppm'),
 ('search-empty','Consulta sin resultados','events-complete-final/search-empty.ppm'),
 ('help-en','Ayuda EN','events-complete-final/help.ppm'),
 ('help-es','Ayuda ES','events-complete-final/spanish-help.ppm'),
 ('variants','Acción principal y variantes','events-complete-final/variants.ppm'),
 ('tall-template','Integral con cuatro ranuras','events-complete-final/tall-integral.ppm'),
 ('unsupported','Rechazo en contexto Grapher','events-complete-final/unsupported-context.ppm'),
 ('allocation-failure','Fallo persistente de asignación; expresión intacta','fault-overflow-final/insert-1-2.ppm'),
 ('save-failure','Guardado fallido, conservado en sesión','fault-overflow-final/save-1-1.ppm'),
 ('stale-receiver','Receptor obsoleto, sin publicación','fault-overflow-final/insert-0-2.ppm'),
 ('indexed-root','Raíz de índice editable completada','events-complete-final/insert-111-0.ppm'),
 ('log-base','Logaritmo de base editable completado','events-complete-final/insert-120-0.ppm'),
 ('continuation','Insertar tras mantisa y continuar: 7','events-complete-final/continuation.ppm'),
 ('history','Editar y recuperar del historial','events-complete-final/recalled-root.ppm'),
 ('steps','Steps conserva TOOLBOX en resultados','events-complete-final/equations-steps.ppm'),
]
if a.popup:
 items=[(name,title,source.replace('events-complete-final/','events-final/').replace('fault-overflow-final/','faults/').replace('insert-1-2','insert-1-1').replace('insert-0-2','insert-0-1')) for name,title,source in items if not name.startswith('category-')]
 items[8:8]=[(f'category-{i}',title,f'events-final/category-{i}.ppm') for i,title in enumerate(['Cálculo','Complejos','Aritmética y combinatoria','Trigonometría','Redondeo'])]
 items=[(name,title,source.replace('category-1-end','category-3-end')) for name,title,source in items]
if a.popup:items.append(('tall-context','Popup sobre fracción alta','tall/popup-tall-context.ppm'))
if a.variables:
 items=[(name,title,source.replace('tall/','events-final/')) for name,title,source in items]
 items += [(name,title,f'events-final/{name}.ppm') for name,title in [
  ('variables','Variables: tres alfabetos'),('latin','Latín: a-z'),('latin-end','Latín: final del alfabeto'),
  ('latin-case','Minúscula y mayúscula'),('greek','Griego: alfa-omega'),('greek-end','Griego: final del alfabeto'),
  ('delta-case','DERECHA selecciona la mayúscula'),('delta-power','Delta al cuadrado'),
  ('special','Constantes y formas griegas alternativas'),('tabs-focus','Foco completo en pestañas'),
  ('greek-query','Consulta griega legible'),('greek-variant-query','Consulta con variante griega'),
  ('greek-delta-query','Consulta con Delta mayúscula'),('greek-fraction-power','Beta con descendente en fracción y potencia')]]
manifest=[];cards=[];images=[]
for name,title,source in items:
 im=Image.open(a.root/source).convert('RGB');assert im.size==(320,240),source;images.append((im,title))
 original=gallery/'original'/(name+'.png');large=gallery/'4x'/(name+'.png')
 im.save(original);im.resize((1280,960),Image.Resampling.NEAREST).save(large)
 manifest.append({'id':name,'title':title,'source':source,'original':[320,240],'enlargement':[1280,960],'originalSha256':hashlib.sha256(original.read_bytes()).hexdigest()})
 cards.append(f'<figure><a href="4x/{name}.png"><img src="original/{name}.png" width="320" height="240" alt="{html.escape(title)}"></a><figcaption>{html.escape(title)} · <a href="original/{name}.png">320×240</a> · <a href="4x/{name}.png">4×</a></figcaption></figure>')
page='''<!doctype html><html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>NumOS · TOOLBOX-01</title><style>body{margin:24px;background:#edf1f6;color:#223348;font:15px system-ui,sans-serif}main{display:flex;flex-wrap:wrap;gap:24px}figure{margin:0;background:white;padding:12px;border-radius:8px;width:320px}img{display:block;image-rendering:pixelated}figcaption{padding-top:12px}a{color:#2465b0}p{max-width:850px}</style><h1>NumOS · TOOLBOX-01</h1><p>Capturas del framebuffer real, 320×240. Cada captura incluye una ampliación entera 4× sin suavizado. Revisión del emulador, no del LCD físico. No se incluyen fuentes ni recursos de red. Los fallos son fixtures privados, fuera del firmware.</p><main>'''+''.join(cards)+'</main></html>'
(gallery/'index.html').write_text(page,encoding='utf8');(gallery/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2),encoding='utf8')
(gallery/'LEEME.txt').write_text('Abrir index.html sin conexión. original/: PNG 320x240. 4x/: PNG 1280x960, vecino más próximo. Sin fuentes. No representa validación física del LCD.\n',encoding='utf8')
# Contact sheets are only review aids, outside the delivered ZIP.
for start in range(0,len(images),6):
 sheet=Image.new('RGB',(960,530),'#dce3ed');draw=ImageDraw.Draw(sheet)
 for i,(im,title) in enumerate(images[start:start+6]):
  x=i%3*320;y=i//3*265;sheet.paste(im,(x,y+20));draw.text((x+4,y+3),title,fill='black')
 sheet.save(a.root/f'final-review-{start//6}.png')
archive=a.root/'TOOLBOX_01_visual.zip'
with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
 for name in ('index.html','manifest.json','LEEME.txt'):z.write(gallery/name,name)
 for name,_,_ in items:
  for sub in ('original','4x'):z.write(gallery/sub/(name+'.png'),sub+'/'+name+'.png')
with zipfile.ZipFile(archive) as z:
 assert z.testzip() is None and len(z.namelist())==3+2*len(items)
 assert not any(n.endswith(('.ttf','.otf','.woff','.woff2')) for n in z.namelist())
print(f'PASS visual ZIP: {len(items)} inspected-ready captures, originals and 4x; {archive.stat().st_size} bytes')
