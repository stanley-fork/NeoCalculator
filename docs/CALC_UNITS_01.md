# CALC-UNITS-01 — cálculo dimensional y unidades de salida

Entrega local del 4 de octubre de 2026. Calculation calcula cantidades reales
cerradas con unidades multiplicativas exactas y permite elegir su representación
en **SHIFT → ALPHA → FORMAT → Unidad de salida**. El emulador validado está en
`C:\.piobuild\numOS\emulator_pc\program.exe`; se abre con
`./scripts/run-emulator-windows.ps1`.

[Galería de 36 capturas originales](../out/calc-units-01/gallery/index.html) ·
[ZIP autocontenido, sin fuentes](../out/calc-units-01/gallery.zip).

## Architecture Assessment

Se conserva el candidato real posterior a UNIT-CATALOG-02, sobre `main`, HEAD
`15b9536ff848549c08f883ca960b1beeeb28bdb5`. El motor reutiliza los átomos tipados,
el registro y el renderer STIX; añade metadatos propietarios al resultado,
sin aumentar todos los nodos ni crear otro contexto Giac. Layout, dibujo y
cursor no incorporan nuevas asignaciones ni fórmulas geométricas.

### Preservación y presupuesto

Todo lo preservado está bajo la ruta ignorada `out/calc-units-01/`:

| Checkpoint | Contenido |
|---|---|
| `A-catalog` | 1.424 archivos, 55.108.441 B; fuentes recibidas, archivos nuevos, hashes, parche binario desde HEAD, índice, worktrees, instrucciones y runtime publicado |
| B | No procede: no se realizó compactación |
| `C1-core` | Primer núcleo de cantidades, separado de la integración de producto |
| `C2-product` | Ans, sesión, historial y FORMAT integrados |
| `C3-final` | Fuentes finales, runtime publicado, hashes y parche binario desde C2 |
| `exclusive-source-manifest.json` | Delta exclusivo respecto a A, con hashes anterior/posterior |
| `C3-final/from-A-catalog.patch` | Delta binario A → entrega, incluidos archivos nuevos |

El índice permaneció vacío. No se reconstruyó desde un commit antiguo. Se
conservaron los worktrees existentes, incluido el registro obsoleto de
`equations-candidate`, sin podarlo. No hubo stash, reset, clean, staging,
commit, push, despliegue web ni flash. `.vscode/settings.json` conserva SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.

Antes de implementar se auditaron ambos ELF: registro/proveedor alcanza
**471.186 B** de `.flash.rodata`: **238.469 B de tablas** y **232.717 B de
cadenas**. Se cuentan rangos físicos compartidos una sola vez; es coste
alcanzable, no delta exclusivo, tamaño de JSON ni consumo de RAM. Las variantes
completas eran la principal oportunidad de compactación; los overrides regionales
ya eran dispersos. Se presupuestó un margen mínimo de imagen de 131.072 B.
El crecimiento real permite conservar registro, IDs, idiomas, ayudas y aliases.
La auditoría final confirma los mismos 471.186 B en ambos targets, sin una
segunda tabla de conversiones (`registry-flash-baseline.json` y `registry-flash-final.json`).

### Decisión sobre Giac

