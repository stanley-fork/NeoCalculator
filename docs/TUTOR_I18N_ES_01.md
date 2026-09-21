# TUTOR-I18N-ES-01 — English / Español

## Candidato y separación de 03A

Trabajo sobre `main`, HEAD `b5ef38c3f97a16ab3df1370ab3bc5a49ef4ed6f2`, en
`C:\Users\Juan Ramón\Documents\Calculadora`. El índice estaba vacío. Los 32
archivos modificados/nuevos de 03A **ya estaban presentes sin commit**.
No se ha reconstruido el producto a partir del HEAD anterior a 03A.

Fingerprint inmediato de ejecución: `3b4269b899d9c565b180d06e694a3cbf10d36c59b4407c38b3dcdfe34ef612b0`.
Fingerprint completo inicial: `b4f702d076386a804644cbf12d55e7b0dfa063d7ba4a4237054ff17404f25b26`.
El algoritmo es SHA-256 de las entradas ordenadas `ruta/NUL/SHA-256/LF`;
el subconjunto de ejecución contiene `src/`, `lib/`, `boards/` y `platformio.ini`.

`out/tutor-i18n-es-01/` conserva parches binarios separados de índice y working
tree, `baseline.json`, copias legibles en `candidate-03a.zip`, y sus hashes.
El ZIP se ha leído y verificado. `final-manifest.json`, `i18n-files.zip` y
`i18n-only.patch` separan esta tarea del candidato 03A, incluso en archivos
compartidos. `.vscode/settings.json` conserva exactamente SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.

Fingerprint final de ejecución: `18c306ef4e4819bf3821fec4dd0ce16c23bdca369b4ad6278773b6db539d07fe`.
La copia ASCII de compilación tiene los mismos bytes de ejecución. No se ha
hecho staging, commit, push, flash ni operación de almacenamiento en la PCB.
La aceptación física de 03A sigue pendiente.

## Catálogo y alcance

**228 claves**, todas con inglés y español no vacíos y esquemas de parámetros
comprobados: las 184 anteriores más 44 de interfaz. Se mantienen las 42 entradas
francesas existentes, byte por byte. Francés y claves inesperadas conservan su
fallback de seguridad; no se usa como sustituto de una traducción española.

[Catálogo bilingüe generado](TUTOR_I18N_ES_01_CATALOG.md): clave, contexto y
parámetros, inglés y español. Se genera desde `Messages.inc`; no es un catálogo
manual. `catalog/catalog.json` añade tipos y referencias concretas a origen.

El inventario incluye reglas, motivos, títulos, operaciones intermedias, casos,
filas, restricciones, candidatos, descartes, conclusiones, familias periódicas,
completitud/verificación/conciliación, método no disponible, errores, límites,
guiado/resumen, navegación, numeración y plurales, Results y su resolución
transitoria, además del selector y su ayuda. Los errores técnicos de Giac
permanecen en el resultado/diagnóstico de máquina; la UI muestra una instrucción
de recuperación traducida, sin presentar un log como enseñanza.

Fuera de alcance: editor y listas de entrada, Templates, nombre global
«Equations», otras aplicaciones y las demás filas de Settings. Sus controles y
sintaxis permanecen intactos. Los valores matemáticos de fallback exacto siguen
siendo los valores del motor, no texto traducido. RAD/DEG, nombres de teclas,
funciones, identificadores y JSON/API no se traducen.

La validación exige EN y ES para **cada** enumerador. Una clave nueva sin ES
falla aunque el fallback pudiera mostrar inglés. El control negativo elimina
una entrada española en una copia aislada y detecta la ausencia. También se
comprueban tipos, placeholders, vacíos, claves sin resolver, plural 0/1/2 y
texto UTF-8 dañado; no se usa una búsqueda de palabras inglesas como prueba de
cobertura.

## Criterio editorial

Imperativo singular: suma, resta, divide, factoriza, despeja, comprueba.
«Ambos miembros», «términos semejantes» y «argumento» sustituyen calcos.
Los parámetros matemáticos de las instrucciones siguen siendo los del paso
comprobado. Las fórmulas permanecen en VPAM/STIX.

| Concepto | Uso español |
|---|---|
| Side / term / factor | miembro / término / factor |
| Coefficient / leading coefficient | coeficiente / coeficiente principal |
| Unknown | incógnita |
| Candidate / accepted solution | candidato a solución / solución aceptada |
| Domain / restriction | dominio / restricción |
| Solution set / periodic family | conjunto de soluciones / familia de soluciones |
| Integer parameter | parámetro entero |
| Principal angle / period | ángulo principal / periodo |
| Discriminant | discriminante |
| Function range | valores posibles o recorrido, nunca rango de matriz |
| System | compatible/incompatible solo cuando la clasificación lo sostiene |

