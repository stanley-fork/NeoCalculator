# UNIT-CATALOG-02: cobertura del catálogo solicitado

Inventario generado por `python scripts/test-expanded-unit-data.py --write-coverage`.
Las 353 filas del adjunto tienen destino tipado: 301 de unidades, 31 de escalas, 9 regionales y 12 naturales.
Una fila puede desdoblarse en varias identidades cuando la tabla agrupa conceptos distintos.
Las referencias contextuales conservan sus requisitos; no se presentan como conversiones numéricas disponibles.

La columna de identidad usa los espacios separados `U` (entrada de unidades) y `R` (referencia física/escala).
El sufijo `:pN` conserva el prefijo exacto. Los IDs son estables y no dependen de esta tabla.
La cobertura comprueba destinos estructurales; un alias de búsqueda no cuenta como implementación de otra unidad.

Fuente del inventario de solicitud: `tests/fixtures/unit_catalog_02_requested_rows.json`, extraído del adjunto original.
SHA-256 del adjunto: `3dbea454b403e74ba0c2f75d464fef0143a5593f958b2481c4a23d0bfcb7465a`.

## 1. Unidades determinadas

| Fila | Solicitud / símbolo | Destino tipado | Naturaleza | Fuente y ubicación |
|---|---|---|---|---|
| 1.001 | Metro — m | U1:p0 — Metro | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.002 | Pulgada internacional — in / ″ | U40:p0 — Pulgada internacional | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.003 | Pie internacional — ft / ′ | U41:p0 — Pie internacional | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.004 | Yarda internacional — yd | U42:p0 — Yarda internacional | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.005 | Milla terrestre internacional — mi | U43:p0 — Milla internacional | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.006 | Mil / thou — mil / thou | U1006:p0 — Mil / thou | exact | [ucum22](https://ucum.org/ucum): [mil_i] |
| 1.007 | Milla náutica internacional — nmi / NM | U44:p0 — Milla náutica | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.008 | Braza / fathom — ftm | U1008:p0 — Braza internacional | exact | [ucum22](https://ucum.org/ucum): [fth_i] |
| 1.009 | Cable, convención de un décimo de milla náutica — — | U1009:p0 — Cable (0,1 nmi) | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): cable length185.2m |
| 1.010 | Cable, convención estadounidense de 720 pies — — | U1010:p0 — Cable (EE. UU., 720 ft) | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): cable720internationalfeet |
| 1.011 | Cadena / chain — ch | U1011:p0 — Cadena internacional | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): International foot-based relation; 66ft |
| 1.012 | Eslabón / link — link | U1012:p0 — Eslabón internacional | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): International foot-based relation; 33/50ft |
| 1.013 | Rod / pole / perch — rd | U1013:p0 — Rod internacional | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): International foot-based relation; 33/2ft |
| 1.014 | Furlong — fur | U1014:p0 — Furlong internacional | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): International foot-based relation; 660ft |
| 1.015 | Hand / mano ecuestre — hand / hh | U1015:p0 — Mano ecuestre | exact | [ucum22](https://ucum.org/ucum): International foot-based relation; 1/3ft |
| 1.016 | Pie topográfico estadounidense histórico — ft (US survey) | U1016:p0 — Pie topográfico EE. UU. | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): US survey foot1200/3937m; obsolete after2022 |
| 1.017 | Unidad astronómica — au / UA | U64:p0 — Unidad astronómica | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.018 | Año luz — ly | U1018:p0 — Año luz (juliano) | exact | [ucum22](https://ucum.org/ucum): [ly]=[c].a_j |
| 1.019 | Pársec — pc | U1019:p0 — Pársec | exact | [iau2015](https://arxiv.org/abs/1510.06262): Note4 exact648000au/pi |
| 1.020 | Segundo luz — — | U1020:p0 — Segundo luz | exact | [bipm](https://www.bipm.org/en/si-brochure-9): Defining speed of light times1second |
| 1.021 | Ángstrom — Å | U1021:p0 — Ángstrom | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): angstrom |
| 1.022 | Punto tipográfico DTP / PostScript — pt | U1022:p0 — Punto DTP | exact | [css4](https://www.w3.org/TR/2024/WD-css-values-4-20240312/): Absolute length:1pt=1/72in |
| 1.023 | Pica DTP — pc (tipografía) | U1023:p0 — Pica DTP | exact | [css4](https://www.w3.org/TR/2024/WD-css-values-4-20240312/): Absolute length:1pc=1/6in |
| 1.024 | Unidad de rack — U | U1024:p0 — Unidad de rack | exact | [rack](https://www.dell.com/support/kbdoc/en-rs/000144807/me4-locating-the-service-tag): Nominal1.75in rack height |
| 1.025 | French / Charrière — Fr / Ch | U1025:p0 — French / Charrière | exact | [ucum22](https://ucum.org/ucum): [Ch]=mm/3; diameter, not area |
| 1.026 | Metro cuadrado — m² | U101:p0 — Metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.027 | Área — a | U1027:p0 — Área | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): are100m²; hectare remains stable existing identity |
| 1.028 | Hectárea — ha | U45:p0 — Hectárea | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.029 | Acre internacional — ac | U1029:p0 — Acre internacional | exact | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): International acre43560ft² |
| 1.030 | Pulgada cuadrada — in² | U1030:p0 — Pulgada cuadrada | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.031 | Pie cuadrado — ft² | U1031:p0 — Pie cuadrado | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.032 | Yarda cuadrada — yd² | U1032:p0 — Yarda cuadrada | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.033 | Milla cuadrada — mi² | U1033:p0 — Milla cuadrada | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.034 | Barn / barnio — b | U1034:p0 — Barnio | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): barn |
| 1.035 | Circular mil — cmil | U1035:p0 — Circular mil | exact | [ucum22](https://ucum.org/ucum): [cml_i]=pi/4*[mil_i]^2 |
| 1.036 | Metro cúbico — m³ | U102:p0 — Metro cúbico | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.037 | Litro — L / l | U46:p0 — Litro | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.038 | Pulgada cúbica — in³ | U1038:p0 — Pulgada cúbica | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.039 | Pie cúbico — ft³ | U1039:p0 — Pie cúbico | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.040 | Yarda cúbica — yd³ | U1040:p0 — Yarda cúbica | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.041 | Galón estadounidense líquido — gal (US liq) | U1041:p0 — Galón líquido EE. UU. | exact | [ucum22](https://ucum.org/ucum): [gal_us]=231in³ |
| 1.042 | Galón imperial — gal (imp) | U1042:p0 — Galón imperial | exact | [ucum22](https://ucum.org/ucum): [gal_br]=4.54609L |
| 1.043 | Cuarto estadounidense líquido — qt (US liq) | U1043:p0 — Cuarto líquido EE. UU. | exact | [ucum22](https://ucum.org/ucum): [qt_us] |
| 1.044 | Cuarto imperial — qt (imp) | U1044:p0 — Cuarto imperial | exact | [ucum22](https://ucum.org/ucum): [qt_br] |
| 1.045 | Pinta estadounidense líquida — pt (US liq) | U1045:p0 — Pinta líquida EE. UU. | exact | [ucum22](https://ucum.org/ucum): [pt_us] |
| 1.046 | Pinta imperial — pt (imp) | U1046:p0 — Pinta imperial | exact | [ucum22](https://ucum.org/ucum): [pt_br] |
| 1.047 | Onza líquida estadounidense — fl oz (US) | U1047:p0 — Onza líquida EE. UU. | exact | [ucum22](https://ucum.org/ucum): [foz_us] |
| 1.048 | Onza líquida imperial — fl oz (imp) | U1048:p0 — Onza líquida imperial | exact | [ucum22](https://ucum.org/ucum): [foz_br] |
| 1.049 | Gill estadounidense — gi (US) | U1049:p0 — Gill EE. UU. | exact | [ucum22](https://ucum.org/ucum): [gil_us] |
| 1.050 | Gill imperial — gi (imp) | U1050:p0 — Gill imperial | exact | [ucum22](https://ucum.org/ucum): [gil_br] |
| 1.051 | Galón estadounidense de áridos — gal (US dry) | U1051:p0 — Galón áridos EE. UU. | exact | [ucum22](https://ucum.org/ucum): [gal_wi]=[bu_us]/8 |
| 1.052 | Cuarto estadounidense de áridos — qt (US dry) | U1052:p0 — Cuarto áridos EE. UU. | exact | [ucum22](https://ucum.org/ucum): [dqt_us] |
| 1.053 | Pinta estadounidense de áridos — pt (US dry) | U1053:p0 — Pinta áridos EE. UU. | exact | [ucum22](https://ucum.org/ucum): [dpt_us] |
| 1.054 | Bushel estadounidense — bu (US) | U1054:p0 — Bushel EE. UU. | exact | [ucum22](https://ucum.org/ucum): [bu_us]=2150.42in³ |
| 1.055 | Bushel imperial — bu (imp) | U1055:p0 — Bushel imperial | exact | [ucum22](https://ucum.org/ucum): [bu_br] |
| 1.056 | Peck estadounidense — pk (US) | U1056:p0 — Peck EE. UU. | exact | [ucum22](https://ucum.org/ucum): [pk_us] |
| 1.057 | Peck imperial — pk (imp) | U1057:p0 — Peck imperial | exact | [ucum22](https://ucum.org/ucum): [pk_br] |
| 1.058 | Barril de petróleo — bbl | U1058:p0 — Barril de petróleo | exact | [ucum22](https://ucum.org/ucum): [bbl_us] |
| 1.059 | Acre-pie internacional — ac·ft | U1059:p0 — Acre-pie internacional | exact<br>estructura compuesta/potenciada | [nist_foot](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors): SI coherent product of identified components |
| 1.060 | Pie tablar / board foot — board ft | U1060:p0 — Pie tablar | exact | [ucum22](https://ucum.org/ucum): [bf_i]=144in³ |
| 1.061 | Cord — cord | U1061:p0 — Cord (madera apilada) | exact | [ucum22](https://ucum.org/ucum): [cr_i] |
| 1.062 | Estéreo — st (volumen) | U1062:p0 — Estéreo (madera apilada) | exact | [ucum22](https://ucum.org/ucum): st |
| 1.063 | Tonelada de registro — register ton | U1063:p0 — Tonelada de registro | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): NIST register ton100ft³ |
| 1.064 | Cucharadita estadounidense habitual — tsp (US) | U1064:p0 — Cucharadita EE. UU. | exact | [ucum22](https://ucum.org/ucum): [tsp_us] |
| 1.065 | Cucharada estadounidense habitual — tbsp (US) | U1065:p0 — Cucharada EE. UU. | exact | [ucum22](https://ucum.org/ucum): [tbs_us] |
| 1.066 | Taza estadounidense habitual — cup (US) | U1066:p0 — Taza habitual EE. UU. | exact | [ucum22](https://ucum.org/ucum): [cup_us] |
| 1.067 | Cucharadita métrica — tsp (métrica) | U1067:p0 — Cucharadita métrica (5mL) | exact | [fda1019](https://www.ecfr.gov/current/title-21/chapter-I/subchapter-B/part-101/subpart-A/section-101.9): b5viii5mL convention |
| 1.068 | Cucharada métrica de 15 mL — tbsp (15 mL) | U1068:p0 — Cucharada métrica (15mL) | exact | [fda1019](https://www.ecfr.gov/current/title-21/chapter-I/subchapter-B/part-101/subpart-A/section-101.9): b5viii15mL convention |
| 1.069 | Cucharada australiana — tbsp (Australia) | U1069:p0 — Cucharada australiana | exact | [cooking_au](https://www.nigella.com/ask/weights-and-measures-for-australia): Australian20mL convention |
| 1.070 | Taza métrica — cup (métrica) | U1070:p0 — Taza métrica (250mL) | exact | [cooking_au](https://www.nigella.com/ask/weights-and-measures-for-australia): Metric250mL convention |
| 1.071 | Taza de etiquetado nutricional estadounidense — cup (FDA) | U1071:p0 — Taza FDA (240mL) | exact | [fda1019](https://www.ecfr.gov/current/title-21/chapter-I/subchapter-B/part-101/subpart-A/section-101.9): b5viii240mL |
| 1.072 | Segundo — s | U3:p0 — Segundo | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.073 | Minuto — min | U47:p0 — Minuto | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.074 | Hora — h | U48:p0 — Hora | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.075 | Día como unidad de duración — d | U49:p0 — Día | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.076 | Semana — — / wk | U1076:p0 — Semana (7 días) | exact | [ucum22](https://ucum.org/ucum): wk=7d |
| 1.077 | Año juliano — a (juliano) | U1077:p0 — Año juliano | exact | [ucum22](https://ucum.org/ucum): a_j=365.25d |
| 1.078 | Shake — shake | U1078:p0 — Shake | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): shake |
| 1.079 | Radián — rad | U8:p0 — Radián | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.080 | Grado sexagesimal — ° | U57:p0 — Grado angular | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.081 | Minuto de arco — ′ / arcmin | U58:p0 — Minuto de arco | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.082 | Segundo de arco — ″ / arcsec | U59:p0 — Segundo de arco | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.083 | Gon / grado centesimal — gon | U1083:p0 — Gon / grado centesimal | exact | [ucum22](https://ucum.org/ucum): gon |
| 1.084 | Vuelta / revolución — rev / turn | U1084:p0 — Vuelta / revolución | exact | [ucum22](https://ucum.org/ucum): circ=2pi rad |
| 1.085 | Estereorradián — sr | U9:p0 — Estereorradián | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.086 | Grado cuadrado — deg² | U1086:p0 — Grado cuadrado (ángulo sólido) | exact | [bipm](https://www.bipm.org/en/si-brochure-9): sr=rad²; (pi/180)²sr |
| 1.087 | Kilogramo — kg | U2:p10 — Gramo | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §3, p.139, kilogram exception |
| 1.088 | Libra avoirdupois — lb | U50:p0 — Libra avoirdupois | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.089 | Onza avoirdupois — oz | U51:p0 — Onza avoirdupois | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.090 | Stone — st (masa) | U1090:p0 — Stone (masa) | exact | [ucum22](https://ucum.org/ucum): [stone_av] |
| 1.091 | Grano — gr | U1091:p0 — Grano | exact | [ucum22](https://ucum.org/ucum): [gr] |
| 1.092 | Quilate métrico — ct | U1092:p0 — Quilate métrico | exact | [ucum22](https://ucum.org/ucum): [car_m] |
| 1.093 | Onza troy — oz t / ozt | U1093:p0 — Onza troy | exact | [ucum22](https://ucum.org/ucum): [oz_tr] |
| 1.094 | Libra troy — lb t | U1094:p0 — Libra troy | exact | [ucum22](https://ucum.org/ucum): [lb_tr] |
| 1.095 | Pennyweight — dwt | U1095:p0 — Pennyweight | exact | [ucum22](https://ucum.org/ucum): [pwt_tr] |
| 1.096 | Tonelada métrica — t | U52:p0 — Tonelada | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.097 | Tonelada corta estadounidense — short ton / sh tn | U1097:p0 — Tonelada corta EE. UU. | exact | [ucum22](https://ucum.org/ucum): [ston_av] |
| 1.098 | Tonelada larga imperial — long ton / lg tn | U1098:p0 — Tonelada larga imperial | exact | [ucum22](https://ucum.org/ucum): [lton_av] |
| 1.099 | Hundredweight estadounidense — cwt (US) | U1099:p0 — Hundredweight EE. UU. | exact | [ucum22](https://ucum.org/ucum): [scwt_av] |
| 1.100 | Hundredweight imperial — cwt (imp) | U1100:p0 — Hundredweight imperial | exact | [ucum22](https://ucum.org/ucum): [lcwt_av] |
| 1.101 | Quintal métrico — q | U1101:p0 — Quintal métrico (100kg) | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): 100kg named convention |
| 1.102 | Slug — slug | U1102:p0 — Slug | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): lbf*s²/ft |
| 1.103 | Dalton / unidad de masa atómica unificada — Da / u | U1103:p0 — Dalton / masa atómica unificada | measured | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): atomic mass constant2022 |
| 1.104 | Electronvoltio dividido por c² — eV/c² | U1104:p0 — Electronvoltio dividido por c² | exact | [bipm](https://www.bipm.org/en/si-brochure-9): eV/c² using exact c and elementary charge |
| 1.105 | Metro por segundo — m/s | U110:p0 — Metro por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.106 | Kilómetro por hora — km/h | U111:p0 — Kilómetro por hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre<br>[bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.107 | Milla por hora — mph / mi/h | U1107:p0 — Milla por hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.108 | Pie por segundo — ft/s | U1108:p0 — Pie por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.109 | Pie por minuto — ft/min | U1109:p0 — Pie por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.110 | Nudo — kn / kt | U120:p0 — Nudo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.111 | Metro por segundo cuadrado — m/s² | U112:p0 — Metro por segundo cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.112 | Pie por segundo cuadrado — ft/s² | U1112:p0 — Pie por segundo cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.113 | Gravedad estándar como referencia de aceleración — g₀ / gₙ | R13:p0 — Gravedad estándar | exact / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): standard acceleration of gravity |
| 1.114 | Gal / galileo — Gal | U1114:p0 — Gal / galileo | exact | [ucum22](https://ucum.org/ucum): Gal=cm/s² |
| 1.115 | Metro por segundo cúbico — m/s³ | U1115:p0 — Metro por segundo cúbico | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.116 | Hercio / hertz — Hz | U10:p0 — Hercio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.117 | Revolución por minuto — rpm / r/min | U1117:p0 — Revolución por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.118 | Radián por segundo — rad/s | U1118:p0 — Radián por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.119 | Radián por segundo cuadrado — rad/s² | U1119:p0 — Radián por segundo cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.120 | Newton — N | U11:p0 — Newton | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.121 | Dina — dyn | U1121:p0 — Dina | exact | [ucum22](https://ucum.org/ucum): dyn=g*cm/s² |
| 1.122 | Kilogramo-fuerza / kilopondio — kgf / kp | U1122:p10 — Gramo-fuerza | exact | [ucum22](https://ucum.org/ucum): gf=g*standard gravity |
| 1.123 | Libra-fuerza — lbf | U65:p0 — Libra-fuerza | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B.8 pound-force; BIPM Appendix CGPM 1901, standard g |
| 1.124 | Onza-fuerza — ozf | U1124:p0 — Onza-fuerza | exact | [ucum22](https://ucum.org/ucum): lbf/16 |
| 1.125 | Poundal — pdl | U1125:p0 — Poundal | exact | [ucum22](https://ucum.org/ucum): lb*ft/s² |
| 1.126 | Kip — kip | U1126:p0 — Kip (1000lbf) | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): kip1000lbf |
| 1.127 | Tonelada-fuerza métrica — tf | U1127:p0 — Tonelada-fuerza métrica | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): 1000kgf |
| 1.128 | Newton-metro — N·m | U119:p0 — Newton metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.129 | Libra-fuerza pie — lbf·ft | U1129:p0 — Libra-fuerza pie | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.130 | Libra-fuerza pulgada — lbf·in | U1130:p0 — Libra-fuerza pulgada | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.131 | Onza-fuerza pulgada — ozf·in | U1131:p0 — Onza-fuerza pulgada | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.132 | Kilogramo-fuerza metro — kgf·m / kp·m | U1132:p10 — Kilogramo-fuerza metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.133 | Newton-segundo — N·s | U1133:p0 — Newton segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.134 | Libra-fuerza segundo — lbf·s | U1134:p0 — Libra-fuerza segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.135 | Julio-segundo — J·s | U1135:p0 — Julio segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.136 | Kilogramo metro cuadrado — kg·m² | U1136:p10 — Kilogramo metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.137 | Libra pie cuadrado — lb·ft² | U1137:p0 — Libra pie cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.138 | Newton por metro — N/m | U1138:p0 — Newton por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.139 | Libra-fuerza por pulgada — lbf/in | U1139:p0 — Libra-fuerza por pulgada | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.140 | Metro a la cuarta — m⁴ | U1140:p0 — Metro a la cuarta | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.141 | Pulgada a la cuarta — in⁴ | U1141:p0 — Pulgada a la cuarta | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.142 | Pascal — Pa | U12:p0 — Pascal | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.143 | Bar — bar | U54:p0 — Bar | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.144 | Atmósfera estándar — atm | U53:p0 — Atmósfera estándar | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Appendix B.8, named row; exact definition |
| 1.145 | Atmósfera técnica — at | U1145:p0 — Atmósfera técnica | exact | [ucum22](https://ucum.org/ucum): att=kgf/cm² |
| 1.146 | Torr — Torr | U1146:p0 — Torr | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): torr=atm/760 |
| 1.147 | Milímetro de mercurio convencional — mmHg | U1147:p0 — Milímetro Hg convencional | exact | [cldr_units](https://github.com/unicode-org/cldr/blob/main/common/supplemental/units.xml): NIST B8 note12 + CLDR density13595.1kg/m³ and g_n; height1mm |
| 1.148 | Metro de columna de agua convencional — mH₂O / mca | U1148:p0 — Metro agua convencional | exact | [ucum22](https://ucum.org/ucum): m[H2O]=9.80665kPa |
| 1.149 | Pulgada de agua convencional — inH₂O | U1149:p0 — Pulgada agua convencional | exact | [ucum22](https://ucum.org/ucum): [in_i'H2O] |
| 1.150 | Pulgada de mercurio convencional — inHg | U1150:p0 — Pulgada Hg convencional | exact | [cldr_units](https://github.com/unicode-org/cldr/blob/main/common/supplemental/units.xml): NIST B8 note12 + CLDR density13595.1kg/m³ and g_n; height1in |
| 1.151 | Libra-fuerza por pulgada cuadrada — psi | U55:p0 — Libra-fuerza por pulgada cuadrada | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B.8 pound-force and inch; BIPM Appendix CGPM 1901, standard g |
| 1.152 | Libra-fuerza por pie cuadrado — psf | U1152:p0 — Libra-fuerza por pie cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.153 | Baria / barye — Ba | U1153:p0 — Baria | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): dyn/cm² |
| 1.154 | Julio / joule — J | U13:p0 — Julio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.155 | Vatio-hora — Wh | U114:p0 — Vatio hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4<br>[bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.156 | Caloría termoquímica — cal_th | U1156:p0 — Caloría termoquímica | exact | [ucum22](https://ucum.org/ucum): cal_th=4.184J |
| 1.157 | Caloría de la Tabla Internacional — cal_IT | U1157:p0 — Caloría Tabla Internacional | exact | [ucum22](https://ucum.org/ucum): cal_IT=4.1868J |
| 1.158 | Unidad térmica británica, Tabla Internacional — Btu_IT | U1158:p0 — Btu Tabla Internacional | exact | [ucum22](https://ucum.org/ucum): [Btu_IT]=1.05505585262kJ |
| 1.159 | Unidad térmica británica termoquímica — Btu_th | U1159:p0 — Btu termoquímica | exact | [nist811_notes](https://www.nist.gov/pml/special-publication-811/nist-guide-si-footnotes): Footnote9: thermochemical calorie4.184J, pound453.59237g, Fahrenheit interval5/9K; exact relation |
| 1.160 | Ergio — erg | U1160:p0 — Ergio | exact | [ucum22](https://ucum.org/ucum): erg=dyn*cm |
| 1.161 | Electronvoltio — eV | U56:p0 — Electronvoltio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.162 | Pie libra-fuerza — ft·lbf | U1162:p0 — Pie libra-fuerza (energía) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.163 | Therm estadounidense — therm (US) | U1163:p0 — Therm (EE. UU.) | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 thermUS exact bold factor |
| 1.164 | Therm, convención EC — therm (EC) | U1164:p0 — Therm (CE) | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 thermEC exact bold factor |
| 1.165 | Tonelada equivalente de petróleo — tep / toe | U1165:p0 — Tonelada equivalente petróleo | exact | [energy_uk](https://assets.publishing.service.gov.uk/media/5a81d77fe5274a2e87dbfc46/Annex_A-D.pdf): AnnexA statistical41.868GJ |
| 1.166 | Tonelada equivalente de TNT — t TNT | U1166:p0 — Tonelada equivalente TNT | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 tonTNT=4.184GJ |
| 1.167 | Vatio / watt — W | U14:p0 — Vatio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.168 | Horsepower mecánico — hp | U1168:p0 — Horsepower mecánico | exact | [ucum22](https://ucum.org/ucum): [HP]=550ft*lbf/s |
| 1.169 | Caballo de vapor / horsepower métrico — CV / PS | U1169:p0 — Caballo de vapor métrico | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): 75kgf*m/s |
| 1.170 | Horsepower eléctrico convencional — hp (electric) | U1170:p0 — Horsepower eléctrico (746W) | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 exact electrical horsepower746W |
| 1.171 | Boiler horsepower — hp (boiler) | U1171:p0 — Horsepower de caldera | conventional_approximate | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 rounded boiler hp9809.50W |
| 1.172 | BTU de Tabla Internacional por hora — Btu_IT/h | U1172:p0 — Btu IT por hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.173 | Tonelada de refrigeración estadounidense — TR | U1173:p0 — Tonelada refrigeración EE. UU. | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): 12000Btu_IT/h |
| 1.174 | Frigoría por hora — frig/h | U1174:p10 — Frigoría por hora (kcal IT) | exact<br>estructura compuesta/potenciada | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): Explicit convention kcal_IT/h, cooling magnitude |
| 1.175 | Voltio-amperio — VA | U1175:p0 — Voltio amperio (aparente) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.176 | Var — var | U1176:p0 — Var (potencia reactiva) | exact | [bipm](https://www.bipm.org/en/si-brochure-9): Special name for reactive power; same SI dimension asW |
| 1.177 | Kelvin — K | U5:p0 — Kelvin | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, kelvin |
| 1.178 | Grado Celsius — °C | U23:p0 — Grado Celsius | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.179 | Grado Fahrenheit — °F | U60:p0 — Grado Fahrenheit | exact | [nist_temperature](https://www.nist.gov/pml/owm/si-units-temperature): Exact conversion equations, Fahrenheit to kelvin |
| 1.180 | Grado Rankine — °R | U1180:p0 — Grado Rankine | exact | [ucum22](https://ucum.org/ucum): [degR]=5K/9 |
| 1.181 | Grado Réaumur — °Ré | U1181:p0 — Grado Réaumur | exact | [ucum22](https://ucum.org/ucum): [degRe] scale5/4K origin273.15K |
| 1.182 | Julio por kelvin — J/K | U1182:p0 — Julio por kelvin | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.183 | Julio por kilogramo kelvin — J/(kg·K) | U1183:p0 — Julio por kilogramo kelvin | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.184 | BTU_IT por libra y grado Fahrenheit de diferencia — Btu_IT/(lb·°F) | U1184:p0 — Btu IT por libra delta F | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.185 | Vatio por metro kelvin — W/(m·K) | U1185:p0 — Vatio por metro kelvin | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.186 | BTU_IT pie por hora, pie cuadrado y grado Fahrenheit de diferencia — Btu_IT·ft/(h·ft²·°F) | U1186:p0 — Btu IT por hora pie delta F | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.187 | Kelvin por vatio — K/W | U1187:p0 — Kelvin por vatio | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.188 | Metro cuadrado kelvin por vatio — m²·K/W | U1188:p0 — Metro cuadrado kelvin por vatio | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.189 | Unidad estadounidense de valor R — h·ft²·°F/Btu_IT | U1189:p0 — Valor R estadounidense | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.190 | Clo — clo | U1190:p0 — Clo | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 clo0.155m²K/W |
| 1.191 | Tog — tog | U1191:p0 — Tog | exact | [tog_iom](https://www.iom.int/sites/g/files/tmzbdl486/files/annex_1bis_-_iom_synthetic_blanket_h_technical_specifications_and_aql.pdf): tog |
| 1.192 | Vatio por metro cuadrado kelvin — W/(m²·K) | U1192:p0 — Vatio por metro cuadrado kelvin | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.193 | Kelvin inverso — K⁻¹ | U1193:p0 — Kelvin inverso | exact<br>estructura compuesta/potenciada | [nist_temperature](https://www.nist.gov/pml/owm/si-units-temperature): Temperature intervals |
| 1.194 | Kilogramo por metro cúbico — kg/m³ | U118:p0 — Kilogramo por metro cúbico | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §3, p.139, kilogram exception<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.195 | Libra por pie cúbico — lb/ft³ | U1195:p0 — Libra por pie cúbico | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.196 | Libra por pulgada cúbica — lb/in³ | U1196:p0 — Libra por pulgada cúbica | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.197 | Libra por galón estadounidense líquido — lb/gal (US liq) | U1197:p0 — Libra por galón líquido EE. UU. | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.198 | Libra por galón imperial — lb/gal (imp) | U1198:p0 — Libra por galón imperial | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.199 | Kilogramo por metro cuadrado — kg/m² | U1199:p10 — Kilogramo por metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.200 | Onza por yarda cuadrada — oz/yd² | U1200:p0 — Onza por yarda cuadrada | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.201 | Kilogramo por metro — kg/m | U1201:p10 — Kilogramo por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.202 | Tex — tex | U1202:p0 — Tex | exact | [ucum22](https://ucum.org/ucum): tex=g/km |
| 1.203 | Denier — den | U1203:p0 — Denier | exact | [ucum22](https://ucum.org/ucum): [den]=g/(9000m) |
| 1.204 | Metro cúbico por segundo — m³/s | U1204:p0 — Metro cúbico por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.205 | Litro por segundo — L/s | U1205:p0 — Litro por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.206 | Litro por minuto — L/min | U1206:p0 — Litro por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.207 | Pie cúbico por segundo — ft³/s / cfs | U1207:p0 — Pie cúbico por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.208 | Pie cúbico por minuto — ft³/min / cfm | U1208:p0 — Pie cúbico por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.209 | Galón estadounidense líquido por minuto — gpm (US) | U1209:p0 — Galón líquido EE. UU. por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.210 | Galón imperial por minuto — gpm (imp) | U1210:p0 — Galón imperial por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.211 | Kilogramo por segundo — kg/s | U1211:p10 — Kilogramo por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.212 | Libra por hora — lb/h | U1212:p0 — Libra por hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.213 | Pascal-segundo — Pa·s | U1213:p0 — Pascal segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.214 | Poise — P | U1214:p0 — Poise | exact | [ucum22](https://ucum.org/ucum): P=dyn*s/cm² |
| 1.215 | Metro cuadrado por segundo — m²/s | U1215:p0 — Metro cuadrado por segundo (viscosidad) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.216 | Stokes — St | U1216:p0 — Stokes | exact | [ucum22](https://ucum.org/ucum): St=cm²/s |
| 1.217 | Metro cuadrado por segundo — m²/s | U1217:p0 — Metro cuadrado por segundo (difusividad) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.218 | Newton por metro — N/m | U1218:p0 — Newton por metro (tensión superficial) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.219 | Dina por centímetro — dyn/cm | U1219:p0 — Dina por centímetro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.220 | Metro cuadrado — m² | U1220:p0 — Metro cuadrado (permeabilidad) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.221 | Darcy — D (darcy) | U1221:p0 — Darcy (permeabilidad porosa) | exact | [usgs_darcy](https://pubs.usgs.gov/wsp/0887/report.pdf): p9: flow1cm³/s through1cm² of1cP fluid over1cm under1atm |
| 1.222 | Amperio — A | U4:p0 — Amperio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, ampere |
| 1.223 | Culombio / coulomb — C | U15:p0 — Culombio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.224 | Amperio-hora — Ah | U113:p0 — Amperio hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, ampere<br>[bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.225 | Carga elemental — e | R14:p0 — Carga elemental | exact / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): elementary charge |
| 1.226 | Voltio — V | U16:p0 — Voltio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.227 | Ohmio — Ω | U18:p0 — Ohmio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.228 | Siemens — S | U19:p0 — Siemens | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.229 | Faradio — F | U17:p0 — Faradio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.230 | Henrio — H | U22:p0 — Henrio | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.231 | Ohmio metro — Ω·m | U1231:p0 — Ohmio metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.232 | Siemens por metro — S/m | U1232:p0 — Siemens por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.233 | Voltio por metro — V/m | U1233:p0 — Voltio por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.234 | Amperio por metro cuadrado — A/m² | U1234:p0 — Amperio por metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.235 | Culombio por metro cuadrado — C/m² | U1235:p0 — Culombio por metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.236 | Faradio por metro — F/m | U1236:p0 — Faradio por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.237 | Culombio metro — C·m | U1237:p0 — Culombio metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.238 | Debye — D (debye) | U1238:p0 — Debye (dipolo eléctrico) | conventional_approximate | [iupac_green](https://iupac.org/wp-content/uploads/2025/03/IUPAC-GB4Abridged.pdf): p18,40: debye approximate SI correspondence; no post2019 exact CGS conversion is asserted |
| 1.239 | Tesla — T | U21:p0 — Tesla | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.240 | Gauss — G / Gs | U1240:p0 — Gauss | exact | [ucum22](https://ucum.org/ucum): G=1e-4T |
| 1.241 | Weber — Wb | U20:p0 — Weber | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.242 | Maxwell — Mx | U1242:p0 — Maxwell | exact | [ucum22](https://ucum.org/ucum): Mx=1e-8Wb |
| 1.243 | Amperio por metro — A/m | U1243:p0 — Amperio por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.244 | Oersted — Oe | U1244:p0 — Oersted | exact | [ucum22](https://ucum.org/ucum): Oe=250/pi A/m |
| 1.245 | Amperio-vuelta — A·turn / A | U1245:p0 — Amperio vuelta (bobina) | exact | [bipm](https://www.bipm.org/en/si-brochure-9): Coil-turn count is dimensionless; this is not angle rad/rev |
| 1.246 | Gilbert — Gi | U1246:p0 — Gilbert | exact | [ucum22](https://ucum.org/ucum): Gb=Oe*cm=2.5/pi A |
| 1.247 | Amperio metro cuadrado — A·m² | U1247:p0 — Amperio metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.248 | Henrio por metro — H/m | U1248:p0 — Henrio por metro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.249 | Mol — mol | U6:p0 — Mol | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, mole |
| 1.250 | Libra-mol — lbmol | U1250:p0 — Libra mol | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 pound-mole453.59237mol |
| 1.251 | Mol por metro cúbico — mol/m³ | U116:p0 — Mol por metro cúbico | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, mole<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.252 | Mol por litro / molar — mol/L / M | U115:p0 — Mol por litro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, mole<br>[bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.253 | Mol por kilogramo de disolvente — mol/kg | U1253:p0 — Mol por kilogramo de disolvente | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.254 | Kilogramo por mol — kg/mol | U1254:p10 — Kilogramo por mol | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.255 | Julio por mol — J/mol | U1255:p0 — Julio por mol | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.256 | Metro cúbico por mol — m³/mol | U1256:p0 — Metro cúbico por mol | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.257 | Katal — kat | U29:p0 — Katal | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.258 | Unidad de actividad enzimática — U | U1258:p0 — Unidad de actividad enzimática | exact | [iupac_enzyme](https://goldbook.iupac.org/terms/view/I03114): 1µmol/min; assay conditions required |
| 1.259 | Becquerel — Bq | U26:p0 — Becquerel | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.260 | Curie — Ci | U1260:p0 — Curie | exact | [ucum22](https://ucum.org/ucum): Ci=37e9Bq |
| 1.261 | Gray — Gy | U27:p0 — Gray | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.262 | Rad de dosis absorbida — rad (dosis) | U1262:p0 — Rad (dosis absorbida) | exact | [ucum22](https://ucum.org/ucum): RAD=0.01Gy |
| 1.263 | Sievert — Sv | U28:p0 — Sievert | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.264 | Rem — rem | U1264:p0 — Rem (dosis equivalente) | exact | [ucum22](https://ucum.org/ucum): REM=0.01Sv |
| 1.265 | Culombio por kilogramo — C/kg | U1265:p0 — Culombio por kilogramo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.266 | Roentgen / röntgen — R | U1266:p0 — Roentgen | exact | [ucum22](https://ucum.org/ucum): R=2.58e-4C/kg |
| 1.267 | Gray por segundo — Gy/s | U1267:p0 — Gray por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.268 | Sievert por hora — Sv/h | U1268:p0 — Sievert por hora | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.269 | Candela — cd | U7:p0 — Candela | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, candela |
| 1.270 | Lumen — lm | U24:p0 — Lumen | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.271 | Lux — lx | U25:p0 — Lux | exact | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.4, Table 4 |
| 1.272 | Foot-candle — fc | U1272:p0 — Foot-candle | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): lm/ft² |
| 1.273 | Phot — ph | U1273:p0 — Phot | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): B8 phot=10000lx; UCUM2.2 phot row rejected as inverted |
| 1.274 | Candela por metro cuadrado / nit — cd/m² / nit | U1274:p0 — Candela por metro cuadrado / nit | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.275 | Stilb — sb | U1275:p0 — Stilb | exact | [ucum22](https://ucum.org/ucum): sb=cd/cm² |
| 1.276 | Lambert — L (lambert) | U1276:p0 — Lambert | exact | [ucum22](https://ucum.org/ucum): Lmb=cd/cm²/pi |
| 1.277 | Foot-lambert — fL | U1277:p0 — Foot-lambert | exact | [nist811](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8): 1/(pi*ft²)cd |
| 1.278 | Lumen por vatio — lm/W | U1278:p0 — Lumen por vatio | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.279 | Lux-segundo — lx·s | U1279:p0 — Lux segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.280 | Vatio por estereorradián — W/sr | U1280:p0 — Vatio por estereorradián | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.281 | Vatio por metro cuadrado — W/m² | U1281:p0 — Vatio por metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.282 | Vatio por metro cuadrado estereorradián — W/(m²·sr) | U1282:p0 — Vatio por metro cuadrado estereorradián | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.283 | Julio por metro cuadrado — J/m² | U1283:p0 — Julio por metro cuadrado | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.284 | Dioptría — D (óptica) | U1284:p0 — Dioptría (lente) | exact | [ucum22](https://ucum.org/ucum): [diop]=1/m |
| 1.285 | Metro inverso — m⁻¹ | U1285:p0 — Metro inverso (número de onda) | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, metre |
| 1.286 | Bit — bit | U1286:p0 — Bit | exact | [nist_binary](https://physics.nist.gov/cuu/Units/binary.html): binary digit count |
| 1.287 | Byte de 8 bits / octeto — B | U1287:p0 — Byte / octeto (8 bits) | exact | [nist_binary](https://physics.nist.gov/cuu/Units/binary.html): 1B=8bit |
| 1.288 | Nibble — nibble | U1288:p0 — Nibble (4 bits) | exact | [nist_binary](https://physics.nist.gov/cuu/Units/binary.html): Explicit4-bit grouping convention |
| 1.289 | Bit por segundo — bit/s | U1289:p0 — Bit por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.290 | Byte por segundo — B/s | U1290:p0 — Byte por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.291 | Baudio — Bd | U1291:p0 — Baudio (símbolos por segundo) | exact | [ucum22](https://ucum.org/ucum): UCUM Bd=1/s; symbol count, never assumed bit count |
| 1.292 | Muestra por segundo — sample/s / Sa/s | U1292:p0 — Muestra por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): Sample count per second<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.293 | Fotograma por segundo — fps | U1293:p0 — Fotograma por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): Frame count per second; capture/render/display role required<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.294 | Evento o pulso por minuto — min⁻¹ / bpm | U1294:p0 — Evento por minuto | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): Event count per minute; counted event required<br>[bipm](https://www.bipm.org/en/si-brochure-9): §4, Table 8, pp.140–141 |
| 1.295 | Operación de coma flotante por segundo — FLOP/s / FLOPS | U1295:p0 — Operación de coma flotante por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): Counted floating-point operations per second; precision and counting policy required<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.296 | Instrucción por segundo — IPS | U1296:p0 — Instrucción por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): Instruction count per second; architecture/program required<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.297 | Operación de entrada/salida por segundo — IOPS | U1297:p0 — Operación E/S por segundo | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): I/O operations per second; size and workload required<br>[bipm](https://www.bipm.org/en/si-brochure-9): §2.3.1, second |
| 1.298 | Litro por cien kilómetros — L/100 km | U1298:p0 — Litro por 100 kilómetros | exact<br>estructura compuesta/potenciada<br>coeficiente explícito | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.299 | Milla por galón estadounidense — mpg (US) | U1299:p0 — Milla por galón líquido EE. UU. | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.300 | Milla por galón imperial — mpg (imp) | U1300:p0 — Milla por galón imperial | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |
| 1.301 | Vatio-hora por kilómetro — Wh/km | U1301:p0 — Vatio hora por kilómetro | exact<br>estructura compuesta/potenciada | [bipm](https://www.bipm.org/en/si-brochure-9): SI coherent product of identified components |

## 2. Escalas y medidas con contexto

| Fila | Solicitud / símbolo | Destino tipado | Naturaleza | Fuente y ubicación |
|---|---|---|---|---|
| 2.001 | Unidad uno — 1 | R100:p0 — Unidad uno | exact / scale | [bipm](https://www.bipm.org/en/si-brochure-9): 2.3.3 dimensionless quantities |
| 2.002 | Porcentaje, por mil y partes por millón — % / ‰ / ppm | R101:p0 — Porcentaje (relación)<br>R102:p0 — Por mil (relación)<br>R103:p0 — Partes por millón | exact / scale | [nist811ratios](https://www.nist.gov/pml/special-publication-811/nist-guide-si-chapter-7-rules-and-style-conventions-expressing-values): 7.10.2<br>[ucum](https://ucum.org/ucum): 28 dimensionless units |
| 2.003 | Decibelio — dB | R104:p0 — Decibelio (relación) | contextual / scale | [ucum](https://ucum.org/ucum): 46 table20 |
| 2.004 | Neper — Np | R105:p0 — Neper (relación de amplitudes) | contextual / scale | [ucum](https://ucum.org/ucum): 46 table20 |
| 2.005 | Decibelio referido a un milivatio — dBm | R106:p0 — Nivel de potencia, 1 mW | contextual / scale | [aes_levels](https://aes.org/publications/par/d/): decibel / 0 dBm |
| 2.006 | Decibelio referido a un vatio — dBW | R107:p0 — Nivel de potencia, 1 W | contextual / scale | [ucum](https://ucum.org/ucum): 46 bel watt |
| 2.007 | Decibelio referido a un voltio eficaz — dBV | R108:p0 — Nivel de tensión, 1 V RMS | contextual / scale | [aes_levels](https://aes.org/publications/par/d/): decibel / 0 dBV |
| 2.008 | Decibelio referido a aproximadamente 0,775 V eficaces — dBu | R109:p0 — Nivel de tensión, aprox. 0.775 V | contextual / scale | [aes_levels](https://aes.org/publications/par/d/): decibel / 0 dBu |
| 2.009 | Decibelio SPL, referencia en aire — dB SPL | R110:p0 — Nivel de presión sonora (aire) | contextual / scale | [ucum](https://ucum.org/ucum): 46 bel sound pressure |
| 2.010 | Nivel con ponderación A — dB(A) | R111:p0 — Nivel sonoro ponderado A | contextual / contextual | [ucum](https://ucum.org/ucum): 46 note on A weighting |
| 2.011 | Decibelio referido a escala completa — dBFS | R112:p0 — Nivel digital de escala completa | contextual / contextual | [aes17](https://www.aes.org/publications/standards/preview.cfm?ID=21): FS and dBFS terminology |
| 2.012 | Número Mach — M / Ma | R113:p0 — Número Mach | contextual / contextual | [nasa_mach](https://www.grc.nasa.gov/www/k-12/airplane/mach.html): M=V/a |
| 2.013 | pH — pH | R114:p0 — pH (actividad) | contextual / contextual | [iupac_ph](https://goldbook.iupac.org/terms/view/P04524): P04524 |
| 2.014 | Grado Brix — °Bx | R115:p0 — Grado Brix (sacarosa) | contextual / contextual | [oiml_brix](https://www.oiml.org/en/files/pdf_r/r142-1-e25.pdf): 3.1, 4.3, Annex D |
| 2.015 | Specific gravity — SG / d | R116:p0 — Densidad relativa | contextual / contextual | [eia_api](https://www.eia.gov/tools/glossary/index.php?id=API_gravity): Specific-gravity reference in API definition |
| 2.016 | Grado API — °API | R117:p0 — Grado API (60 F/60 F) | contextual / scale | [eia_api](https://www.eia.gov/tools/glossary/index.php?id=API_gravity): API gravity equation |
| 2.017 | Equivalente — eq | R118:p0 — Equivalente químico | contextual / contextual | [ucum](https://ucum.org/ucum): 45 equivalents |
| 2.018 | Normalidad — N (química) | R119:p0 — Normalidad (química) | contextual / contextual | [ucum](https://ucum.org/ucum): 45 equivalents, composed with litre |
| 2.019 | Osmol — Osm / osmol | R120:p0 — Osmol | contextual / contextual | [ucum](https://ucum.org/ucum): 45 osmol and caution on osmolar |
| 2.020 | Unidad internacional — UI / IU | R121:p0 — Unidad internacional (biológica) | contextual / contextual | [ucum](https://ucum.org/ucum): 45 international unit [iU] |
| 2.021 | Unidad nefelométrica o de formazina — NTU / FNU | R122:p0 — Turbidez nefelométrica (NTU)<br>R123:p0 — Turbidez de formazina (FNU) | contextual / contextual | [usgs_turbidity](https://pubs.usgs.gov/twri/twri9a6/twri9a67/twri9a_Section6.7_v2.1.pdf): 6.7.1.B and table6.7-4 |
| 2.022 | American Wire Gauge / Standard Wire Gauge — AWG / SWG | R124:p0 — Calibre americano AWG<br>R125:p0 — Calibre británico SWG | contextual / contextual | [astm_awg](https://store.astm.org/standards/b258): Scope1.1-1.2<br>[bsi_swg](https://knowledge.bsigroup.com/products/guide-to-series-of-basic-sizes-for-metal-sheet-strip-and-wire): BS3737:1964 catalogue; table not imported |
| 2.023 | Mesh — mesh | R126:p0 — Mesh (especificación de tamiz) | contextual / contextual | [astm_mesh](https://store.astm.org/e0011-24.html): Scope1.1,1.3 |
| 2.024 | Punto por pulgada / píxel por pulgada — dpi / ppi | R127:p0 — Puntos por pulgada<br>R128:p0 — Píxeles por pulgada | contextual / scale | [css4](https://www.w3.org/TR/2024/WD-css-values-4-20240312/): 7.4 resolution |
| 2.025 | Píxel CSS, em y rem — px / em / rem | R129:p0 — Píxel CSS<br>R130:p0 — Em CSS<br>R131:p0 — Rem CSS | contextual / contextual | [css4](https://www.w3.org/TR/2024/WD-css-values-4-20240312/): 6.2<br>[css4](https://www.w3.org/TR/2024/WD-css-values-4-20240312/): 6.1.1 |
| 2.026 | Mes y año civil — — | R132:p0 — Mes civil<br>R133:p0 — Año civil | contextual / contextual | [usno_calendar](https://aa.usno.navy.mil/faq/calendars): Calendar introduction |
| 2.027 | Ciclo de reloj — cycle | R134:p0 — Ciclo de reloj | contextual / contextual | [nist_clock](https://csrc.nist.gov/glossary/term/frequency): Definition of period and frequency |
| 2.028 | Jiffy — jiffy | R135:p0 — Jiffy (según sistema) | contextual / contextual | [linux_timers](https://cdn.kernel.org/doc/html/latest/timers/hrtimers.html): Jiffies and HZ |
| 2.029 | Palabra — word | R136:p0 — Palabra (según arquitectura) | contextual / contextual | [coral_word](https://developers.google.com/coral/guides/hardware/riscv-instr-set): Architecture word-size table |
| 2.030 | Metro cúbico normal o estándar; pie cúbico estándar — Nm³ / Sm³ / scf | R137:p0 — Metro cúbico normal de gas<br>R138:p0 — Metro cúbico estándar de gas<br>R139:p0 — Pie cúbico estándar de gas | contextual / contextual | [iso_gas](https://www.iso.org/standard/20461.html): Abstract: temperature, pressure, humidity |
| 2.031 | Presión absoluta o manométrica — psia / psig; bar(a) / bar(g) | R140:p0 — Presión absoluta en psi<br>R141:p0 — Presión manométrica en psi<br>R142:p0 — Presión absoluta en bar<br>R143:p0 — Presión manométrica en bar | contextual / contextual | [nist_pressure](https://www.nist.gov/laboratories/tools-instruments/piston-gauges-and-pressure-transducers): Gauge and absolute modes |

## 3. Regional e histórico

| Fila | Solicitud / símbolo | Destino tipado | Naturaleza | Fuente y ubicación |
|---|---|---|---|---|
| 3.001 | Vara de Burgos, referencia histórica — — | U1302:p0 — Vara de Burgos (1852) | historical | [cem1852](https://www.cem.es/sites/default/files/2019-11/00000458recurso.pdf): p643 Castilla; correspondences rounded per p656 |
| 3.002 | Fanega de tierra castellana, convención indicada — — | U1303:p0 — Fanega castellana superficie (1852) | historical | [cem1852](https://www.cem.es/sites/default/files/2019-11/00000458recurso.pdf): p643 Castilla; correspondences rounded per p656 |
| 3.003 | Fanega castellana de áridos — — | U1304:p0 — Fanega castellana áridos (1852) | historical | [cem1852](https://www.cem.es/sites/default/files/2019-11/00000458recurso.pdf): p643 Castilla; correspondences rounded per p656 |
| 3.004 | Cántara o arroba castellana de vino, convención indicada — — | U1305:p0 — Cántara castellana vino (1852) | historical | [cem1852](https://www.cem.es/sites/default/files/2019-11/00000458recurso.pdf): p643 Castilla; correspondences rounded per p656 |
| 3.005 | Arroba de aceite, convención histórica indicada — — | U1306:p0 — Arroba castellana aceite (1852) | historical | [cem1852](https://www.cem.es/sites/default/files/2019-11/00000458recurso.pdf): p643 Castilla; correspondences rounded per p656 |
| 3.006 | Catty de Hong Kong — catty / kan | U1307:p0 — Catty de Hong Kong | exact | [hk](https://www.cmchk.org.hk/pcm/pdf/guide_whole_chm_bfver_c.pdf): Common catty conversion table |
| 3.007 | Tael de masa común de Hong Kong — tael | U1308:p0 — Tael común de Hong Kong | exact | [hk](https://www.cmchk.org.hk/pcm/pdf/guide_whole_chm_bfver_c.pdf): 1tael=1/16catty; not gold-trade variant |
| 3.008 | Tsubo — tsubo | U1309:p0 — Tsubo japonés | conventional_approximate | [jetro](https://www.jetro.go.jp/ext_images/en/reports/survey/pdf/2013_05_01_biz.pdf): Hiroshima property area conversion; rounded |
| 3.009 | Rai — rai | U1310:p0 — Rai tailandés | exact | [thai_boi](https://www.nso.go.th/nsoweb/storage/ebook/2023/20230509185951_10500.pdf): 1rai1600m² |

## 4. Referencias naturales y físicas

| Fila | Solicitud / símbolo | Destino tipado | Naturaleza | Fuente y ubicación |
|---|---|---|---|---|
| 4.001 | Velocidad de la luz en el vacío — c | R1:p0 — Velocidad de la luz en el vacío | exact / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): speed of light in vacuum |
| 4.002 | Radio de Bohr — a₀ | R2:p0 — Radio de Bohr | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Bohr radius |
| 4.003 | Masa del electrón — mₑ | R3:p0 — Masa del electrón | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): electron mass |
| 4.004 | Hartree — Eₕ | R4:p0 — Energía de Hartree | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Hartree energy / atomic unit of energy |
| 4.005 | Unidad atómica de tiempo — ℏ/Eₕ | R5:p0 — Unidad atómica de tiempo | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): atomic unit of time |
| 4.006 | Constante de Planck reducida — ℏ | R6:p0 — Constante de Planck reducida | exact / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): reduced Planck constant |
| 4.007 | Longitud de Planck — ℓₚ | R7:p0 — Longitud de Planck | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Planck length |
| 4.008 | Tiempo de Planck — tₚ | R8:p0 — Tiempo de Planck | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Planck time |
| 4.009 | Masa de Planck — mₚ (Planck) | R9:p0 — Masa de Planck | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Planck mass |
| 4.010 | Fuerza de Planck — Fₚ | R10:p0 — Fuerza de Planck | derived / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Derived from CODATA 2022 G; not a separately listed table entry |
| 4.011 | Energía de Planck — Eₚ | R11:p0 — Energía de Planck | derived / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Derived in joules from CODATA 2022 h,c,G; table also lists mass energy equivalent in GeV |
| 4.012 | Temperatura de Planck — Tₚ | R12:p0 — Temperatura de Planck | measured / physical_constant | [codata2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt): Planck temperature |

## Alcance de las comprobaciones

Los factores y constantes se contrastan con oráculos independientes de la tabla generada.
Los casos de prueba detectan, entre otros, pérdida de incertidumbre, un factor falso para Mach/pH/mes,
calorías confundidas, kilogramo desfasado, prefijo fuera de la potencia, alias Cal en la variante incorrecta y pérdida de cobertura.
Estas comprobaciones offline no son operaciones dimensionales que ya funcionen en Calculation.