El [manual primario de Xcas](https://www-fourier.ujf-grenoble.fr/~parisse/giac/doc/fr/cascmd_fr/index.html)
describe unidades y conversiones. La decisión usa además el probe enlazado contra
los objetos reales de **KhiCAS 1.4.9+khicas.57**, con su mismo ABI/configuración:

| Entrada del probe | Resultado del perfil vendorizado |
|---|---|
| `2_m+30_cm` | `32`, sin identidad ni escala de centímetro |
| `2_cm*3_cm` | `6`, sin unidades |
| `6_km/300_s` | `1/50`, sin kilo |
| `2_m+3_s` | `5`, sin diagnóstico dimensional |
| `mksa(1_cm)` | `mksa(1)`, sin resolver |
| `usimplify(12_V/3_A)` | `usimplify(4)`, sin resolver |
| `sin(90_deg)` en RAD | `sin(90)`, sin semántica angular explícita |

En `lib/giac/src/kprog.cc` parte del soporte depende de `WITH_UNITS`;
`mksa_unit` de `prog.h` emplea `double`. Se elige **dimensión NumOS y escalar
exacto Giac**, sin parchear el vendor ni construir otro CAS. El probe y su log
se conservan en `tests/host/giac_units_probe.cpp` y
`C:/.codex-cache/calc-units-01/giac-probe/giac_units_probe.log`.

## Solution

### Modelo e interpretación estructural

`Quantity.h/.cpp` separa el canónico exacto propietario (`Value`), la propiedad
inmutable compartida (`Owned`), el descriptor de salida (`Descriptor`) y los
coeficientes/AST de presentación preparados (`Display`). `Value` conserva
dimensiones y procedencia de magnitud mediante `UnitId`. Todo valor admitido es
real, cerrado, exacto y multiplicativo por construcción; los demás se rechazan
con un estado tipado antes de publicar. No se expone `giac::gen` ni un contexto
en la UI. La aproximación nunca decide compatibilidad o exactitud.

Las siete dimensiones SI se completan con ángulo plano, ángulo sólido,
información y siete dominios de recuento. Se mantiene `lm = cd·sr`, también en
lux. Información/recuentos no desaparecen por tener vector SI nulo. Esta es una
representación interna, sin modificar los datos normativos del registro.

`InputRowSyntax.h` comparte con el serializador previo la interpretación de
filas, precedencia, signos y productos implícitos. Se visitan nodos existentes;
no se analiza texto visible ni se ejecutan fórmulas descriptivas del catálogo.
`NodeUnit` continúa separado de variables, memoria A, letras griegas y constantes.

La presencia de unidades/referencias se comprueba antes de simplificar y los
hijos se validan antes de operar: `0*(2 m+3 s)`, `(2 m+3 s)^0`, `0*Mach`,
`1/(0 s)` y `ln(2 m)` no se vuelven válidos por borrar un subárbol. Un error
dimensional usa `QuantityError`, distinto de `Syntax ERROR`; las capacidades
pendientes usan `UnitsUnavailable` con un motivo específico EN/ES compartido
entre las cuatro regiones.

### Capacidades y cobertura

| Operación | Contrato |
|---|---|
| Suma/resta | Igual vector completo; coeficientes en referencia coherente |
| Producto/división | Factores exactos, suma/resta de exponentes y denominador comprobado |
| Cero | `0 m` conserva longitud; ni `0 m+3 s` ni `0+3 m` se coaccionan |
| Potencias | Exponente racional adimensional acotado; dimensiones resultantes enteras |
| Raíces | Grados 1–16; `sqrt(9 cm²)=3/100 m`; dimensión fraccionaria fuera de alcance se diagnostica |
| `abs` | Conserva dimensiones |
| `ln`, `exp`, logaritmos | Argumentos adimensionales y dominio escalar válido |
| `sin`, `cos`, `tan` | Ángulo explícito convertido exactamente a radianes; se restaura el modo global |
| Expresión sin unidades | Ruta previa, incluido RAD/DEG |
| Cancelación completa | Escalar ordinario, con factores ya aplicados |
| Otras funciones, matrices, tutor, cantidades complejas/simbólicas | Capacidad pendiente, sin devolver un número desnudo |

La admisión deriva de identidad, prefijos, `conversion`, `exactness`, factor
positivo y origen cero: **190 de 202 definiciones**, **1.918 variantes atómicas
ofrecidas** y **3.612 de 3.672 variantes de unidades del proveedor**. Las otras
60 variantes tienen diagnóstico pendiente. Una composición puede contener varias
definiciones; estos conteos no se suman como si fueran unidades distintas.

| Motivo pendiente | Definiciones conservadas en Toolbox |
|---|---|
| Afín | Celsius, Fahrenheit, Réaumur |
| Medida/incertidumbre | Dalton/unidad de masa atómica unificada |
| Aproximación convencional | Boiler horsepower, debye, tsubo |
| Histórica | Vara de Burgos; fanega superficial, fanega seca, cántara de vino y arroba de aceite castellanas de 1852 |

Las **58 referencias físicas/contextuales**, con 154 variantes, siguen
pendientes: las medidas indican incertidumbre; las constantes físicas exactas
indican que su familia aún no está integrada; Mach, gases, calendario y escalas
contextuales no reciben un factor ficticio. No se habilita toda `NodeQuantityReference`.

Los factores siguen siendo racionales, potencias decimales y potencias exactas
de π. **kg es kilo+gramo**, `(cm)²` eleva también el prefijo, kW·h prefija W,
y el 100 de `L/(100·km)` permanece en el descriptor. No se habilita como
conversión multiplicativa la reciprocidad consumo ↔ distancia/volumen.
Los extremos Q/q se prueban sin volver cero el exacto; se descarta una
aproximación que subdesborde a cero.

### Magnitud y representación

Una unidad introducida conserva procedencia. Multiplicar/dividir por un escalar
la mantiene; sumar procedencias distintas la pierde. Las reglas locales son
V/A → Ω, A·Ω → V, W·tiempo → J, A·tiempo → C y V·A → W. La magnitud conocida
permite una unidad coherente de esa familia; si falta, se usan bases.

No se infiere Hz/Bq, Gy/Sv ni energía/momento solo por exponentes. El ejemplo
compuesto devuelve **100 s⁻¹**; el usuario puede elegir Hz sin factor adicional.
Elegir una salida no cambia la procedencia canónica ni demuestra que se haya
inferido frecuencia. Un consumo con dimensión de área no recibe automáticamente
«Superficie» como interpretación física.

### FORMAT

FORMAT corto alterna exacto/decimal conservando la unidad. El menú avanzado
ofrece Standard, Decimal, SCI, ENG, FIX y **Unidad de salida** para cantidades.
TABLE, Steps y los demás modos conservan su contrato fuera de esa frontera;
Steps no se ofrece como desarrollo dimensional no implementado.

Unidad de salida permite SI predeterminado, elegir una unidad completa y cambiar
cada componente. Reutiliza pestañas, búsqueda, favoritos y prefijos de Toolbox
en modo destino. Filtra categorías vacías y unidades incompatibles; al confirmar
revalida dimensión, capacidad, generación del resultado y receptor. Navegar no
ejecuta Giac ni crea un árbol por variante. La elección de destino no inserta
en el editor ni añade Recientes.

20 m/s → longitud km → tiempo h produce 72 km/h. En m/s² el exponente del tiempo
permanece −2; cm² aplica el cuadrado del factor. SCI/ENG/FIX formatean el
coeficiente de la unidad elegida; ENG no cambia además el prefijo.

Cada cambio parte del **canónico**, prepara coeficiente y AST completos y después
publica ambos. Cancelar conserva la vista anterior. Edición/nuevo resultado
invalida una selección pendiente. La salida usa unidades en redonda y negro,
productos, potencias y fracciones reales STIX, no texto plano con asteriscos.

### Ans, PreAns, A–F e historial

Ans/PreAns conservan la cantidad inmutable. El espejo de Giac recibe `undef`
para impedir que otra ruta lea solo su coeficiente. El historial mantiene
**50 entradas**, con AST de entrada, resultado y cantidad. Recuperar prepara
clones/vista antes de sustituir el estado y vuelve a SI sin cambiar el valor.
Un error conserva entrada/cursor y Ans/PreAns; el último resultado válido sigue
recuperable en historial mientras la UI presenta el diagnóstico.

STO A–F admite cantidades **solo durante la sesión** y muestra ese aviso.
No amplía VR01/VR02. Antes de aceptarlas prepara y comprueba un snapshot duradero
con el slot a cero, evitando que resucite un escalar antiguo. Comprueba longitudes
escritas, lectura íntegra después del cierre y rename sin retirar antes el
archivo activo. Si falla conserva memoria/snapshot anteriores e informa.
Los valores no persistibles no serializan campos numéricos obsoletos; recargar
invalida la cantidad de sesión mediante las revisiones existentes.

### Integración posterior

`admissible` y `Error` delimitan nuevas capacidades. Afines necesitará el
contrato temperatura punto/diferencia; referencias medidas, incertidumbre
propietaria; persistencia, un esquema versionado independiente. El descriptor
ya contiene componentes, prefijos, potencias y factores internos: convertir no
requiere analizar símbolos.

WASM-MATH conserva su API escalar: no enlaza Quantity/Toolbox/LVGL ni expone
memoria/filesystem. Equations/Grapher/Calculus mantienen el filtro de capacidades.

## Memory & Typographic Profile

| Target / etapa | Imagen | Flash enlazada | RAM estática | IRAM texto |
|---|---:|---:|---:|---:|
| WROOM recibido | 6.234.032 | 6.233.673 | 119.608 | 60.407 |
| Tras compactación | No realizada; igual al recibido | Igual | Igual | Igual |
| WROOM final | **6.259.984** | **6.259.621** | **119.672** | **60.407** |
| CAM recibido | 6.146.256 | 6.145.885 | 118.200 | 59.039 |
| CAM final | **6.172.544** | **6.172.173** | **118.264** | **59.039** |

WROOM crece **25.952 B de imagen**, 25.948 B enlazados y 64 B de RAM estática.
Quedan **293.616 B** de la partición de 6.553.600 B, 162.544 B por encima del
mínimo presupuestado. No cambian particiones, dependencias, clocks, seguridad ni
pool LVGL de 64 KiB. Son imágenes compiladas, no firmware instalado identificado.

DWARF ESP32-S3: `Value` 44 B, `Dimension` 17 B, `Descriptor` 112 B, `Display`
56 B, `CalculationEvaluation` 200 B, `SessionExact` 112 B, `HistoryEntry` 160 B,
`CalculationApp` 1.000 B. `NodeUnit`/`NodeQuantityReference` siguen en 52 B y
`NodeRow` en 56 B. No hay copia del registro por resultado.

Marcos propios Xtensa: `contains` 1.696 B, exponente 640 B, conversión 496 B,
commit Ans 432 B, historial 416 B, visita recursiva 352 B. Análisis limitado
a profundidad 16, 192 nodos, exponentes −64…64, denominador/grado hasta 16,
64 llamadas escalares, texto de 2.000 bytes y 17 componentes; salida limitada
a 4.096×1.024. Escaneo de presencia: 400 posiciones. El stack de loop previo
de 65.536 B no cambia. Marcos propios no equivalen a una cota total con Giac.

Muestras native `new/new[]` + AST en un recorrido de 123456789123456789 m:
primera invocación de cada ámbito, salvo historial, que selecciona la cantidad
en la segunda llamada después de pasar por el escalar más reciente:

| Ámbito | Incremento pico muestreado | Delta al salir |
|---|---:|---:|
| Análisis | 304 B | −45 B |
| Conversión | 1.368 B | 1.153 B |
| Composición AST | 256 B | 240 B |
| Ans | 31 B | 31 B |
| STO | 307 B | 19 B |
| Preparar selector | 2.104 B | 2.104 B |
| Publicar destino | 1.592 B | 196 B |
| Formato numérico | 864 B | 411 B |
| Recuperar historial | 3.678 B | 2.697 B |

Se excluyen `malloc` de C y LVGL; envoltorios C++ de Giac pueden contribuir.
No se ha aislado el máximo de su allocator interno. Los deltas positivos incluyen
objetos retenidos; el negativo libera propiedad anterior. No son tamaños ESP32
ni máximos globales de heap/PSRAM.

Los **100 ciclos comparables**, tras cuatro de calentamiento, llenan historial
con 60 evaluaciones antes de cada ciclo, convierten, navegan prefijos, usan
favoritos, recuperan historial, provocan error y vuelven a HOME sin reset Giac.
Mínimo LVGL **muestreado**: **6.656 B** libres; bloque mayor **5.368 B**.
Las 100 fronteras HOME mantienen 67 objetos, 3 timers, 19.352 B libres y cero
handles. Pasan además 50 ciclos FORMAT con EN/ES/pool fijo. No se extrapola a
latencia ni máximos físicos de heap o pila.

Layout/dibujo/cursor usan geometría compartida anterior. Pasan spacing, brackets,
notación y viewport. Una cantidad de 62 dígitos se desplaza con tecla mantenida,
se estabiliza en el extremo y vuelve a sus píxeles iniciales, conservando Ans.

## Pruebas y evidencias

Evidencias bajo `out/calc-units-01/`; binarios host aislados bajo
`C:/.codex-cache/calc-units-01/`. No se promueven goldens ni se amplían máscaras.

| Puerta | Resultado / evidencia |
|---|---|
| Núcleo | 42 casos dirigidos y 60 escenarios racionales generados: suma, producto asociativo, dos conversiones independientes y recuperación; **6.329 aserciones** (`verified-host`) |
| Datos | 190 definiciones, 1.918 variantes atómicas, 3.612 composiciones admitidas, 60 pendientes; **105 factores independientes** |
| Producto native | 23 recorridos reales, 13 ejemplos mínimos, componentes, SCI/ENG/FIX, Ans/historial, A–F, cuatro locales (`verified-events`) |
| Viewport de cantidad | Resultado largo, repetición, extremo, vuelta al origen y Ans (`quantity-viewport-held`) |
| Asignaciones fallidas | 22 puntos alcanzados en análisis, conversión, composición, Ans, STO, selector, publicación, formato e historial de cantidad (`faults-release`) |
| Guardado | 9 casos; apertura, escritura corta, lectura de verificación y rename fallidos preservan datos (`storage-verified`) |
| Mutaciones | **12/12 rechazadas** (`mutations-verified`) |
| Corpus previo | **900/900 trazas**, solo tiempos excluidos según contrato (`corpus900/comparison.json`) |
| Editor y catálogo | Entrada 113; Toolbox 207; receptores/apps 279; catálogo 36; extensión 26 y 16 ayudas reevaluadas con el nuevo estado esperado |
| Infinito | 22 comprobaciones de compacto/procedencia/listas; `infinity_presentation_checks` |
| Presentación previa | 29 FORMAT/ENG/TABLE con observer, 7 viewport, brackets native; Tutor/composición, i18n, spacing y notación host |
| Ciclos | 100 mixtos con historial lleno y 50 FORMAT, pool fijo (`lifecycle-final`, `format-lifecycle-final`) |
| Web cantidades | Chromium/Firefox/WebKit × shell/componente; favoritos IDBFS; gate final incluye STO/recarga sin resucitar escalar (`web/quantities-verified`) |
| Web FORMAT previo | Seis superficies; teclado/puntero, SCI/ENG y ráfagas rápidas (`web/format-verified`) |
| Idiomas web | 24 combinaciones; ocho acumuladas WebKit (`web/locales`, `web/webkit-accumulated`) |
| WASM-MATH | Release/Debug en tres navegadores; 322 dependencias contrastadas, sin targets UI (`math-headless-report.json`) |
| Builds/publicación | Native, WROOM y CAM; publisher/lanzador habitual (`publication-final.log`, `launcher-final.log`) |

Mutaciones: kilo omitido, prefijo aplicado una vez en cm², kg/g, cancelación sin
escala, suma incompatible, etiqueta sin coeficiente, doble escala en Ans, unidad
como escalar, referencia borrada, afín como factor, nominal medido exacto y
pérdida del 100 de L/100km. Los esperados no se generan de la tabla probada;
ida/vuelta no es el único oráculo. Se separan casos, variantes y píxeles.

Web usa barreras observables de entrada/publicación y recarga real, sin sleeps
fijos para decidir que terminó. Los frames deterministas native delimitan la
ejecución del harness; no acreditan latencia física.

La galería contiene 36 PNG originales de 320×240, HTML con estilos locales y
manifiesto de hashes. Se comprueban dimensiones, recursos relativos y ZIP
(solo PNG/HTML/JSON). Incluye el ejemplo, Hz, m/cm, cm², km/h, Ω, J/kW·h,
componentes, error/recuperación, Ans tras convertir, referencias pendientes y
en-US, en-GB, es-ES, es-419.

### Archivos exclusivos de esta fase

Respecto al snapshot A, no a HEAD, que incluye revisiones anteriores sin commit:

```text
docs/CALC_UNITS_01.md
platformio.ini
scripts/build-toolbox-allocation-probe.py
scripts/package-calculation-quantities.py
scripts/profile-quantity-host.py
scripts/profile-toolbox-firmware.py
scripts/test-calculation-quantities.py
scripts/test-expanded-unit-catalog.py
scripts/test-quantity-faults.py
scripts/test-quantity-lifecycle.py
scripts/test-quantity-mutations.py
scripts/test-quantity-viewport.py
scripts/test-tutor-composition-host.py
scripts/test-unit-catalog.py
src/apps/CalculationApp.cpp
src/apps/CalculationApp.h
src/hal/NativeHal.cpp
src/math/CalculationEngine.cpp
src/math/CalculationEngine.h
src/math/CalculationFormat.h
src/math/InputRowSyntax.h
src/math/Quantity.cpp
src/math/Quantity.h
src/math/VariableManager.cpp
src/math/VariableManager.h
src/math/giac/GiacEngine.h
src/ui/Toolbox.cpp
src/ui/Toolbox.h
tests/fixtures/quantity_factor_oracles.inc
tests/host/fakes/storage/FS.h
tests/host/giac_units_probe.cpp
tests/host/production_storage_test.cpp
tests/host/quantity_calculation_checks.cpp
tests/host/run_production_storage_tests.py
tests/host/unit_catalog_checks.cpp
tests/wasm/calculation-driver.mjs
tests/wasm/calculation-quantities-focused.mjs
tests/wasm/calculation-quantities.mjs
tests/wasm/system-locales.mjs
tests/wasm/unit-catalog.mjs
```

`platformio.ini` solo añade `Quantity.cpp` al filtro native en este delta. Los
tests anteriores actualizan la guarda provisional, el enlace del renderer para
spacing y la tecla física STO; no relajan tolerancias ni resultados del corpus.

## Risks & Edge Cases

- Afines, incertidumbre, referencias y cantidades simbólicas/complejas siguen
  pendientes. Las cantidades de A–F son de sesión; no hay tutor dimensional.
- Exponentes dimensionales enteros: una raíz que requiera fracciones queda fuera
  del contrato aunque Giac conozca su coeficiente.
- Los dos cierres acumulados WebKit de UNIT-CATALOG-02 se conservan sin causa
  demostrada. El barrido acumulado actual pasó; no demuestra una corrección ni OOM.
- Siguen los límites anteriores de `parMixed` y Grapher con pool fijo.
- Se conservan los intentos fallidos del harness: FORMAT sin observer, mapeos
  de Ans/STO, puntos de inyección no alcanzados y perfil que seleccionaba primero
  la entrada más reciente del historial. Se corrigieron las pruebas, sin ocultar
  logs ni promover capturas.
- El host no sustituye stack high-water, fragmentación, latencia y recuperación
  de filesystem en PCB. No hubo validación física.

### Checklist físico pendiente de autorización

1. Identificar modelo, partición y firmware instalado antes de cualquier flash.
2. Probar los 13 ejemplos, prefijos extremos y cuatro locales en pantalla.
3. Medir heap interno/PSRAM, bloque mayor, stack high-water y watchdog con
   historial lleno, conversiones y HOME.
4. Comprobar teclas mantenidas, cancelación, cursor, pan y consumo de EXE.
5. Comprobar STO de sesión/reinicio, fallos recuperables y preferencias; no
   presentar las cantidades como duraderamente guardadas.
6. Registrar aparte Grapher/parMixed y contrastar muestras host sin equiparar tiempos.

## Alternatives Considered

Usar unidades de Giac directamente perdía factores/identidades en el perfil
compilado. La adaptación escalar exacta conserva una sola autoridad de datos.
La propiedad inmutable evita ampliar cada nodo; reutilizar el proveedor en
modo destino conserva navegación y favoritos sin otro árbol de widgets. No se
añade inferencia física general, CAS alternativo ni compactación innecesaria.

La publicación local comprobó ejecutable y cuatro DLL, smoke 2+3, reemplazo
atómico y copia `previous-program.exe`. El lanzador habitual evaluó también el
ejemplo compuesto y Hz. No se cerró la sesión del usuario; la versión anterior
permaneció accesible durante las compilaciones aisladas.

Asunto orientativo para un futuro commit, **no creado**:
`feat(calculation): evaluate quantities and convert output units`.