Positivo y no negativo, candidato y solución comprobada, equivalencia e
implicación, desconocido y falso se mantienen distintos. La afirmación de
igualdad del conjunto completo con Giac conserva exactamente la guarda 03A.

Dos correcciones bilingües mínimas de ambigüedades inglesas:

- `trig.principal` ya no identifica el ángulo principal con un «reference angle».
- `log.base_inverse` dice que cada miembro se usa como exponente de la base
  válida; no ordena ambiguamente «elevar ambos miembros a e».

No cambian regla, relación, parámetros ni hipótesis. Una revisión editorial
separada del agente inspeccionó el catálogo y secuencias completas de sistemas,
cuadrática, logaritmo, radical, seno y tangente. Detectó y se corrigieron un
`funci?n` introducido por una herramienta de edición y un título periódico de
dos líneas; se conservaron las primeras capturas. La revisión posterior vio
«función» correcta y «Soluciones periódicas» sin solapamiento. Esto no es una
aprobación humana del español ni una revisión del LCD físico.

## Selección de idioma

Desde HOME abre **Settings**, baja a la última fila **Tutor language** y pulsa
**EXE** o LEFT/RIGHT. La fila pasa a **Idioma del tutor — Español**; repite para
**English**. Es la quinta fila en native/web y la sexta en producción, que
mantiene Brightness. HOME permite volver a Equations. Se respeta English por
defecto y no se detecta automáticamente el idioma del navegador.

Se ignora REPEAT en esta selección. No se escribe una preferencia por cada
repetición de tecla. Producción reutiliza el byte reservado 10 del registro
NST2 de 16 bytes, versiones 2/3 existentes y checksum. Web reutiliza el byte 9
del ST01 de 10 bytes, sin cambiar versión. Cero y valores desconocidos se
interpretan como English. Un registro corrupto mantiene el rechazo existente.
Native conserva la política actual de ajustes en memoria: no se añade un
archivo persistente nuevo para el emulador nativo.

Pruebas: registros antiguos con sus demás valores intactos, checksum/corrupción,
valor desconocido; en un perfil web nuevo, selección real, guardado, recarga,
registro antiguo cero e inyección aislada del byte desconocido. Ninguna prueba
toca almacenamiento físico.

## Invariancia y propiedad de fórmulas

El idioma es una preferencia de presentación, fuera del snapshot matemático.
Cambiar la locale de una página retenida revalida la identidad de sus referencias
de fórmula y reutiliza sus AST ya publicados: no llama a Giac, no clona fórmulas
ni ejecuta el planificador. Se actualizan prosa, leyendas, alturas y scroll.
No se comparte un árbol mutable entre padres; no se introduce caché global.
Se conserva el desplazamiento horizontal y el scroll vertical válido.

Las **761 trazas** se comparan EN/ES/EN. El único campo excluido entre idiomas es
`steps[*].text`. Todos los demás campos, incluidos condiciones, candidatos,
binders, pasos, completitud, verificaciones, snapshots, recursos contados y
conciliación, son idénticos. Frente al baseline se separa únicamente `micros`
(una nueva medición) y se enumeran las dos correcciones inglesas anteriores.
Las **163 conciliaciones mejoradas por 03A** se conservan; no se ignora una
sección completa para conseguir el resultado.

Nueve métodos recorren cada página EN→ES→EN: mismos AST, referencias,
identidades de paso y contador de construcción; **cero conversiones** durante
el cambio. Otra prueba compara el viewport antes/después de cambiar de idioma
en una fórmula ya desplazada con VAR. Edit/cancel, reapertura y HOME conservan
el contrato de vida anterior.

## Tipografía y capturas

El subconjunto Montserrat compilado no contenía todos los acentos requeridos.
Se añaden exclusivamente 16 glifos a 10/12/14 px:
`á é í ó ú ü ñ Á É Í Ó Ú Ü Ñ ¿ ¡`.
`generate_tutor_spanish_fonts.py` exige el conversor existente **lv_font_conv
1.5.3**, usa el TTF del repositorio y conserva OFL. Las fuentes de respaldo
mantienen las métricas/kerning ASCII originales. La prueba consulta realmente
los 48 glifos y compara los 95 caracteres ASCII en los tres tamaños.
STIX, Δ, paréntesis y geometría matemática no cambian. No hay asignaciones
nuevas en layout/render ni fuentes globalmente más pequeñas.

Las imágenes originales son **320×240**, con todos los desplazamientos de cada
secuencia. Evidencia bajo `out/tutor-i18n-es-01/`:

- `ui/`: 50 secuencias / 259 páginas / 597 frames EN/ES, resumen y muestras FR/pseudo.
- `ui-editorial/`: reemplazos corregidos de seno, tangente y radical, sin borrar
  la evidencia inicial.
- `ui-final/`: valor absoluto de dos soluciones y fórmulas anchas RAD/DEG; VAR
  alcanza los extremos y vuelve al origen.
- `locale-acceptance/`: selector real, modo no disponible y conservación de pan.
- `failure/`: aviso recuperable español producido por fallos reales inyectados.
- `failure-acceptance/`: recuperación final, también con título e instrucciones
  en español si falla el propio cambio de idioma.
- `comparison/index.html`: 21 comparaciones bilingües completas, hoja por página.
- `wasm/`: las mismas fórmulas en los navegadores, no un solver JavaScript.

Las hojas `*-contact-part*.png` conservan lectura a escala original. Se han
inspeccionado imágenes, no solo cajas. El texto español ocupa más líneas en
varios motivos; desplaza fórmulas hacia abajo y usa el scroll existente. No se
han cambiado goldens, máscaras, fuentes STIX ni offsets para ocultar diferencias.

## Verificación y límites de evidencia

Los comandos exactos, salidas y hashes están en los directorios citados.
Entradas reproducibles principales:

```text
python scripts/test-tutor-i18n-catalog.py --out out/i18n/catalog --review-file docs/TUTOR_I18N_ES_01_CATALOG.md
python scripts/test-tutor-i18n-host.py --source SNAPSHOT --build NATIVE_BUILD --out HOST
python scripts/test-tutor-i18n-invariance.py --bin HOST/tutor_i18n_main.exe --baseline BASELINE_03A_HOST --out out/i18n/invariance
python scripts/test-tutor-i18n-ui.py --bin NATIVE --out out/i18n/ui
python scripts/test-tutor-teaching-ui.py --bin NATIVE --out out/i18n/walkthroughs --bilingual --cases isolated both-sides factoring quadratic complex rational system dependent inconsistent abs-variable radical-extraneous exp-injective log-domain trig-sine trig-affine trig-tangent trig-sine-deg trig-impossible
python scripts/test-tutor-nonlinear-lifecycle.py --bin FIXED_POOL_NATIVE --out out/i18n/pool --trig --i18n
python scripts/test-tutor-i18n-failure.py --steps-bin STEPS_PROBE --results-bin RESULTS_PROBE --out out/i18n/failure
node scripts/tutor-teaching-web.mjs SNAPSHOT --browser=chromium --nonlinear --transcendental --trig --periodic --spanish
node scripts/test-tutor-i18n-web-storage.mjs SNAPSHOT out/i18n/web-storage
```

Los probes privados se construyen con `build-tutor-ui-allocation-probe.py`, uno
con `--results`; no forman parte del firmware. La inyección mantiene el fallo
durante la recuperación de la publicación afectada. **152 casos** cubren apertura
española, cambio de idioma y Results periódicos, una vez y persistente; no se
publica matemática parcial y una apertura posterior recupera el resultado sin
reiniciar Giac. No demuestra seguridad de todas las asignaciones internas de Giac.

La compilación usa dependencias fijadas: Giac `1.4.9+khicas.57`, LVGL `9.5.0`,
PlatformIO `6.1.19`, SCons `4.8.1`, Xtensa GCC `8.4.0+2021r2-patch5`, Arduino
`3.20017.241212+sha.dcc1105b`; native GCC `15.2.0`. No se han actualizado.

| Gate | Resultado y evidencia |
|---|---|
| Catálogo / control negativo | PASS, 228 EN + 228 ES, 0 ausencias; `catalog/coverage.json` |
| Tipos, plurales, fuentes y registros | PASS, 1.498 comprobaciones; `host-acceptance/tutor_i18n_checks.log` en el caché de compilación |
| 761 trazas frente a 03A | PASS, `invariance-acceptance/`; 163 mejoras de conciliación conservadas |
| Corpus y mutaciones existentes | PASS, `host-regression/` en el caché: 235 comprobaciones/23 mutaciones 02A, 386/25 02B, 889/166 trig; corpus histórico y desafíos |
| Recuperación del planificador existente | PASS, 880 inyecciones; `host-regression/tutor_nonlinear_allocation.log` |
| Proyección/conclusión | PASS, 6 fixtures, 20 pasos, 35 guardas rechazadas; traza intacta |
| Selector / EN→ES→EN / pan | PASS, `locale-final/`; nueve métodos, todas sus páginas, cero conversiones en cada cambio |
| Fallos de publicación localizada | PASS, 152 inyecciones; `failure-acceptance/`, incluida recuperación con cabecera/pie españoles |
| Equations | PASS, 92 entradas de resultado, transacciones, Templates, eventos físicos y ciclo de vida; `equations/` |
| Results/Steps periódicos | PASS, nueve entradas RAD/DEG, `periodic-results/` |
| Pool fijo / cambios de idioma | PASS, 50 ciclos y 300 muestras; `pool50-acceptance/` |
| Persistencia web real | PASS, selección, recarga, registro antiguo/desconocido; `web-storage-acceptance/` |
| Native | PASS, `recovery-final-emulator_pc.json` |
| WROOM normal/demo, CAM normal | PASS, logs `recovery-final-*` |
| WROOM bring-up, CAM validación | PASS, logs `acceptance-*` |
| Web paquete/smoke | PASS, `wasm/acceptance/web.json` |
| Chromium / Firefox / WebKit, EN y ES | PASS, seis recorridos completos en `wasm/acceptance/`; cada uno conserva hashes del WASM y resultados |
| Whitespace | PASS, `git diff --check`; avisos CRLF existentes separados de errores |

