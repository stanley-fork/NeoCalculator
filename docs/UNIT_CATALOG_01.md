# UNIT-CATALOG-01 — catálogo e inserción de unidades

Entrega local del 2 de octubre de 2026. Calculation permite buscar, previsualizar,
insertar, editar y guardar unidades con identidad propia. **El cálculo dimensional
y las conversiones de resultados siguen pendientes de CALC-UNITS-01.** Una entrada
con unidades conserva su árbol y muestra un aviso EN/ES; no se evalúa como letras,
no se simplifica previamente y no reemplaza Ans.

## Architecture Assessment

El registro puro se separa del adaptador Toolbox, del editor y de Giac. Los datos
inmutables residen en flash; la interfaz reutiliza sus cuatro filas y canvases.
`NodeUnit` utiliza las métricas, estilos y rutas comunes de layout, dibujo y cursor
de STIX Two Math, sin cambiar la tabla TeX de espaciado ni el ensamblador de
delimitadores. No se amplía el pool LVGL de 64 KiB.

## Preservación del candidato real

Se inspeccionaron HEAD, rama, índice, working tree y worktrees, las instrucciones
del repositorio y del workspace, el resumen Toolbox, su catálogo y los contratos
de Calculation/FORMAT. La base real era `main` en
`15b9536ff848549c08f883ca960b1beeeb28bdb5`, con revisiones locales posteriores a
ese commit. No se reconstruyó desde un commit antiguo.

Los checkpoints están en la ruta ignorada `out/unit-catalog-01/`:

| Checkpoint | Contenido |
|---|---|
| `A-toolbox` | Candidato recibido: fuentes rastreadas y nuevas, binario publicado y sus cuatro DLL |
| `B-infinity` | Corrección independiente de ±∞, probada y publicada antes de comenzar las unidades |
| `C-unit` | Candidato final; delta exclusivo respecto a B |

Cada checkpoint conserva `source/`, `runtime/`, `manifest.json` con SHA-256,
estado del índice, worktrees, instrucciones y parche binario desde HEAD. B y C
añaden `from-previous.patch`, que también incluye archivos nuevos. El manifiesto
final enumera los archivos exactos del delta. Las 900 trazas anteriores se
copiaron con hashes a `baseline-corpus/` antes de reproducirlas.

No hubo stash, reset, clean, staging, commit, push, despliegue ni flash.
`.vscode/settings.json` conserva SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.
El informe distingue imágenes **compiladas** de firmware instalado; no se afirma
que las imágenes de esta entrega estén instaladas en una calculadora física.

## Corrección independiente de ambos infinitos

`ResultPresentation::BothInfinities` acompaña a la procedencia de la opción
explícita de Toolbox. La presentación compacta exige además que el resultado
Giac siga siendo exactamente la lista ordenada de infinito negativo y positivo.
El valor canónico continúa siendo `[-infinity, +infinity]`: dos alternativas,
no un escalar ni el infinito complejo sin dirección.

El árbol de entrada, el historial y los snapshots de Ans/PreAns mantienen esa
matemática. Las referencias de sesión comprueban revisión y valor antes de
conservar la indicación. Una operación derivada no hereda ciegamente la marca.
No hay sustituciones de texto de listas por ±∞. Listas explícitas, vectores,
intervalos e indefinidos no adquieren presentación compacta.

La puerta independiente comprende 22 comprobaciones host y 16 recorridos del
emulador: inserción, resultado, Ans, historial, lista explícita, +∞, −∞,
`2·(−∞)`, `(-∞)²` con paréntesis y controles de indefinidos. La ayuda EN/ES
explica los dos valores sin exigir su representación como lista. La galería
final incluye el resultado ±∞ en ambos idiomas.

## Solution: registro, generación e identidades

La fuente única es `src/math/units/registry.json`. El generador
`scripts/generate-unit-catalog.py` produce `UnitRegistryData.inc`,
`UnitToolboxEntries.inc` y el [inventario reproducible](UNIT_CATALOG_01_INVENTORY.md).
`--check` comprueba que los artefactos coinciden, sin modificarlos.

