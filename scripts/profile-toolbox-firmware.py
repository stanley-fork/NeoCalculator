#!/usr/bin/env python3
"""Record local firmware identity, ELF sections and Xtensa's own stack frames.

Requires pyelftools and the installed ESP32-S3 toolchain. This does not flash,
modify an ELF or infer whole-call-stack high water from individual frames.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess
from elftools.elf.elffile import ELFFile

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--build', type=Path, required=True)
p.add_argument('--toolchain', type=Path, required=True)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--env', action='append', required=True)
a = p.parse_args()
a.out.mkdir(parents=True, exist_ok=True)
flash = ('.iram0.text', '.iram0.vectors', '.dram0.data', '.flash.text', '.flash.rodata')
ram = ('.dram0.data', '.dram0.bss', '.noinit')
records = []
suffix = '.exe' if (a.toolchain/'xtensa-esp32s3-elf-nm.exe').exists() else ''

def tool(name, *args):
    return subprocess.check_output([str(a.toolchain/('xtensa-esp32s3-elf-'+name+suffix)),
                                    *map(str, args)], text=True)

def identity(path):
    return dict(bytes=path.stat().st_size, sha256=hashlib.sha256(path.read_bytes()).hexdigest())

for env in a.env:
    base = a.build/env
    elf_path = base/'firmware.elf'
    image_path = base/'firmware.bin'
    record = dict(environment=env, elf=identity(elf_path), image=identity(image_path))
    with elf_path.open('rb') as stream:
        elf = ELFFile(stream)
        sections = {s.name: s['sh_size'] for s in elf.iter_sections()}
        record.update(linkedFlash=sum(sections.get(s, 0) for s in flash),
                      staticRam=sum(sections.get(s, 0) for s in ram),
                      iramText=sections['.iram0.text'], sections=sections)
        if env == 'numos-esp32-s3-wroom-1u-n16r8':
            sizes = {}
            for cu in elf.get_dwarf_info().iter_CUs():
                name = cu.get_top_DIE().attributes.get('DW_AT_name')
                if not name or not any(part in name.value.decode(errors='replace') for part in ('Toolbox', 'CursorController', 'MathAST.cpp','Quantity.cpp','CalculationApp.cpp','CalculationEngine.cpp')):
                    continue
                for die in cu.iter_DIEs():
                    attrs = die.attributes
                    if die.tag not in ('DW_TAG_structure_type', 'DW_TAG_class_type'):
                        continue
                    if 'DW_AT_name' not in attrs or 'DW_AT_byte_size' not in attrs:
                        continue
                    name = attrs['DW_AT_name'].value.decode(errors='replace')
                    if name in ('Session', 'Store', 'Identity', 'Entry', 'Prepared', 'Provider', 'Level', 'NodeRow', 'NodeParen', 'NodeSpecialValue', 'NodeUnit', 'NodeQuantityReference', 'Atom', 'ReferenceAtom', 'Definition', 'Reference', 'Prefix', 'Item','Value','Dimension','Descriptor','Display','Walker','Sink','CalculationEvaluation','HistoryEntry','SessionExact','CalculationApp'):
                        sizes[name] = attrs['DW_AT_byte_size'].value
            record['types'] = sizes
    if env == 'numos-esp32-s3-wroom-1u-n16r8':
        names = []
        for line in tool('nm', '--defined-only', elf_path).splitlines():
            fields = line.split()
            if len(fields) == 3 and fields[1] in ('t', 'T', 'w', 'W') and (
                    'toolbox' in fields[2].lower() or 'quantity' in fields[2] or 'commitResultAns' in fields[2] or 'publishOutput' in fields[2] or 'loadHistoryEntry' in fields[2] or 'QuantityReference' in fields[2] or 'NodeUnit' in fields[2] or 'scanUnits' in fields[2] or 'makeUnit' in fields[2] or 'insertPrepared' in fields[2] or 'insertPower' in fields[2] or 'measureMathAtom' in fields[2] or 'layoutTextAtom' in fields[2] or ('calculateLayout' in fields[2] and ('NodeRow' in fields[2] or 'NodeParen' in fields[2]))):
                names.append(fields[2])
        frames = []
        for name in names:
            # WHY: --disassemble expects the mangled name without -C. The
            # immediate is often hexadecimal; parsing decimal alone yields 0.
            disasm = tool('objdump', '-d', '--disassemble='+name, elf_path)
            match = re.search(r'\bentry\s+a1,\s*(0x[0-9a-fA-F]+|\d+)', disasm)
            frames.append(dict(symbol=name, name=tool('c++filt', name).strip(),
                               ownFrameBytes=int(match[1], 0) if match else None))
        record['ownStackFrames'] = sorted(frames, key=lambda r: r['ownFrameBytes'] or 0, reverse=True)
    target = a.out/env
    target.mkdir(exist_ok=True)
    shutil.copy2(image_path, target/'firmware.bin')
    (target/'identity.json').write_text(json.dumps(record, indent=2)+'\n')
    records.append(record)
    print(env, 'image', record['image']['bytes'], 'flash', record['linkedFlash'],
          'RAM', record['staticRam'], 'IRAM text', record['iramText'])
(a.out/'resources-firmware.json').write_text(json.dumps(records, indent=2)+'\n')
