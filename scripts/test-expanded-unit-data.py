#!/usr/bin/env python3
"""Independent UNIT-CATALOG-02 definition, identity and request-coverage gates.

These are offline data checks, not dimensional operations in Calculation.
The fixed fixtures contain request rows and reviewed primary-source oracles;
expected factors are never read back from the production registry.
"""
import argparse
import copy
import hashlib
import importlib.util
import json
from decimal import Decimal, localcontext
from fractions import Fraction as F
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'src/math/units/registry.json'
FIXTURES = ROOT / 'tests/fixtures'
ORACLES = json.loads((FIXTURES / 'unit_catalog_02_oracles.json').read_text(encoding='utf-8'))
REQUEST = json.loads((FIXTURES / 'unit_catalog_02_requested_rows.json').read_text(encoding='utf-8'))
spec = importlib.util.spec_from_file_location('unit_generator', ROOT / 'scripts/generate-unit-catalog.py')
generator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generator)

# Stable, previously published catalogue IDs. New rows use explicit IDs 1000+row.
# This assignment is independent of row order in registry.json.
EXISTING_ITEMS = {
    1: 1, 2: 40, 3: 41, 4: 42, 5: 43, 7: 44, 17: 64, 26: 101,
    28: 45, 36: 102, 37: 46, 72: 3, 73: 47, 74: 48, 75: 49,
    79: 8, 80: 57, 81: 58, 82: 59, 85: 9, 87: 2, 88: 50, 89: 51,
    96: 52, 105: 110, 106: 111, 110: 120, 111: 112, 116: 10, 120: 11,
    123: 65, 128: 119, 142: 12, 143: 54, 144: 53, 151: 55, 154: 13,
    155: 114, 161: 56, 167: 14, 177: 5, 178: 23, 179: 60, 194: 118,
    222: 4, 223: 15, 224: 113, 226: 16, 227: 18, 228: 19, 229: 17,
    230: 22, 239: 21, 241: 20, 249: 6, 251: 116, 252: 115, 257: 29,
    259: 26, 261: 27, 263: 28, 269: 7, 270: 24, 271: 25,
}
CONTEXT_ROWS = [
    ['unit_one'], ['percent_ratio', 'per_mille_ratio', 'parts_per_million'],
    ['decibel_ratio'], ['neper_ratio'], ['dbm_power'], ['dbw_power'],
    ['dbv_voltage'], ['dbu_voltage'], ['db_spl_air'], ['db_a_weighted'],
    ['db_full_scale'], ['mach_number'], ['ph_activity'], ['brix_sucrose'],
    ['specific_gravity'], ['api_gravity'], ['chemical_equivalent'], ['normality'],
    ['osmole'], ['international_activity'], ['turbidity_ntu', 'turbidity_fnu'],
    ['american_wire_gauge', 'standard_wire_gauge'], ['mesh_size'],
    ['dots_per_inch', 'pixels_per_inch'], ['css_pixel', 'css_em', 'css_rem'],
    ['calendar_month', 'calendar_year'], ['clock_cycle'], ['jiffy'],
    ['architecture_word'],
    ['normal_gas_volume', 'standard_gas_volume', 'standard_gas_cubic_foot'],
    ['psi_absolute', 'psi_gauge', 'bar_absolute', 'bar_gauge'],
]
PHYSICAL_ROWS = [
    'speed_of_light', 'bohr_radius', 'electron_mass', 'hartree_energy', 'atomic_time',
    'reduced_planck', 'planck_length', 'planck_time', 'planck_mass', 'planck_force',
    'planck_energy', 'planck_temperature',
]


def fraction(exact):
    numerator, denominator, exponent, pi_power = exact
    assert pi_power == 0, 'This rational oracle must not discard pi'
    return F(numerator, denominator) * F(10) ** exponent


def item_terms(item):
    return item.get('terms', [[item.get('unit'), item['default_prefix'], item.get('power', 1)]])


