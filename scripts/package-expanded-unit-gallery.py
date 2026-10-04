#!/usr/bin/env python3
"""Package original native/web UNIT-CATALOG-02 captures with no remote resources."""
import argparse
import hashlib
import html
import json
import re
import zipfile
from pathlib import Path

from PIL import Image

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--events', type=Path, required=True)
p.add_argument('--locales', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--extra-image', type=Path, action='append', default=[], help='Additional reviewed original capture')
p.add_argument('--base-events', type=Path, help='Validated UNIT-CATALOG-01 event captures from the current candidate')
p.add_argument('--web-locales', type=Path, help='Original 320x240 Chromium locale canvas captures')
p.add_argument('--web-reference-locales', type=Path, help='Final original Chromium regional reference captures')
a = p.parse_args()
results = json.loads((a.events / 'results.json').read_text(encoding='utf8'))
assert results and all(result['passed'] for result in results.values()), 'Event suite must pass before packaging.'
base_results = {}
if a.base_events:
    base_results = json.loads((a.base_events / 'results.json').read_text(encoding='utf8'))
    assert base_results and all(result['passed'] for result in base_results.values()), 'Base event suite must pass.'
a.out.mkdir(parents=True, exist_ok=True)
assets = a.out / 'images'
assets.mkdir(exist_ok=True)

labels = {
    'calorie-capital': 'Cal: kilocaloría termoquímica / Thermochemical kilocalorie',
    'watt-hour-prefix-centred': 'Wh: selección neutral centrada / Neutral selection centred',
    'watt-hour-prefix-kwh': 'kWh: prefijo sobre W / Prefix on W',
    'gallons-us': 'Galón líquido estadounidense / US liquid gallon',
    'gallons-imperial': 'Galón imperial / Imperial gallon',
    'fuel-coefficient': 'L/(100·km): coeficiente y componentes explícitos / Explicit components',
    'fuel-coefficient-unavailable': 'L/(100·km): cálculo aún no disponible / Calculation not yet available',
    'count-components-flops': 'FLOP/s: recuento y tiempo / Count and time',
    'historical-vara': 'Vara de Burgos: correspondencia histórica / Historical correspondence',
    'typed-physical-reference': 'Velocidad de la luz: referencia física tipada / Typed physical reference',
    'favorites-read-cal': 'Favorito recuperado: kcal termoquímica / Reloaded exact variant',
    'favorites-read-kwh': 'Favorito recuperado: kWh / Reloaded component prefix',
    'favorite-mm': 'Favorito mm: variante exacta / Exact prefixed favourite',
    'favorite-kmh': 'Favorito km/h: componentes tipados / Typed components',
    'search-4-15': 'mA: miliamperio / Milliampere',
    'search-4-9': 'MA: megaamperio / Megaampere',
    'search-10-9': 'MHz: megahercio / Megahertz',
    'search-10-15': 'mHz: milihercio / Millihertz',
    'search-12-0': 'Pa: pascal',
    'search-4-18': 'pA: picoamperio / Picoampere',
    'search-18-0': 'Ω: letra y unidad desambiguadas / Letter and unit distinguished',
    'search-19-0': 'S: siemens',
    'search-3-0': 's: segundo / Second',
}
locales = {'en-US': 'English (US)', 'en-GB': 'English (UK)', 'es-ES': 'Español (España)',
           'es-419': 'Español (Latinoamérica)'}
help_labels = {'ph': 'pH', 'mach': 'Mach', 'decibel': 'dB', 'normal-gas': 'Nm³',
    'electron-mass': 'Masa electrónica / Electron mass', 'atomic-time': 'Tiempo atómico / Atomic time',
    'planck-length': 'Longitud de Planck / Planck length', 'planck-temperature': 'Temperatura de Planck / Planck temperature'}
base_labels = {
    'units': 'Unidades / Units', 'length': 'Longitud / Length',
    'metre-centred': 'Metro: selección neutral centrada / Metre: centred neutral selection',
    'km': 'km: kilómetro / Kilometre', 'micrometre': 'µm: micrómetro / Micrometre',
    'quectometre': 'qm: extremo inferior / Smallest prefix',
    'quettametre': 'Qm: extremo superior / Largest prefix',
    'electricity': 'Electricidad / Electricity', 'resistance': 'Resistencia / Resistance',
    'kohm': 'kΩ: kiloohmio / Kiloohm', 'microfarad': 'µF: microfaradio / Microfarad',
    'square-cm': 'cm² = (cm)²', 'cubic-cm': 'cm³ = (cm)³',
    'edited-quantity': 'Cantidad editada / Edited quantity',
    'unavailable': 'Aviso provisional / Interim availability notice',
    'both-infinities': 'Dos valores, ±∞ / Two values, ±∞',
}

def label_for(stem):
    if stem in labels:
        return labels[stem]
    for code, label in locales.items():
        for gas in ('normal', 'standard'):
            if stem == code + '-' + gas + '-gas-result':
                spelling = 'meter' if code == 'en-US' else 'metre'
                return label + ' · ' + gas.title() + ' cubic ' + spelling + ' · Chromium, canvas 320×240'
        if stem == code + '-metre-result':
            return label + ' · ' + ('Meter' if code == 'en-US' else 'Metre') + ' · Chromium, canvas 320×240'
        if stem == code + '-selected':
            return label + ' · Ajustes / Settings'
        if stem == code + '-catalogue':
            return label + ' · Catálogo / Catalogue'
        if stem == code + '-catalogue-unavailable':
            return label + ' · Aviso provisional / Interim availability notice'
    for code, label in [('en', 'English'), ('es', 'Español')]:
        if stem.startswith(code + '-') and stem[len(code)+1:] in base_labels:
            return label + ' · ' + base_labels[stem[len(code)+1:]]
        if stem == code + '-ambiguity-ph':
            return label + ' · pH: picohenrio y acidez / Picohenry and acidity'
        if stem == code + '-ambiguity-decibel':
            return label + ' · dB: decibyte y decibelio / Decibyte and decibel'
        if stem == code + '-dalton-help':
            return label + ' · Dalton: valor medido / Measured value'
        prefix = code + '-reference-help-'
        if stem.startswith(prefix):
            return label + ' · ' + help_labels[stem[len(prefix):]]
    raise AssertionError('Missing intentional caption: ' + stem)

groups = [
    ('settings', 'Cuatro regiones de idioma / Four language regions',
     [a.locales / (code + '-selected.png') for code in locales]),
    ('catalogue', 'Catálogo y editor / Catalogue and editor',
     sorted(path for path in a.events.glob('*.png') if '-help' not in path.stem and '-reference-help-' not in path.stem)),
    ('help-en', 'Ayuda científica en inglés / Scientific help in English', sorted(a.events.glob('en-*-help*.png'))),
    ('help-es', 'Ayuda científica en español / Scientific help in Spanish', sorted(a.events.glob('es-*-help*.png'))),
]
groups[1][2].extend(a.extra_image)
if a.web_locales:
    groups[0][2].extend(a.web_locales / (code + '-metre-result.png') for code in ('en-US', 'en-GB'))
if a.web_reference_locales:
    groups[0][2].extend(a.web_reference_locales / (code + '-' + gas + '-gas-result.png')
                       for gas in ('normal', 'standard') for code in ('en-US', 'en-GB'))
if a.base_events:
    base_files = [a.base_events / (code + '-' + name + '.ppm')
                  for code in ('en', 'es') for name in base_labels]
    base_files.extend(a.base_events / (name + '.ppm') for name in (
        'favorite-mm', 'favorite-kmh', 'search-4-15', 'search-4-9', 'search-10-9',
        'search-10-15', 'search-12-0', 'search-4-18', 'search-18-0', 'search-19-0', 'search-3-0'))
    groups.insert(1, ('base', 'Catálogo base conservado / Preserved base catalogue', base_files))
manifest = []
sections = []
pixel_hashes = {}
represented_captures = 0
for key, title, files in groups:
    assert files, 'No screenshots for ' + key
    cards = []
    for source in files:
        represented_captures += 1
        caption = label_for(source.stem)
        with Image.open(source) as image:
            assert image.size == (320, 240), f'Not an original framebuffer: {source}'
            pixel_hash = hashlib.sha256(image.convert('RGBA').tobytes()).hexdigest()
            if pixel_hash in pixel_hashes:
                previous = pixel_hashes[pixel_hash]
                previous.setdefault('identical_sources', []).append(source.as_posix())
                previous.setdefault('identical_captions', []).append(caption)
                continue
            target = assets / (source.stem + '.png')
            image.save(target)
        relative = target.relative_to(a.out).as_posix()
        manifest.append({'file': relative, 'source': source.as_posix(), 'width': 320, 'height': 240,
                         'caption': caption, 'sha256': hashlib.sha256(target.read_bytes()).hexdigest()})
        pixel_hashes[pixel_hash] = manifest[-1]
        cards.append(f'<figure><a href="{html.escape(relative)}"><img src="{html.escape(relative)}" '
                     f'width="320" height="240" loading="lazy" alt="{html.escape(caption)}"></a>'
                     f'<figcaption>{html.escape(caption)}</figcaption></figure>')
    sections.append(f'<section id="{key}"><h2>{html.escape(title)}</h2><div class="grid">' + ''.join(cards) + '</div></section>')

captured = {entry['source'] for entry in manifest}
captured.update(source for entry in manifest for source in entry.get('identical_sources', []))
assert all(path.as_posix() in captured for path in a.events.glob('*.png')), 'Every event screenshot must be included.'
assert len({entry['file'] for entry in manifest}) == len(manifest), 'Duplicate screenshot names.'
document = '''<!doctype html><html lang="es"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>NumOS — UNIT-CATALOG-02</title><style>
*{box-sizing:border-box}body{margin:0;background:#f3f5f7;color:#1c2e42;font:16px system-ui,sans-serif}main{max-width:1120px;margin:auto;padding:28px}header{border-bottom:3px solid #d48b18;padding-bottom:20px}h1{font-size:30px;margin:0 0 12px}p{line-height:1.55;max-width:920px}nav{display:flex;flex-wrap:wrap;gap:18px;margin-top:18px}a{color:#185581}section{margin-top:36px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(320px,1fr));gap:18px}figure{margin:0;padding:10px;background:white;border:1px solid #cdd5dd;border-radius:5px}img{display:block;width:320px;height:240px;max-width:100%;object-fit:contain;image-rendering:pixelated}figcaption{padding-top:12px;font-size:14px;min-height:48px}footer{margin-top:32px;color:#566779}
</style><main><header><h1>NumOS · Catálogo ampliado y regiones</h1>
<p>Capturas reales del emulador a 320 × 240, sin retoque. Los cuatro idiomas regionales se seleccionan en Ajustes; la traducción del sistema avanza por componentes. Unidades y referencias conservan identidad matemática. Las escalas contextuales muestran qué información adicional necesitan.</p>
<p>El cálculo dimensional y las conversiones aún no están disponibles: insertar una unidad o referencia muestra un aviso específico al evaluar y conserva la entrada. Esta galería documenta navegación, edición, persistencia y ayuda; no acredita operaciones dimensionales.</p>
<nav><a href="#settings">Regiones</a><a href="#base">Catálogo base conservado</a><a href="#catalogue">Catálogo y editor</a><a href="#help-en">English help</a><a href="#help-es">Ayuda en español</a></nav></header>''' + ''.join(sections) + '''
<footer>Todos los recursos son locales y relativos. No se adjuntan fuentes ni dependencias de red. Los originales están enlazados desde cada captura. El manifiesto incluye dimensiones, rutas de origen y hashes SHA-256.</footer></main></html>'''
if not a.base_events:
    document = document.replace('<a href="#base">Catálogo base conservado</a>', '')
(a.out / 'index.html').write_text(document, encoding='utf8')
(a.out / 'manifest.json').write_text(json.dumps({'expanded_event_cases': len(results), 'base_event_cases': len(base_results), 'represented_captures': represented_captures, 'image_count': len(manifest), 'images': manifest}, ensure_ascii=False, indent=2) + '\n', encoding='utf8')
(a.out / 'event-results.json').write_text(json.dumps(results, indent=2) + '\n', encoding='utf8')
if base_results:
    (a.out / 'base-event-results.json').write_text(json.dumps(base_results, indent=2) + '\n', encoding='utf8')
for relative in re.findall(r'(?:src|href)="([^"]+)"', document):
    if relative.startswith('#'):
        assert f'id="{relative[1:]}"' in document, 'Broken section link: ' + relative
    else:
        assert not re.match(r'^[a-z]+:', relative, re.I), 'External resource'
        assert (a.out / relative).is_file(), 'Broken relative resource: ' + relative
archive = a.out.with_suffix('.zip')
allowed = [a.out / 'index.html', a.out / 'manifest.json', a.out / 'event-results.json'] + [a.out / entry['file'] for entry in manifest]
if base_results:
    allowed.append(a.out / 'base-event-results.json')
with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as output:
    for path in allowed:
        assert path.suffix.lower() in ('.html', '.json', '.png'), 'Non-gallery file'
        output.write(path, path.relative_to(a.out).as_posix())
with zipfile.ZipFile(archive) as output:
    assert len(output.namelist()) == len(allowed)
    assert output.testzip() is None
print(f'PASS {len(results)} expanded + {len(base_results)} base event cases, {len(manifest)} original 320x240 images; relative resources and ZIP verified: {archive}')