La frontera WASM-MATH no cambia respecto a 03A: valores y claves de máquina se
comprueban por invariancia; no se atribuye a esta tarea una nueva ejecución de
su matriz Release/Debug. El WASM de interfaz sí se reconstruye y prueba.
La matriz matemática completa de 03A conserva sus observaciones históricas;
no se convierte en validación física ni se promocionan sus goldens.
Siguen documentadas en `TUTOR_ENGINE_03A.md` las 18 discrepancias de goldens
históricos y la deriva previa del cuerpo de Grapher Templates. No se ha
repetido ni declarado PASS una nueva comparación estricta de esos goldens en
esta tarea de catálogos. Las capturas bilingües nuevas son evidencia de texto
y recorrido, no nuevos goldens de referencia.

Primeras incidencias de harness conservadas: un comando de CAM usó un nombre
de entorno inexistente (`esp32-s3-devkitc-1`), corregido a `esp32s3_n16r8`;
la primera prueba de persistencia tomó píxeles antes de que se publicase el
frame del selector (100 ms), corregida con el mismo Enter de producto y espera
de dibujo de 350 ms. No se debilitó ninguna expectativa matemática.
La primera inyección de fallo durante EN→ES dejó cabecera y pie ingleses;
la corrección usa strings estáticos del catálogo incluso bajo fallo persistente.
Se conserva la captura anterior y se repitieron las 152 inyecciones, vistas,
compilaciones y pool fijo. La captura de aceptación muestra el aviso completo
en español. La revisión editorial separada también inspeccionó esta recuperación
y la ayuda final del selector.

## Recursos

| Medida WROOM ordinario | Baseline 03A | I18N final | Delta |
|---|---:|---:|---:|
| Flash enlazada | 5.602.477 B | 5.621.041 B | +18.564 B |
| Imagen | 5.602.848 B | 5.621.408 B | +18.560 B |
| RAM estática | 119.024 B | 119.160 B | +136 B |
| Texto IRAM | 60.407 B | 60.407 B | 0 B |

SHA-256 de la imagen ordinaria **compilada, no instalada**:
`79c43066c68f5038d804b7cc510145d078216680ca0ecf86da1ee49b49954a14`.
`firmware-final.json` y los logs conservan las medidas. Desensamblado Xtensa:
frame propio de `drawStep` 928→928 B, `showResult` 304→368 B,
`SettingsApp::updateValues` 96→96 B. Estos frames no incluyen la cadena de
llamadas ni son una medición de stack máximo físico.

Payload C++/AST máximo solicitado en secuencias emparejadas del mismo binario
(EN→ES): cuadrática 3.771→3.791 B, logaritmo 3.174→3.191 B, seno no habitual
9.080→9.098 B. Son incrementos respecto al inicio de cada preparación; no se
suman entre páginas y no incluyen LVGL, malloc de Giac ni overhead del allocator.
Un cambio de locale en la página retenida del seno usa 225 B temporales C++ y
cero asignaciones de nodos; su geometría matemática se reutiliza.

El pool LVGL sigue en **65.536 B**. En 50 ciclos mixtos, sin reset de Giac,
el mínimo **muestreado** de payload libre es 7.200 B frente a 7.256 B en 03A.
Cada HOME conserva 67 objetos, 3 timers, cero handles retenidos y payload
monitorizado total/libre de 57.472/19.160 B. No hay pérdida progresiva; el
monitor excluye cabeceras TLSF y no mide el pico total del heap.