def validate(data):
    generator.validate(data)
    units = {u['id']: u for u in data['units']}
    unit_keys = {u['key']: u for u in units.values()}
    items = {i['id']: i for i in data['items']}
    refs = {r['key']: r for r in data['references']}
    prefixes = {p['id']: p for p in data['prefixes']}
    checks = []

    def check(label, condition):
        assert condition, label
        checks.append(label)

    def atom_scale(key, prefix=0, power=1):
        u = unit_keys[key]
        return (fraction(u['scale']) * F(10) ** prefixes[prefix]['exponent']) ** power

    def item_scale(item_id, selected_prefix=None):
        item = items[item_id]
        result = fraction(item.get('coefficient', [1, 1, 0, 0]))
        for index, (uid, pid, power) in enumerate(item_terms(item)):
            if selected_prefix is not None and index == item.get('prefix_component', 0):
                pid = selected_prefix
            result *= (fraction(units[uid]['scale']) * F(10) ** prefixes[pid]['exponent']) ** power
        return result

    for uid, expected in ORACLES['unit_factors'].items():
        check('independent exact definition ' + uid, fraction(units[int(uid)]['scale']) == F(*expected))
    check('gram SI reference', atom_scale('gram') == F(1, 1000))
    check('kg is kilo gram', atom_scale('gram', 10) == 1 and items[2]['default_prefix'] == 10)
    check('mg is one millionth kg', atom_scale('gram', 15) == F(1, 10**6))
    check('cm squared prefix inside power', item_scale(101, 14) == F(1, 10000))
    check('cm cubed prefix inside power', item_scale(102, 14) == F(1, 10**6))
    check('mL cubic metre scale', atom_scale('litre', 15) == F(1, 10**6))
    check('Wh structured components', item_terms(items[114]) == [[14, 0, 1], [48, 0, 1]])
    check('Wh to J definition', item_scale(114) == 3600)
    check('kWh prefix targets W', item_scale(114, 10) == 3600000 and items[114]['prefix_component'] == 0)
    check('Ah remains charge', item_scale(113, 10) == 3600000 and item_terms(items[113])[0][0] == 4)
    check('extreme powered prefix stays nonzero', item_scale(102, 24) == F(1, 10**90))
    check('extreme powered prefix does not overflow', item_scale(102, 1) == 10**90)
    check('parsec preserves inverse pi', units[1019]['scale'] == [969394202136, 1, 5, -1])
    check('circular mil preserves pi', units[1035]['scale'][3] == 1)
    check('square angle preserves pi squared', units[1086]['scale'][3] == 2)
    check('eV/c2 exact relation', fraction(units[1104]['scale']) == F('1.602176634e-19') / 299792458**2)
    check('calorie variants distinct', units[1156]['scale'] != units[1157]['scale'])
    check('thermochemical cal exact', atom_scale('calorie_thermochemical') == F('4.184'))
    check('nutritional kcal exact', atom_scale('calorie_thermochemical', 10) == 4184)
    check('IT kcal retains its convention', atom_scale('calorie_international_table', 10) == F('4186.8'))
    check('Cal/kcal aliases target kilo only', all(a in units[1156]['variant_aliases']['10'] for a in ('Cal', 'kcal'))
          and 'Cal' not in units[1156]['aliases'] and 'kcal' not in units[1156]['aliases'])
    check('Cal symbol alias is case-sensitive kilo', units[1156]['variant_symbol_aliases']['10'] == ['Cal']
          and 'Cal' not in units[1156].get('symbol_aliases', []))
    check('Btu_th temperature interval', fraction(units[1159]['scale']) == F('4.184') * F('453.59237') * F(5, 9))
    check('Debye rounded correspondence, not exact', units[1238]['exactness'] == 'conventional_approximate'
          and bool(units[1238].get('uncertainty_note'))
          and abs(fraction(units[1238]['scale']) - F('3.33564e-30')) <= F('5e-36'))
    check('Darcy original definition', fraction(units[1221]['scale']) == F('0.001') * F('0.000001') * F('0.01') / (F('0.0001') * 101325))
    check('gram-force kgf entry anchor', items[1122]['default_prefix'] == 10 and item_scale(1122) == F('9.80665'))
    check('dalton remains measured', units[1103]['exactness'] == 'measured' and units[1103]['uncertainty_decimal'] == '5.2e-37')
    check('historical values not exact', all(units[1000+r]['exactness'] == 'historical' for r in range(302, 307)))
    check('boiler hp approximate', units[1171]['exactness'] == 'conventional_approximate')
    check('Reaumur affine offset', units[1181]['conversion'] == 'affine' and fraction(units[1181]['offset']) == F('273.15'))
    check('thermal compounds use differences', all(5 not in [t[0] for t in item_terms(items[1000+r])]
          and 60 not in [t[0] for t in item_terms(items[1000+r])] for r in [182, 183, 184, 185, 186, 187, 188, 189, 192]))
    check('fuel consumption explicit 100km coefficient', items[1298]['coefficient'] == [1, 100, 0, 0])
    check('reactive power quantity preserved', units[1176]['quantity'] == 'reactive_power')
    check('information not ordinary scalar', units[1286]['domain'] == 'information' and units[1287]['reference'] == 'bit')
    check('rates contain typed time', all(item_terms(items[1000+r]) == [[1000+r, 0, 1], [47 if r == 294 else 3, 0, -1]] for r in range(292, 298)))

    # A symbol is case-sensitive even when language-name search is folded.
    for name, symbol in [('ampere', 'A'), ('siemens', 'S'), ('pascal', 'Pa'), ('hertz', 'Hz'), ('gal', 'Gal')]:
        check('canonical symbol ' + name, unit_keys[name]['symbol'] == symbol)
    for name, pid, symbol in [('ampere', 15, 'mA'), ('ampere', 9, 'MA'), ('hertz', 15, 'mHz'),
                              ('hertz', 9, 'MHz'), ('pascal', 0, 'Pa'), ('ampere', 18, 'pA')]:
        check('symbol prefix identity ' + symbol, prefixes[pid]['symbol'] + unit_keys[name]['prefixed_symbol'] == symbol)
    check('micron alias only micro metre', 'micron' in unit_keys['metre']['variant_aliases']['16']
          and 'micron' not in unit_keys['metre']['aliases'])
    check('fermi alias only femto metre', 'fermi' in unit_keys['metre']['variant_aliases']['19']
          and 'fermi' not in unit_keys['metre']['aliases'])

    for key, expected in ORACLES['references'].items():
        r = refs[key]
        check('CODATA value ' + key, Decimal(r['nominal_decimal']) == Decimal(expected['nominal_decimal']))
        check('CODATA uncertainty ' + key, Decimal(r['uncertainty_decimal']) == Decimal(expected['uncertainty_decimal']))
        check('CODATA exactness ' + key, r['exactness'] == expected['exactness'])
    for key, dimension in ORACLES['reference_dimensions'].items():
        check('reference dimensions ' + key, refs[key]['dimension'] == dimension)
    for key, factor in ORACLES['ratio_factors'].items():
        check('ratio factor ' + key, refs[key]['scale'] == factor)
    check('Hartree source precision', refs['hartree_energy']['nominal_decimal'] == '4.3597447222060e-18')
    check('hbar exact expression', refs['reduced_planck']['exact_scale'] == [662607015, 2, -42, -1]
          and refs['reduced_planck']['nominal_is_truncated'])
    for r in refs.values():
        check('reference source ' + r['key'], bool(data['sources'][r['source']].get('url')) and bool(r['location']))
        check('reference translation ' + r['key'], bool(r['en']) and bool(r['es']))
        check('reference notation ' + r['key'], bool(r.get('notation')))
        if r['kind'] == 'contextual':
            check('no invented factor ' + r['key'], 'scale' not in r and bool(r.get('required_context')))
        if r['kind'] != 'physical_constant' and r['key'] not in ORACLES['ratio_factors']:
            check('no nonlinear scalar factor ' + r['key'], 'scale' not in r)
        if r['kind'] == 'physical_constant' and r['key'] != 'standard_gravity':
            check('physical reference unprefixed ' + r['key'], r['prefixes'] == [0])
    for key in ('chemical_equivalent', 'osmole', 'international_activity', 'standard_gravity'):
        check('all contextual decimal multiples ' + key, refs[key]['prefixes'] == list(range(25)))
    for a, b in [('turbidity_ntu', 'turbidity_fnu'), ('american_wire_gauge', 'standard_wire_gauge'),
                 ('dots_per_inch', 'pixels_per_inch'), ('calendar_month', 'calendar_year')]:
        check('distinct contextual identities ' + a, refs[a]['id'] != refs[b]['id'])
    check('pressure tags use correct units', refs['psi_absolute']['base_unit_id'] == refs['psi_gauge']['base_unit_id'] == 55
          and refs['bar_absolute']['base_unit_id'] == refs['bar_gauge']['base_unit_id'] == 54)
    check('pressure reference retained', refs['psi_absolute']['pressure_reference'] != refs['psi_gauge']['pressure_reference'])
    with localcontext() as context:
        context.prec = 60
        c, gravity, uncertainty = Decimal(299792458), Decimal('6.67430e-11'), Decimal('1.5e-15')
        pi = Decimal('3.14159265358979323846264338327950288419716939937510582097494459')
        hbar = Decimal('6.62607015e-34') / (2 * pi)
        force, energy = c**4 / gravity, (hbar * c**5 / gravity).sqrt()
        for key, nominal, u in [('planck_force', force, force * uncertainty / gravity),
                                ('planck_energy', energy, energy * uncertainty / (2 * gravity))]:
            r = refs[key]
            check('derived provenance ' + key, r['exactness'] == 'derived' and bool(r['uncertainty_formula']))
            check('derived nominal rounding ' + key, abs(Decimal(r['nominal_decimal']) / nominal - 1) < Decimal('5e-7'))
            check('derived uncertainty rounding ' + key, abs(Decimal(r['uncertainty_decimal']) / u - 1) < Decimal('0.02'))

    # Structural namespace/coverage checks cannot pass merely because an alias exists.
    check('unit namespace width', all(0 < iid < 0x4000 for iid in items))
    check('reference namespace width', all(0 < r['id'] < 0x4000 for r in refs.values()))
    unit_namespace = {0x8000 | iid for iid in items}
    reference_namespace = {0x4000 | r['id'] for r in refs.values()}
    check('namespaces disjoint', not unit_namespace.intersection(reference_namespace))
    check('same small numeric ID remains distinct', (0x8000 | 1) != (0x4000 | refs['speed_of_light']['id']))
    expected_rows = {(r['section'], r['row']): r for r in REQUEST['rows']}
    actual_rows = {(r['section'], r['row']): r for r in data['requested_coverage']}
    check('353 source rows exactly once', len(data['requested_coverage']) == len(actual_rows) == 353
          and actual_rows.keys() == expected_rows.keys())
    bound_unit_identities = set()
    for key, original in expected_rows.items():
        row = actual_rows[key]
        section, number = key
        if section in (1, 3) and number not in (113, 225):
            iid = EXISTING_ITEMS.get(number, 1000 + number)
            check('stable item binding ' + str(key), row.get('item') == iid and iid in items)
            check('source row name unchanged ' + str(key), row.get('name') == original['name'])
            check('no coverage alias fallback ' + str(key), not row.get('reference_key') and not row.get('reference_keys') and not row.get('alias'))
            item = items[iid]
            pid = row.get('prefix', item['default_prefix'])
            identity = (0x8000 | iid, pid)
            check('distinct requested unit identity ' + str(key), identity not in bound_unit_identities)
            bound_unit_identities.add(identity)
            if number in (87, 122):
                check('base family requested kilo ' + str(key), row.get('prefix') == 10)
            for uid, term_prefix, _ in item_terms(item):
                check('constructor components exist ' + str(key), uid in units and term_prefix in units[uid]['offered_prefixes'])
        else:
            expected = ([['standard_gravity'] if number == 113 else ['elementary_charge']][0] if section == 1
                        else CONTEXT_ROWS[number-1] if section == 2 else [PHYSICAL_ROWS[number-1]])
            bound = row.get('reference_keys', [row.get('reference_key')])
            check('typed reference coverage ' + str(key), bound == expected and all(k in refs for k in bound))
    return checks


