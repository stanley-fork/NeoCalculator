#!/usr/bin/env python3
"""Reproduce the minimal Montserrat Spanish fallback subset (lv_font_conv 1.5.3)."""
from pathlib import Path
import subprocess,shutil
ROOT=Path(__file__).resolve().parents[1]
converter=shutil.which('lv_font_conv.cmd') or shutil.which('lv_font_conv')
assert converter, 'Install the existing lv_font_conv 1.5.3 tool'
version=subprocess.check_output([converter,'--version'],text=True).strip()
assert version=='1.5.3',version
for size in [10,12,14]:
    output=ROOT/f'src/fonts/montserrat_es_{size}.c'
    subprocess.run([converter,'--font','assets/fonts/Montserrat-Regular.ttf','--size',str(size),'--bpp','4','--format','lvgl','--symbols','áéíóúüñÁÉÍÓÚÜÑ¿¡','--no-compress','--no-kerning','-o',output.relative_to(ROOT).as_posix()],cwd=ROOT,check=True)
    text=output.read_text(encoding='utf-8').replace('#include "lvgl/lvgl.h"','#include "lvgl.h"')
    header='''/*
 * Generated from assets/fonts/Montserrat-Regular.ttf.
 * Copyright 2011 The Montserrat Project Authors.
 * SPDX-License-Identifier: OFL-1.1
 * License: assets/fonts/LICENSES/Montserrat-OFL-1.1.txt
 * Regenerate: python scripts/generate_tutor_spanish_fonts.py
 * Generator: lv_font_conv 1.5.3. Spanish glyphs only; ASCII uses LVGL's font.
 */
'''
    output.write_text((header+text).rstrip()+'\n',encoding='utf-8',newline='\n')