La prosa española sí tiene coste dinámico: strings temporales y almacenamiento
de etiquetas LVGL. El profiler separa C++/AST de LVGL/Giac; sus incrementos no
se suman entre momentos distintos. No se afirma coste cero a partir de RAM
estática ni se atribuyen tiempos/heap/stack de host al ESP32. Stack físico,
placement interno/PSRAM y latencia del LCD **no medidos** en esta tarea.

## Archivos de esta tarea

```text
docs/TUTOR_I18N_ES_01.md
docs/TUTOR_I18N_ES_01_CATALOG.md
platformio.ini
scripts/build-tutor-ui-allocation-probe.py
scripts/generate_tutor_spanish_fonts.py
scripts/test-tutor-i18n-catalog.py
scripts/test-tutor-i18n-failure.py
scripts/test-tutor-i18n-host.py
scripts/test-tutor-i18n-invariance.py
scripts/test-tutor-i18n-ui.py
scripts/test-tutor-i18n-web-storage.mjs
scripts/test-tutor-nonlinear-lifecycle.py
scripts/test-tutor-teaching-ui.py
scripts/tutor-teaching-web.mjs
src/apps/EquationsApp.cpp
src/apps/EquationsApp.h
src/apps/SettingsApp.cpp
src/apps/SettingsApp.h
src/apps/TutorStepsView.inc
src/demo/DemoSettingsRecord.h
src/fonts/montserrat_es_10.c
src/fonts/montserrat_es_12.c
src/fonts/montserrat_es_14.c
src/math/tutor/Derivation.h
src/math/tutor/Locale.h
src/math/tutor/Messages.inc
src/ui/TutorFonts.h
tests/host/tutor_i18n_checks.cpp
tests/host/tutor_i18n_main.cpp
```

La división exacta before/after con hashes está en `i18n-files.txt` y
`final-manifest.json`. Los otros cambios sin commit siguen perteneciendo a 03A.
Pendiente: lectura del usuario y aceptación física separada; francés completo
y traducción del resto del sistema permanecen fuera de alcance.

## Cierre combinado: revisión editorial y controles de software (2026-09-21)

El estado inicial sigue siendo `main` en
`b5ef38c3f97a16ab3df1370ab3bc5a49ef4ed6f2`, sin cambios en el índice. Se preservaron
por separado los parches binarios de índice/working tree, los archivos nuevos,
los manifiestos y los ZIP anteriores en `out/tutor-03a-es-closeout/`.
`.vscode/settings.json` conserva su SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.
No se restauró HEAD ni se eliminó el candidato 03A.

Los únicos retoques de contenido son:

- `candidate.sign_invalid`: «Descarta este candidato: al sustituirlo, el otro
  miembro resulta negativo, pero debe ser mayor o igual que cero.» La condición
  `IsolatedRange` que usa esta clave es siempre `Nonnegative`; el checker evalúa
  la condición tras sustituir el candidato. Los objetivos positivos de exp/log
  usan otro rol y no reciben esta explicación.
- `trig.principal`: «Usa la función trigonométrica inversa para obtener el
  ángulo principal.» No cambian intervalos ni cantidades.
- Las ayudas españolas distinguen `<> Paso`, `^v Despl.` y `VAR Horiz.` en Steps;
  `<> Horiz.` en Results; y `VAR Pág.` en Results paginados. VAR desplaza las
  fórmulas anchas directamente, no activa un modo. En Results cambia el grupo
  mostrado, no calcula otra solución. El equivalente inglés pasa de `VAR Next
  set` a `VAR Page`. Se mantienen todas las asignaciones y tamaños de fuente.

El catálogo se regeneró desde `Messages.inc`: 228/228 claves EN y ES, parámetros
válidos, sin ausencias; el control negativo de traducción retirada falla como
debe. La revisión visual directa en emulador incluye el rechazo de cero y sus
dos restricciones, ángulo principal, modo resumen y Results paginados con cinco
raíces. No constituye aprobación del usuario ni revisión de la LCD física.

El ZIP autocontenido `out/tutor-03a-es-closeout/revision-03a-es.zip` incluye
`index.html`, catálogo, instrucciones y todos sus recursos relativos. Mantiene
21 secuencias bilingües completas, los originales de 320×240 y las capturas
adicionales de ayudas, selector y método no disponible. `visual-package.json`
registra el hash y la comprobación de ausencia de recursos perdidos.

### Identidad y comprobaciones