Cada definición contiene ID estable, nombres EN/ES, aliases, símbolo, magnitud,
siete exponentes dimensionales, referencia coherente SI, escala y origen exactos,
políticas de prefijos, naturaleza de conversión, función de temperatura, fuente,
localización, exactitud y variante. Las relaciones apuntan directamente a la base
coherente SI; no se crea un grafo recursivo de definiciones. El generador valida
IDs, traducciones, referencias, máscaras, límites, categorías y colisiones de
aliases. Una colisión debe declararse; la búsqueda presenta cada identidad.

`UnitRegistry.h` no incluye Toolbox, LVGL, Giac ni un asignador. Los factores se
representan mediante `numerador/denominador × 10^exponente × π^potencia`, con
enteros acotados. Ningún `double` constituye la fuente de verdad. Todas las
definiciones incluidas son exactas; las basadas en mediciones se difieren.
Las conversiones actuales se clasifican en multiplicativas o afines; otras
transformaciones se rechazan en el registro de esta versión.

Los espacios de identidad se separan así:

- `UnitId` y `PrefixId`: átomo matemático validado, independiente de la UI.
- Identidad Toolbox de unidad: `0x8000 | ItemId`, con el prefijo en `variant`.
- Variables Unicode, memorias y constantes: conservan sus identidades anteriores.
- Presets compuestos: ID de catálogo propio y términos de unidad explícitos.

No se almacena una fila, nombre traducido, dirección de widget o cadena CAS como
identidad. Reordenar categorías o definiciones no cambia favoritos anteriores.
La búsqueda utiliza descriptores generados en flash y una página de resultados;
no materializa widgets ni AST para las 884 variantes.

## Fuentes y exactitud

Las fuentes descargadas y sus hashes se conservan en `sources/manifest.json`:

- [BIPM, SI Brochure](https://www.bipm.org/en/si-brochure-9): se consultó realmente
  la **9.ª edición, versión 4.01, junio de 2026**. Definiciones base §2.3.1;
  unidades derivadas tabla 4; prefijos §3/tablas 7; no SI tabla 8. Se registra
  la localización particular en cada definición, no solo este enlace general.
- [BIPM, prefijos SI](https://www.bipm.org/en/measurement-units/si-prefixes): los
  24 prefijos vigentes, incluidas las incorporaciones de 2022.
- [CEM, traducción española](https://www.cem.es/sites/default/files/documentos/2022-08/30362_elsistemainternacionaldeunidades_web_0.pdf):
  9.ª edición, prólogo de diciembre de 2019. Se usa para terminología española;
  no se toma su antigua cobertura de prefijos como catálogo vigente.
- [NIST SP 811, apéndice B.8](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8):
  edición 2008, usada únicamente para definiciones convencionales estables.
  Una fila redondeada no se transforma en igualdad exacta: psi se deriva de
  libra avoirdupois, gravedad convencional e inch internacional.
- [NIST, temperatura](https://www.nist.gov/pml/owm/si-units-temperature): relaciones
  exactas de Celsius/Fahrenheit/Kelvin y diferencias de temperatura.
- [NIST, §9.3](https://www.nist.gov/pml/special-publication-811/nist-guide-si-chapter-9-rules-and-style-conventions-spelling-unit-names):
  excepciones inglesas `kilohm` y `megohm`, registradas en la fuente de datos.

NumWorks solo orientó la interacción ya existente; no se copiaron sus tablas,
conversiones, traducciones ni implementación.

## Cobertura real

Hay **55 definiciones**, **68 entradas base del catálogo** y **884 variantes
ofrecidas**. Las definiciones cubren las siete familias base SI —masa anclada en
gramo, con kilogramo como referencia— y las 22 derivadas con nombre especial,
23 unidades prácticas adicionales y tres diferencias de temperatura explícitas.
Con las 152 identidades matemáticas previas hay 1.036 identidades totales: 1.024
descubribles y las 12 de compatibilidad anteriores. El harness que informa 1.045
añade nueve entradas de prueba; no son producto.
Las 68 entradas comprenden 55 accesos, dos potencias y once compuestos.

| Categoría raíz | Entradas base | Cobertura |
|---|---:|---|
| Longitud | 7 | m; in, ft, yd, mi internacionales; nmi; au |
| Superficie y volumen | 4 | m², m³, ha, L |
| Tiempo y frecuencia | 5 | s, min, h, d; Hz en subcategoría separada |
| Masa | 4 | g/kg, t, lb y oz avoirdupois |
| Movimiento y fuerzas | 10 | m/s, km/h, m/s², knot; N, lbf; Pa, bar, atm, psi |
| Energía y potencia | 5 | J, W, eV, W·h, N·m (momento de fuerza) |
| Electricidad | 10 | A, C, A·h, V, Ω, S, F, H, Wb, T |
| Temperatura | 6 | K, °C, °F y sus tres diferencias explícitas |
| Ángulos | 5 | rad, sr, °, ′, ″ |
| Sustancia | 6 | mol, kat, mol/L, mol/m³, g/L, kg/m³ |
| Luz | 3 | cd, lm, lx |
| Radiación | 3 | Bq, Gy, Sv |

Electricidad tiene nueve subcategorías: corriente, carga, tensión, resistencia,
conductancia, capacidad, inductancia, flujo y densidad de flujo magnético.
Hz/Bq, J/N·m, Gy/Sv y ángulos conservan magnitudes diferenciadas aun cuando
coincidan sus dimensiones. Insertar radianes no elimina la unidad.

Se difieren dalton (masa medida/incertidumbre), galones, calorías y caballos de
potencia (requieren selector de convención), duraciones de calendario y unidades
logarítmicas. No hay meses o años universales ni una promesa de todas las unidades
existentes. No se confunden libra-masa y libra-fuerza.

## Prefijos, potencias y compuestos

El selector conserva el orden de exponentes descendente:
`Q R Y Z E P T G M k h da [neutral] d c m µ n p f a z y r q`.
La opción neutral tiene exponente cero y no cuenta como prefijo oficial.

En `Unidades → Longitud → Metro`, EXE inserta m y RIGHT abre el selector en m.
La selección neutral queda en el índice 12, con `top=10`: el viewport actual
de cuatro filas muestra hm, dam, m, dm. km, µm, Qm y qm se alcanzan por scroll.
BACK restaura selección y scroll del padre; desde un favorito mm, RIGHT abre en
mm (índice 15). RIGHT conserva su función de pestaña/cursor cuando la lista no
tiene foco. No se añade una pestaña ni una aplicación.

Se distinguen prefijos estándar y ofrecidos. El constructor admite **850 pares
atómicos registrados**, de los cuales **823** se ofrecen como familias atómicas.
Los 27 restantes son los 24 prefijos de Celsius y mas/µas/pas astronómicos:
válidos y formateables, pero diferidos en la UI. No se declaran ilegales por ser
infrecuentes. Las 884 variantes incluyen además las potencias y los compuestos.

kg es exactamente `(gramo, kilo)`, sin duplicado de favorito. mg equivale a
10⁻⁶ kg. En cm² y cm³, el prefijo pertenece al átomo dentro de `NodePower`:
se eleva toda la unidad prefijada. Los compuestos contienen `NodeUnit`,
`NodePower`, fracciones y producto explícito, no nombres opacos; km/h contiene
metro+kilo y hora como componentes editables. A·h y W·h se insertan como
secuencias transaccionales cuyos componentes puede recorrer el cursor.

## Editor, tipografía y serialización

`makeUnit(Atom)` valida antes de construir `NodeUnit`. Seleccionar m no introduce
una variable libre, A no consulta/escribe memoria y Ω no se convierte en la
letra griega. No cambia la separación previa entre e/i/I/π libres y constantes.

La inserción prepara el árbol y reserva el destino antes de publicar cambios.
Funciona en fila vacía, después de un número, en fracciones y paréntesis, como
base de potencia y junto a otra unidad. Después de 2 inserta una operación
estructural de producto valor–unidad; no concatena `2s` ni envuelve silenciosamente
toda la expresión anterior. Entre unidades se usa un punto visible: m·s es
distinto de ms. La unión prefijo+unidad se dibuja como un único átomo en redonda
y negro. La separación valor–unidad es geometría de 3 mu compartida, y °/′/″
tienen adhesión angular; no se añaden espacios de texto para simular estructura.

Medida y dibujo usan `layoutTextAtom` y las mismas métricas STIX; cursor, clonación,
borrado, potencias y envoltura en fracción conservan los nodos. Se verificaron
glifos reales, incluidos µ, Ω, ° y exponentes, en DISPLAY/TEXT/SCRIPT/SCRIPTSCRIPT.
No se modifica la política global de cursiva o colores. El aviso español usa
el fallback de acentos ya existente. El popup invalida su superficie común al
refrescar para evitar fragmentos de pestañas tras cambiar de nivel.

La representación de máquina del átomo es cerrada y versionada:
`[0x55, versión=1, UnitId bajo, UnitId alto, PrefixId]` (5 bytes).
`decode` comprueba tamaño, versión, ID y prefijo antes de modificar el destino.
No acepta llamadas CAS ni identifica retrospectivamente una variable m como
metro. ID corrupto o variante no válida producen fallo; nunca unidad cero ni
primer elemento por defecto.

El historial de Calculation conserva clones del AST durante la sesión y recupera
las entradas con sus unidades. **No se introduce un nuevo formato de persistencia
de expresiones completas**: el codec anterior es el contrato de datos del átomo,
y la ruta de texto hacia Giac rechaza unidades deliberadamente. Favoritos sí
persisten mediante el esquema existente de Toolbox. Equations, Calculus y
Grapher filtran la capacidad de unidades; no reciben nodos como letras por error.

## Evaluación provisional y Giac vendorizado

`scanUnits` recorre el árbol autoral antes de serializar, simplificar o sincronizar
la sesión Giac. Detecta también `0×unidad` y `unidad/unidad`. Está acotado a
400 nodos y profundidad 82; el límite es de profundidad, no del número de
hermanos (se conserva el caso previo de 150 sumandos).

Una unidad válida devuelve `MathEngineStatus::UnitsUnavailable`, con aviso:

> El cálculo con unidades aún no está disponible en esta versión.

No produce Syntax ERROR ni un número sin unidades. Mantiene la entrada completa,
registra su historial donde procede y conserva Ans. Tras AC, `2+2` vuelve a dar 4.
Identidad inválida o límite estructural tienen diagnóstico propio y no entran en
Giac. Tampoco se amplía FORMAT, TABLE o ENG con conversiones provisionales.

El Giac vendorizado contiene `mksa_unit` en `lib/giac/src/prog.h`, tablas y rutas
`WITH_UNITS` en `kprog.cc`, y operadores `at_unit` en `kgen.cc`. Su estructura
incluye coeficientes y exponentes `double`; no es la fuente normativa exacta de
este catálogo ni garantiza las políticas de magnitud/temperatura. Se documenta
como posible herramienta algebraica para la siguiente fase, sin activar una
ruta por sufijos o asumir soporte por el aspecto de una cadena. El motor
headless WASM-MATH sigue sin enlazar Toolbox/LVGL ni exponer memoria/filesystem.

## Búsqueda y persistencia

Nombres EN/ES, aliases, magnitudes, símbolos y nombres prefijados entran en el
proveedor actual. El plegado de nombres tolera acentos; los símbolos completos
conservan mayúsculas. µ/μ y Ω/Ω equivalentes se aceptan solo en la frontera de
unidades, sin normalizar el alfabeto libre del usuario.

Se prueban metro/metre/meter, milimetro/milímetro/millimeter,
resistencia/ohm/ohmio/Ω, microsegundo/µs/μs, mA/MA, MHz/mHz, Pa/pA y s/S.
Un símbolo exacto no se diluye en coincidencias textuales sin relación. Ω o m
pueden mostrar la letra y la unidad como identidades separadas; los resultados
de unidades muestran nombre y categoría en dos líneas.

Se conservan **24 favoritos**, **12 recientes de sesión** y los dos registros
recuperables con CRC/versionado del Store existente. ID+variant bastan: no hubo
migración de esquema. mm, kΩ, µF y km/h guardan la variante exacta; kg comparte
identidad con kilo+gramo. Solo una inserción confirmada modifica Recientes;
navegar prefijos no escribe flash. La recarga web comprueba IDBFS real.

## Memory & Typographic Profile

Las medidas de firmware y sus hashes están en `firmware/resources-firmware.json`;
los resultados host se conservan íntegros junto a los scripts de reproducción.
La tabla de cierre se genera en el manifiesto de entrega y se transcribe debajo.

| Perfil | Imagen compilada | Flash enlazada | RAM estática | IRAM `.text` |
|---|---:|---:|---:|---:|
| numos-esp32-s3-wroom-1u-n16r8 | 5.826.320 B | 5.825.957 B | 119.608 B | 60.407 B |
| esp32s3_n16r8 | 5.738.704 B | 5.738.333 B | 118.200 B | 59.039 B |

Respecto a la referencia compilada de A, las imágenes crecen 74.992 B (WROOM) y
75.104 B (CAM); RAM estática +64 B e IRAM sin incremento. Este delta incluye
B y C, no se atribuye íntegramente al registro ni a una instalación física.

Stack Impact: frames propios Xtensa: guarda 704 B, búsqueda 464 B, refresco 432 B,
layout de unidad 80 B e inserción 64 B. Heap Delta: +52 B de objeto por átomo
Xtensa, más contenedor/asignador al construir; **0 asignaciones nuevas en layout,
dibujo y cursor**. IRAM Delta: 0 B. Bounding Box Alignment: verificado por
probes host con glifos reales, no por medición física.

Una unidad simple necesita un nodo; una unidad al cuadrado y km/h requieren
cinco nodos cada uno; m/s², nueve. A·h prepara cuatro nodos y publica tres
componentes en la fila receptora. No se guardan textos dinámicos en NodeUnit.


`NodeUnit`: 52 bytes Xtensa / 64 bytes host. No aumenta el tamaño de NodeRow,
NodeParen o NodeSpecialValue. En Xtensa: Atom 4, Identity 4, Store 164, Session
1380 bytes. No se crea un árbol de widgets para el catálogo ni un AST por prefijo.
Los descriptores inmutables suman 48.125 bytes en ELF, **sin incluir cadenas ni
código**; el delta real de flash se informa por separado.

La búsqueda mantiene un cache de clasificación de símbolo de 65 caracteres y
un booleano; los buffers temporales están acotados. La construcción de AST sí
asigna nodos, fuera del layout/draw/cursor. Layout de la unidad utiliza un buffer
local de 32 bytes y la ruta de métricas existente, sin asignaciones. Los frames
de pila propios se extraen del prólogo Xtensa; no representan máximos de cadena
de llamadas, interrupciones ni pila observada en hardware.

En el muestreo final: mínimo libre **5.992 bytes**, mayor bloque mínimo **5.008 bytes**;
HOME estable en 67 objetos, tres timers, cero handles y 19.352 bytes libres.
En los 100 ciclos comparables con historial lleno se muestrea el pool fijo,
apertura, búsqueda, prefijos, favorito, inserción/cancelación y HOME. Se conservan
cuatro calentamientos y 60 evaluaciones previas por ciclo. Los mínimos y tiempos
son muestras **host**, no máximos de heap/pila ni latencias ESP32. La reserva
configurada es 65.536 bytes; el pool TLSF utilizable informado es menor.

## Pruebas, controles negativos y recuperación

Las puertas y sus artefactos exactos se enumeran en `out/unit-catalog-01/delivery.json`.

| Puerta | Alcance |
|---|---|
| Datos | Oráculos independientes Fraction para cm, mm, cm²/cm³, mg/kg, mL/m³, min/h, in, lb/oz, psi, eV, extremos 10⁻⁹⁰/10⁹⁰ y offsets de temperatura |
| Mutaciones | Once mutaciones: kilogramo, mili, caso eléctrico, traducción, referencia de item, ciclo, offset, potencia, referencia SI, ID duplicado y alias ambiguo deben fallar |
| Constructor | Los 850 pares atómicos registrados; también se preparan las 884 variantes ofrecidas por el proveedor |
| Identidad/editor | Clone/codec, ID corrupto, versión desconocida, m libre, memoria A, Ω libre, potencias, compuesto editable, producto explícito, 0×unidad y unidad/unidad |
| Modal nativo | 36 recorridos reales EN/ES, selección/scroll, prefijos extremos, cancelar sin cambiar AST, EXE sin fuga al editor, historial y favoritos entre procesos |
| Tipografía | Glifos y previews reales con STIX y tres tamaños; denegación de asignaciones en layout; tinta negra y dimensiones/cursor |
| Recuperación | 100 ciclos con historial lleno y pool fijo; puerta anterior adicional de 50 recorridos con entrada larga y 50 HOME |
| Fallos | 17 recorridos de fallo de apertura/preview/búsqueda/inserción/guardado/epoch; cuatro fallos de creación de fila; 3665 posiciones persistentes de fallo de asignación host |
| Navegadores | Shell y componente en Chromium, Firefox y WebKit; eventos, símbolos, historial y recarga real IDBFS |
| Regresión | Toolbox, cálculo cotidiano, infinito/listas, alfabetos/constantes, Steps/tutor, VAR/STO, FORMAT/TABLE/ENG, notación/layout/cursor, EN/ES y fallos de presupuesto |
| Matemáticas | 900 trazas previas conservadas y reproducidas: igualdad salvo tiempos; WASM-MATH en tres navegadores |
| Compilación | Native, WROOM N16R8, CAM N16R8 y web; ninguna validación física nueva |

Los fallos de fila se inyectan en un overlay privado del factory; no se afirma
interposición de todos los malloc internos de LVGL. En fallo se conserva la
expresión y se desmonta el modal parcial. No se publica media unidad ni un
favorito a medias. Los controles de búsqueda por símbolos usan eventos SDL
nativos y, para Unicode web, eventos DOM keypress entregados al handler SDL:
no escriben AST o memoria interna, ni certifican todos los IME del sistema.

Los harnesses web esperan estado observable; las esperas nativas son frames
deterministas. No se promocionan goldens ni se amplían máscaras. Se conservan
logs de intentos fallidos durante desarrollo, incluidas invocaciones incorrectas
de probes privados; los resultados finales están diferenciados en el manifiesto.

Tiempos host de la serie final (ámbitos inclusivos, pueden solaparse):

| Ámbito | Muestras | Media | Máximo observado |
|---|---:|---:|---:|
| Apertura | 310 | 0.935 ms | 2.680 ms |
| Refresco de filas/previews | 2580 | 0.737 ms | 5.064 ms |
| Entrada de búsqueda | 104 | 7.371 ms | 9.353 ms |
| Activar/entrar/insertar | 310 | 0.495 ms | 1.456 ms |
| Cierre | 309 | 0.087 ms | 0.158 ms |

Estos tiempos proceden de puntos de diagnóstico acumulados, no de un trazador de
latencia de extremo a extremo ni de ejecución ESP32.

### Risks & Edge Cases

- La aritmética dimensional, afín y de conjuntos/signos ± no está implementada.
  La guarda es un límite visible, no una conversión aproximada.
- El pool fijo sigue estrecho; los mínimos muestreados no acreditan todos los
  escenarios simultáneos ni el consumo del CAS en PSRAM física.
- `parMixed` conserva la discrepancia histórica del detector estricto de tinta.
  No se proclama reparada ni se oculta mediante tolerancias nuevas. En esta
  revisión el runner estricto antiguo se detiene antes, en `parX`, porque lee
  también el probe de medición a baseline cero; se reproduce igual con A y C
  (`parens-strict-A.log`, `parens-strict.log`). La puerta vigente de corchetes
  con observador privado sí pasa; no se modifica la máscara del runner antiguo.
- Grapher ya agotaba el pool fijo antes de abrir Toolbox en el baseline; sus
  recorridos funcionales corresponden al perfil CLIB, no a hardware/pool fijo.
- No se añade persistencia de historial completo ni una gramática general de
  importación de AST. La unidad nunca se degrada a variable al serializar.
- El selector actual tiene cuatro filas; la posición neutral se centra con la
  discreción propia de un número par de filas. No se omite ningún prefijo.

### Alternatives Considered

Se descartaron nombres CAS arbitrarios, duplicar variables como unidades y un
motor parcial de sufijos: perderían identidad o aparentarían cálculo dimensional.
La combinación estable unidad+prefijo evita duplicados de kilogramo y conserva
favoritos al reordenar. Los presets estructurados permiten editar componentes
sin analizar texto visible. Se reutiliza el modal existente para no multiplicar
widgets ni construir cientos de AST anticipadamente.

## Emulador y galería

La publicación estable permanece en `C:\.piobuild\numOS\emulator_pc\program.exe`,
con sus DLL. Todas las compilaciones se hicieron en caches aisladas fuera de
esa carpeta y de sus antecesores, con auto-clean deshabilitado. El publicador
validado comprueba runtimes y smoke, conserva copia previa y sustituye el exe
atómicamente. No se cierra una sesión del usuario si está bloqueado.
La publicación final se completó; `previous-program.exe` conserva B. El lanzador
habitual verificó la entrada con unidades, su aviso, historial y la vuelta a 4.
`publication.log` y `launcher-final.log` registran esa comprobación.

La galería autocontenida es `out/unit-catalog-01/gallery/index.html`; su ZIP es
`out/unit-catalog-01/gallery.zip`. Incluye **41 originales 320×240**, EN/ES,
recursos relativos comprobados y hashes. No incluye fuentes. Muestra categorías,
metro centrado, extremos, resistencia, kΩ/µF, cm²/cm³, favoritos mm/km/h,
símbolos ambiguos, expresión editada, aviso provisional y ±∞ compacto.


## Archivos exactos del delta C (respecto a B)

- `docs/UNIT_CATALOG_01.md` (nuevo)
- `docs/UNIT_CATALOG_01_INVENTORY.md` (nuevo)
- `scripts/build-toolbox-allocation-probe.py`
- `scripts/generate-unit-catalog.py` (nuevo)
- `scripts/package-unit-gallery.py` (nuevo)
- `scripts/profile-toolbox-firmware.py`
- `scripts/replay-unit-baseline.py` (nuevo)
- `scripts/test-toolbox-faults.py`
- `scripts/test-unit-catalog.py` (nuevo)
- `scripts/test-unit-data.py` (nuevo)
- `scripts/test-unit-lifecycle.py` (nuevo)
- `scripts/test-unit-row-faults.py` (nuevo)
- `src/apps/CalculationApp.cpp`
- `src/hal/NativeHal.cpp`
- `src/math/CalculationEngine.cpp`
- `src/math/CursorController.cpp`
- `src/math/MathAST.cpp`
- `src/math/MathAST.h`
- `src/math/ToolboxCatalog.cpp`
- `src/math/ToolboxCatalog.h`
- `src/math/giac/GiacEngine.h`
- `src/math/units/UnitRegistry.h` (nuevo)
- `src/math/units/UnitRegistryData.inc` (nuevo)
- `src/math/units/UnitToolboxEntries.inc` (nuevo)
- `src/math/units/UnitToolboxProvider.inc` (nuevo)
- `src/math/units/registry.json` (nuevo)
- `src/ui/MathRenderer.cpp`
- `src/ui/Toolbox.cpp`
- `tests/host/toolbox_checks.cpp`
- `tests/host/toolbox_visual_checks.cpp`
- `tests/host/unit_catalog_checks.cpp` (nuevo)
- `tests/wasm/unit-catalog.mjs` (nuevo)

## Contrato de integración para CALC-UNITS-01

1. Consumir `NodeUnit::atom()` y las operaciones AST, sin analizar los símbolos
   dibujados. Resolver UnitId/PrefixId mediante el registro validado.
2. Sustituir la guarda de `UnitsUnavailable` por una ruta de cantidades que
   preserve entrada, precisión y Ans ante errores. No simplificar primero.
3. Elevar conjuntamente escala y prefijo para potencias; mantener productos y
   denominadores estructurales. Trabajar con racionales/decimales exactos/π.
4. Definir políticas explícitas de magnitud: misma dimensión no selecciona
   automáticamente Hz/Bq, J/N·m, Gy/Sv ni elimina ángulos.
5. Diferenciar temperatura absoluta y diferencia; origen afín no es un factor.
   Rechazar operaciones inválidas antes de publicar un resultado.
6. Reutilizar proveedor, búsqueda, identidad y Store; FORMAT podrá seleccionar
   cada componente de salida mediante Atom, sin rehacer Toolbox.
7. Si se persisten expresiones completas, envolver el codec cerrado de unidad
   en el formato de AST versionado del producto, conservando rechazo transaccional.

Aceptación futura, **todavía pendiente**:
`(2 s / 5 m) × (10 km / 40 s²) → 100 s⁻¹`.
Las pruebas offline de factores no acreditan esa operación en Calculation.
La selección de Hz requiere además una política posterior de magnitudes.

Checklist físico preparado, sin ejecutarlo: arranque WROOM/CAM, PSRAM y margen
de heap, latencia de navegación real, cursor/glifos en pantalla, favoritos tras
corte de alimentación, sesión larga con historial, temperaturas absolutas y
recuperación tras errores. Requiere autorización antes de cualquier flash.