def write_coverage(data, target):
    """Render every original row, including split identities and honest data nature."""
    rows = {(r['section'], r['row']): r for r in data['requested_coverage']}
    units = {u['id']: u for u in data['units']}
    items = {i['id']: i for i in data['items']}
    refs = {r['key']: r for r in data['references']}
    def escape(value):
        return str(value).replace('|', '\\|').replace('\n', ' ')
    text = ['# UNIT-CATALOG-02: cobertura del catálogo solicitado', '',
            'Inventario generado por `python scripts/test-expanded-unit-data.py --write-coverage`. ',
            'Las 353 filas del adjunto tienen destino tipado: 301 de unidades, 31 de escalas, 9 regionales y 12 naturales. ',
            'Una fila puede desdoblarse en varias identidades cuando la tabla agrupa conceptos distintos. ',
            'Las referencias contextuales conservan sus requisitos; no se presentan como conversiones numéricas disponibles.', '',
            'La columna de identidad usa los espacios separados `U` (entrada de unidades) y `R` (referencia física/escala). ',
            'El sufijo `:pN` conserva el prefijo exacto. Los IDs son estables y no dependen de esta tabla. ',
            'La cobertura comprueba destinos estructurales; un alias de búsqueda no cuenta como implementación de otra unidad.', '',
            'Fuente del inventario de solicitud: `tests/fixtures/unit_catalog_02_requested_rows.json`, extraído del adjunto original. ',
            'SHA-256 del adjunto: `' + REQUEST['source_sha256'] + '`.', '']
    section_names = {1: 'Unidades determinadas', 2: 'Escalas y medidas con contexto', 3: 'Regional e histórico', 4: 'Referencias naturales y físicas'}
    for section in range(1, 5):
        text += ['## ' + str(section) + '. ' + section_names[section], '',
                 '| Fila | Solicitud / símbolo | Destino tipado | Naturaleza | Fuente y ubicación |',
                 '|---|---|---|---|---|']
        for original in (r for r in REQUEST['rows'] if r['section'] == section):
            row = rows[(section, original['row'])]
            targets, natures, sources = [], [], []
            if row.get('item'):
                item = items[row['item']]
                pid = row.get('prefix', item['default_prefix'])
                targets.append(f"U{item['id']}:p{pid} — {item['es']}")
                components = [units[t[0]] for t in item_terms(item)]
                natures += sorted({u['exactness'] for u in components})
                if len(item_terms(item)) > 1 or item.get('power', 1) != 1:
                    natures.append('estructura compuesta/potenciada')
                if item.get('coefficient', [1, 1, 0, 0]) != [1, 1, 0, 0]:
                    natures.append('coeficiente explícito')
                if item.get('source'):
                    sources.append((item['source'], item.get('location', '')))
                else:
                    sources += [(u['source'], u['location']) for u in components]
            else:
                for k in row.get('reference_keys', [row.get('reference_key')]):
                    r = refs[k]
                    targets.append(f"R{r['id']}:p0 — {r['es']}")
                    natures.append(r['exactness'] + ' / ' + r['kind'])
                    sources.append((r['source'], r['location']))
            source_text = []
            for key, location in dict.fromkeys(sources):
                url = data['sources'][key].get('url')
                link = f'[{escape(key)}]({url})' if url else escape(key)
                source_text.append(link + ': ' + escape(location))
            cells = [f"{section}.{original['source_row']:03d}", escape(original['name']) + ' — ' + escape(original['symbol']),
                     '<br>'.join(escape(t) for t in targets), '<br>'.join(escape(n) for n in dict.fromkeys(natures)), '<br>'.join(source_text)]
            text.append('| ' + ' | '.join(cells) + ' |')
        text.append('')
    text += ['## Alcance de las comprobaciones', '',
             'Los factores y constantes se contrastan con oráculos independientes de la tabla generada. ',
             'Los casos de prueba detectan, entre otros, pérdida de incertidumbre, un factor falso para Mach/pH/mes, ',
             'calorías confundidas, kilogramo desfasado, prefijo fuera de la potencia, alias Cal en la variante incorrecta y pérdida de cobertura. ',
             'Estas comprobaciones offline no son operaciones dimensionales que ya funcionen en Calculation.', '']
    target.write_text('\n'.join(line.rstrip() for line in text), encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--registry', type=Path, default=SOURCE)
    parser.add_argument('--write-coverage', action='store_true')
    parser.add_argument('--report', type=Path, default=ROOT / 'out/unit-catalog-02/data-regression.json')
    args = parser.parse_args()
    data = json.loads(args.registry.read_text(encoding='utf-8'))
    checks = validate(data)
    mutations = []
    def mutate(label, fn):
        broken = copy.deepcopy(data)
        fn(broken)
        try:
            validate(broken)
        except (AssertionError, KeyError, ValueError):
            mutations.append(label)
            return
        raise AssertionError('Mutation undetected: ' + label)
    def unit(d, key):
        return next(u for u in d['units'] if u['key'] == key)
    def ref(d, key):
        return next(r for r in d['references'] if r['key'] == key)
    mutate('kg factor changed', lambda d: unit(d, 'gram').__setitem__('scale', [1, 1, 0, 0]))
    mutate('cm squared flattened', lambda d: next(i for i in d['items'] if i['id'] == 101).__setitem__('power', 1))
    mutate('Wh prefixed on hour', lambda d: next(i for i in d['items'] if i['id'] == 114).__setitem__('prefix_component', 1))
    mutate('m and M conflated', lambda d: d['prefixes'][15].__setitem__('symbol', 'M'))
    mutate('IT calorie changed to th', lambda d: unit(d, 'calorie_international_table').__setitem__('scale', [523, 125, 0, 0]))
    mutate('Cal alias incorrectly base', lambda d: unit(d, 'calorie_thermochemical')['aliases'].append('Cal'))
    mutate('measured constant marked exact', lambda d: ref(d, 'bohr_radius').__setitem__('exactness', 'exact'))
    mutate('electron uncertainty lost', lambda d: ref(d, 'electron_mass').__setitem__('uncertainty_decimal', '0'))
    mutate('Mach false factor', lambda d: ref(d, 'mach_number').__setitem__('scale', [340, 1, 0, 0]))
    mutate('pH false factor', lambda d: ref(d, 'ph_activity').__setitem__('scale', [1, 1, 0, 0]))
    mutate('month fixed duration', lambda d: ref(d, 'calendar_month').__setitem__('scale', [2592000, 1, 0, 0]))
    mutate('reference source missing', lambda d: ref(d, 'specific_gravity').__setitem__('source', 'unknown'))
    mutate('translation missing', lambda d: ref(d, 'specific_gravity').__setitem__('es', ''))
    mutate('unit in reference ID range', lambda d: d['items'][0].__setitem__('id', 0x4001))
    mutate('coverage row lost', lambda d: d['requested_coverage'].pop())
    mutate('coverage points to different row', lambda d: d['requested_coverage'][0].__setitem__('item', 40))
    mutate('coverage faked by alias', lambda d: d['requested_coverage'][0].__setitem__('alias', 'Metro'))
    reordered = copy.deepcopy(data)
    for key in ('units', 'items', 'references', 'requested_coverage'):
        reordered[key].reverse()
    validate(reordered)
    assert generator.outputs(reordered) == generator.outputs(data), 'Ordering changed stable output identities'
    if args.write_coverage:
        write_coverage(data, ROOT / 'docs/UNIT_CATALOG_02_COVERAGE.md')
    report = {'passed': True, 'checks': len(checks), 'mutation_controls': mutations,
              'source_rows': 353, 'unit_definitions': len(data['units']), 'catalog_items': len(data['items']),
              'reference_definitions': len(data['references']),
              'reference_variants': sum(len(r['prefixes']) for r in data['references']),
              'registry_sha256': hashlib.sha256(args.registry.read_bytes()).hexdigest(),
              'scope': 'Independent offline data/identity/coverage checks only; no Calculation arithmetic claim.'}
    args.report.parent.mkdir(exist_ok=True, parents=True)
    args.report.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