Runtime combinado anterior:
`18c306ef4e4819bf3821fec4dd0ce16c23bdca369b4ad6278773b6db539d07fe`.
Runtime combinado revisado:
`a5cc45687930d7a14c6b0feafeb0d2fbdbb3595702f28621c139d760fdbc5f50`.
Las diferencias de runtime son exclusivamente los textos indicados. Native y
producción normal se compilaron en un árbol aislado distinto del utilizado para
03A, con las dependencias fijadas. La imagen normal tiene SHA-256
`12e1277cf0e54c0db004312391d30dede901d67b50dc0571a7063d6282ef847d`.
Tamaños: imagen 5.621.408 B, flash enlazada 5.621.041 B, RAM estática 119.160 B,
IRAM text 60.407 B; delta cero respecto a los tamaños previos, **no** identidad
binaria ni coste dinámico cero.

Evidencia adicional bajo `out/tutor-03a-es-closeout/`:

- 1.498 comprobaciones host de catálogo/parámetros/fuentes/preferencias.
- 761 trazas EN/ES matemáticamente invariantes respecto a 03A; las 163
  conciliaciones mejoradas permanecen intactas.
- 204 casos periódicos y controles de independencia, adaptación, comparación,
  binders, mutaciones y recuperación, tanto en 03A aislado como en el combinado.
- Cambio EN→ES→EN de nueve familias retenidas: cero conversiones nuevas, mismo
  grafo, fórmulas y construcción de prueba; movimiento horizontal conservado.
- 152 casos de fallos de publicación localizada y recuperación, incluidos
  fallos persistentes; no prueba exhaustiva de todo el heap de Giac.
- 50 ciclos mixtos del pool fijo sin reset de Giac: 300 muestras, mínimo
  observado de payload libre 7.200 B; HOME estable en 67 objetos, 3 temporizadores
  y 19.160 B libres de 57.472 B monitorizados. Son muestras host de bloques LVGL,
  no picos completos, memoria física ESP32 ni tamaño distinto del pool de 64 KiB.
- Results/Steps periódicos y las 92 comprobaciones de Equations pasan en cada
  árbol. No se han promovido goldens ni ampliado máscaras.

Los intentos fallidos del guion editorial quedan conservados: una ruta absoluta
con caracteres no ASCII no se abrió en el harness; un fixture inicialmente
elegido no activaba paginación. La repetición usa una ruta relativa y cinco raíces
exactas, y pasa. No hubo corrección de producto para esos fallos del guion.

La sonda temporal queda fuera de los dos commits. La autorización específica
se recibió para este cierre; la aceptación conjunta y la restauración del
ordinario se describen a continuación. La revisión humana de la LCD y del
español sigue diferida. Los datos históricos conservan su alcance original.

La matriz final afectada termina en PASS: native, WROOM normal/bring-up/demo,
CAM normal/validación y las seis ejecuciones EN/ES en Chromium, Firefox y WebKit.
También pasan recarga de la preferencia española en un perfil web aislado,
registro antiguo y valor desconocido. Esto último no acredita persistencia en
la PCB. La frontera WASM-MATH no cambió respecto a la evidencia aceptada de 03A;
se conserva su matriz Release/Debug anterior, sin atribuirle una ejecución nueva.
Los controles periódicos actuales suman 2.215 aserciones y 192 casos de inyección
de fallos con recuperación, en cada snapshot.

Paquete visual definitivo: 531 originales y 640 recursos relativos verificados,
9.031.617 B, SHA-256
`86f4b60bc3980409fbe69eaf5c702228ce2751165597be543e19c4565590cb16`.
Antes del control final del índice, los únicos archivos modificados por este cierre sobre el combinado inicial eran
`src/math/tutor/Messages.inc`, este informe, `docs/TUTOR_ENGINE_03A.md` y el
catálogo generado. `closeout-only-manifest.json` registra sus hashes antes/después.

## Aceptación física conjunta (2026-09-21)

Se usó la WROOM de producción redescubierta en COM9, USB 303a:1001,
serial/MAC `44:B1:76:A7:B7:2C`. Se autorizó expresamente este ciclo. Se escribió
solo la aplicación activa (`0x10000`, capacidad `0x640000`), con copia de
recuperación, verificación del fabricante y lectura completa independiente.
El prefijo/tabla de particiones permaneció idéntico. No se escribió bootloader,
NVS ni eFuses, ni se borró/formateó almacenamiento. Las preferencias se guardaron
normalmente desde Settings; no se afirma identidad de los bytes de LittleFS.

Los 13 ciclos principales recorrieron la aplicación real mediante eventos
canónicos: seno cero; seno afín RAD EN/ES; seno no habitual DEG EN/ES;
tangente RAD EN/ES; cuadrática EN/ES; radical con rechazo; logaritmo;
sistema único; frontera `sin(x)=0*x/x`. Se comprobaron Results/Steps,
desplazamiento vertical completo, pan, resumen/guiado, BACK, cancelación de
edición, HOME y reentrada. Las reaperturas conservaron una construcción de
prueba. Results mantuvo procedencia Giac independiente; la frontera con
dominio excluido no publicó familias completas. La explicación de rechazo
por signo emitió la nueva frase sobre sustituir el candidato.

