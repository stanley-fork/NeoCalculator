#!/usr/bin/env python3
"""Independent numeric oracles and mutation controls, never UI arithmetic claims."""
import copy, importlib.util, json
from fractions import Fraction as F
from pathlib import Path
root=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('generator',root/'scripts/generate-unit-catalog.py')
g=importlib.util.module_from_spec(spec);spec.loader.exec_module(g)
data=json.loads(g.SOURCE.read_text(encoding='utf8'))
def oracles(d):
    g.validate(d)
    units={u['key']:u for u in d['units']};prefix={p['symbol']:p['exponent'] for p in d['prefixes']}
    def scale(key,p='',power=1):
        n,den,e,pi=units[key]['scale'];assert pi==0
        return (F(n,den)*F(10)**(e+prefix[p]))**power
    assert prefix==dict(zip(['','Q','R','Y','Z','E','P','T','G','M','k','h','da','d','c','m','µ','n','p','f','a','z','y','r','q'],[0,30,27,24,21,18,15,12,9,6,3,2,1,-1,-2,-3,-6,-9,-12,-15,-18,-21,-24,-27,-30]))
    assert scale('metre','c')==F(1,100) and scale('metre','m')==F(1,1000)
    assert scale('metre','c',2)==F(1,10000) and scale('metre','c',3)==F(1,1000000)
    # The prefix belongs to the powered atom, not to a factor outside it.
    assert next(i for i in d['items'] if i['id']==101)['power']==2
    assert next(i for i in d['items'] if i['id']==102)['power']==3
    assert scale('gram','k')==1 and scale('gram','m')==F(1,1000000)
    assert scale('litre','m')==F(1,1000000)
    assert scale('minute')==60 and scale('hour')==3600
    assert scale('inch')==F(127,5000)
    assert scale('pound')==F(45359237,100000000)
    assert scale('ounce')*16==scale('pound')
    assert scale('psi')==F(8896443230521,1290320000)
    assert scale('electronvolt')==F(801088317,5*10**27)
    assert scale('metre','q',3)==F(1,10**90) and scale('metre','Q',3)==10**90
    assert units['celsius']['offset']==[27315,100,0,0]
    assert units['fahrenheit']['offset']==[45967,180,0,0] and scale('fahrenheit')==F(5,9)
    assert units['delta_fahrenheit']['offset'][0]==0
    assert units['hertz']['dimension']==units['becquerel']['dimension'] and units['hertz']['quantity']!=units['becquerel']['quantity']
    assert units['gray']['dimension']==units['sievert']['dimension'] and units['gray']['quantity']!=units['sievert']['quantity']
    assert sum(u['si']=='named_derived' for u in units.values())==22
    assert sum(u['si'] in ('base','base_family') for u in units.values())==7
    for key,symbol in [('ampere','A'),('siemens','S'),('pascal','Pa'),('hertz','Hz'),('ohm','Ω')]:assert units[key]['symbol']==symbol
    for key in ('metre','gram','second','ampere','kelvin','mole','candela'):assert len(units[key]['offered_prefixes'])==25
oracles(data)
mutations=[]
def mutation(name,fn):
    broken=copy.deepcopy(data);fn(broken)
    try:oracles(broken)
    except (AssertionError,KeyError):mutations.append(name);return
    raise AssertionError('Undetected mutation: '+name)
def unit(d,key):return next(u for u in d['units'] if u['key']==key)
mutation('kilogram factor',lambda d:unit(d,'gram')['scale'].__setitem__(2,0))
mutation('milliprefix exponent',lambda d:d['prefixes'][15].__setitem__('exponent',-2))
mutation('electrical symbol case',lambda d:unit(d,'ampere').__setitem__('symbol','a'))
mutation('missing translation',lambda d:unit(d,'metre').__setitem__('es',''))
mutation('invalid item reference',lambda d:d['items'][0].__setitem__('unit',65535))
mutation('category cycle',lambda d:d['categories'][0].__setitem__('parent',201))
mutation('temperature offset',lambda d:unit(d,'fahrenheit')['offset'].__setitem__(0,0))
mutation('prefix outside square structure',lambda d:next(i for i in d['items'] if i['id']==101).__setitem__('power',1))
mutation('invalid coherent reference',lambda d:unit(d,'metre').__setitem__('reference','unknown'))
mutation('duplicate UnitId',lambda d:d['units'][1].__setitem__('id',1))
def collide(d):
    unit(d,'metre')['aliases'].append('same alias')
    unit(d,'gram')['aliases'].append('same alias')
mutation('undeclared ambiguous alias',collide)
shuffled=copy.deepcopy(data);shuffled['units'].reverse();shuffled['items'].reverse()
assert {u['id']:u for u in shuffled['units']}=={u['id']:u for u in data['units']}
oracles(shuffled)
assert g.outputs(shuffled)==g.outputs(data), 'reordering must not change generated identities or descriptors'
print('PASS exact independent data oracles; mutations detected: '+', '.join(mutations))