Desde Settings se comprobó ES→HOME→Settings→RESET→ES y el recorrido inverso
en inglés, mediante RESET hardware USB. Brillo (144/192), modo angular,
complejos, modo educativo y precisión se conservaron. Los RESET fueron
anteriores a los ciclos de memoria; no se reinició Giac entre estos. La prueba
de locale con página retenida y cero conversiones es host; pasar por HOME
para usar Settings destruye legítimamente la app. La colisión de binder k/n_0
también mantiene alcance host. No se inventa acceso físico para esas pruebas.

### Tiempos instrumentados en la PCB

Milisegundos. Reapertura de la misma página final, tres muestras por fixture.
Las columnas son ámbitos anidados y no deben sumarse. La conciliación se
incluye en la generación del tutor; no existe aquí temporizador separado.
No son latencias del primer píxel LCD. EN/ES usa idéntica fórmula y página,
en secuencia con el estado caliente: las diferencias no prueban por sí solas
un coste causal del idioma.

| Caso | Giac ordinario | Tutor | Results | Primera apertura | Primera preparación final | Reapertura mediana / máx. (n=3) |
|---|---:|---:|---:|---:|---:|---:|
| sine-zero | 28.660 | 217.367 | 6.240 | 43.631 | 38.905 | 66.093 / 66.166 |
| affine-en | 169.474 | 520.680 | 9.477 | 45.396 | 45.941 | 70.146 / 70.154 |
| affine-es | 169.696 | 520.975 | 9.479 | 46.449 | 48.989 | 73.292 / 73.336 |
| unfamiliar-en | 352.218 | 1266.387 | 10.477 | 45.527 | 81.387 | 106.333 / 106.383 |
| unfamiliar-es | 352.262 | 1265.717 | 10.481 | 46.628 | 84.525 | 109.625 / 109.671 |
| tangent-en | 125.318 | 391.115 | 8.238 | 41.121 | 34.959 | 60.158 / 60.306 |
| tangent-es | 125.429 | 390.770 | 8.202 | 42.017 | 38.319 | 63.327 / 63.419 |
| quadratic-en | 48.194 | 458.904 | 8.392 | 59.991 | 33.885 | 57.953 / 57.953 |
| quadratic-es | 48.176 | 459.022 | 8.420 | 60.546 | 35.781 | 59.890 / 59.900 |
| radical | 35.450 | 1264.285 | 4.929 | 40.230 | 16.454 | 42.855 / 42.879 |
| logarithm | 23.286 | 361.080 | 5.144 | 39.713 | 17.608 | 44.218 / 44.219 |
| system | 11.162 | 211.058 | 7.589 | 38.067 | 20.633 | 45.811 / 45.875 |

### Memoria y estabilización

HOME inicial: interna libre 144.208 B / bloque mayor 102.388 B; PSRAM libre
8.227.755 B / bloque mayor 8.126.452 B. Tras el calentamiento y la frontera
no admitida: interna 143.872 B; los bloques mayores permanecen iguales.
El primer uso deja retención de calentamiento, no coste dinámico cero.
Los objetos/pantallas/temporizadores vuelven a 71/5/3; ninguna familia ni
prueba queda propiedad de la app tras HOME. LVGL monitoriza 61.496 B de
payload, con 40.800 B libres al volver a HOME; el pool configurado sigue
siendo 64 KiB.

Las 141 muestras de frontera de los ciclos principales observaron mínimos
de interna libre 143.872 B, PSRAM libre 8.208.247 B y payload LVGL libre
33.548 B. No son picos completos ni mínimos simultáneos. La pila mínima
sin usar fue 51.672 **bytes**: el puerto fijado define `StackType_t` como
`uint8_t`, confirmado por `sizeof(StackType_t)==1`; no se multiplica por cuatro.

Se retiene un fallo diagnóstico: seis repeticiones exigían un margen de
solo 4 B y tres muestras finales idénticas. Apareció una bajada transitoria
de 8 B de PSRAM, recuperada inmediatamente; no cambió la memoria interna,
los bloques mayores ni los objetos. No se cambió código ni se borró ese fallo.

Resultado de la revisión posterior: 28 ciclos en total, sin reset de Giac entre ellos. Secuencia final de PSRAM libre: `8224363, 8224363, 8224363, 8224363, 8224363, 8224363` B. La aceptación se limita a la estabilización observada; no acredita ausencia universal de fugas. Los logs iniciales y la repetición quedan conservados.

### Restauración, identidad y separación

No hubo corrección de código de producción durante la prueba física.
Runtime combinado:
`a5cc45687930d7a14c6b0feafeb0d2fbdbb3595702f28621c139d760fdbc5f50`.
Sonda temporal (solo evidencia, excluida del commit):
`0bfebf982e16369be00eb1b708b108c5e3363034811e1ec1e286f555238ab5a7`.
Ordinario final instalado y leído íntegramente:
`12e1277cf0e54c0db004312391d30dede901d67b50dc0571a7063d6282ef847d`.
Imagen 5.621.408 B; flash enlazada 5.621.041 B; RAM estática 119.160 B;
IRAM 60.407 B. Mismos tamaños que el candidato previo al retoque editorial,
con bytes distintos por los textos corregidos. El rebuild final ordinario
conserva exactamente ese hash. Sin sonda instalada.

El smoke ordinario final recorre selección de español, seno afín, logaritmo,
Steps, navegación y HOME. Sus barreras serie acreditan ejecución y respuesta;
los estados internos detallados se observaron en la sonda del mismo runtime,
no se atribuyen al ordinario APIs de introspección que no tiene.
El primer intento de smoke terminó en timeout al enviar el nombre `VAR`,
inexistente en el puente serie ordinario. Se conserva el log; la repetición
corrige solo ese guion. VAR ya pasó por la matriz canónica de la sonda,
pero no se afirma que el smoke ordinario lo ejercitase.
Preferencia final: **HOME / RAD / Español**. Revisión humana LCD/lingüística:
**diferida**, sin aprobación atribuida al usuario.

Los dos snapshots se validan con directorios de compilación separados:
03A (32 rutas) y después localización (29 rutas, siete compartidas con 03A).
El staging compara cada blob del árbol propuesto con su snapshot aislado,
aplicando los filtros de Git. El primer binario aislado no se probó en la PCB;
la evidencia física corresponde al combinado. Los hashes finales de commits,
árboles y listas exactas se guardan en la evidencia de cierre; no se necesitan
enmiendas ni un tercer commit para insertar el hash del propio informe.
El artefacto instalado se compiló antes de los commits: el código runtime
guardado es idéntico, pero una compilación posterior con nueva identidad Git
o metadatos no recibe retrospectivamente la validación de ese binario.

Todos los artefactos de prueba, imágenes, copias de placa y overlays permanecen
bajo rutas ignoradas. `.vscode/settings.json` conserva sus bytes. No se hace
push ni se inicia otra fase matemática. Pendientes: lectura humana del español
y LCD; la medición no separa conciliación ni primer píxel y no demuestra picos
completos del asignador.

### Normalización final del índice

El control de whitespace del índice detectó una línea vacía al final del
informe 03A (corregida antes del primer commit) y blancos finales en el script
de catálogo y las tres fuentes Montserrat generadas. Se eliminaron esos
blancos y el generador fija ahora un único salto final. Los tres archivos C
coinciden byte a byte al excluir únicamente su whitespace final; no cambian
arrays, glifos, métricas ni tokens compilados. El generador fijado 1.5.3 se
ejecutó de nuevo y produjo exactamente esos archivos normalizados.

Fingerprint raw anterior, correspondiente a la sonda y al ordinario probado:
`a5cc45687930d7a14c6b0feafeb0d2fbdbb3595702f28621c139d760fdbc5f50`.
Fingerprint raw final guardado:
`add63b49ceaa6350b151bec43986398ec2b675c2cee4fc0f7fc09b06c67b3fed`.
La diferencia está limitada a EOF en los tres C generados. El rebuild aislado
native y WROOM pasa; la imagen ordinaria recompilada conserva **exactamente**
el SHA-256 instalado y leído de la placa, no solo sus tamaños. No se necesita
otra escritura. Las 228 entradas/negativo, las 1.498 comprobaciones de
catálogo/fuentes/preferencias y el recorrido EN→ES→EN con cero conversiones
pasan otra vez. La primera salida host a una ruta acentuada falló al crear
el objeto; la repetición en el directorio ASCII del snapshot pasó. Ambos logs
se conservan. No se modificó código matemático ni se repitieron innecesariamente
los navegadores por un cambio de whitespace sin efecto binario.

El cierre modifica nueve rutas respecto al combinado inicial: las cuatro
editoriales/documentales descritas antes, más
`scripts/generate_tutor_spanish_fonts.py`, `scripts/test-tutor-i18n-catalog.py`
y `src/fonts/montserrat_es_{10,12,14}.c`. La frontera del segundo commit sigue
siendo de 29 rutas. El manifiesto detalla antes/después de cada archivo.
