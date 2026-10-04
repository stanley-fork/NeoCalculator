# UNIT-CATALOG-02 — catálogo ampliado e idiomas regionales

Continuación de UNIT-CATALOG-01, 3 de octubre de 2026. El catálogo incorpora las
**353 filas del adjunto** como unidades, composiciones, constantes físicas o
escalas con contexto, conservando sus diferencias matemáticas. Ajustes ofrece
**English (US), English (UK), Español (España) y Español (Latinoamérica)** como
preferencia del sistema. La traducción del resto del SO continúa progresivamente.

**El cálculo dimensional y las conversiones siguen pendientes de CALC-UNITS-01.**
Insertar una unidad o referencia conserva el árbol, permite editarlo y recuperarlo,
y al evaluar muestra el aviso deliberado de función todavía no disponible. No
se publica un número que omita la unidad ni se modifica Ans.

## Architecture Assessment

Se mantiene el proveedor del popup existente, con cuatro pestañas y cuatro filas
reutilizadas. Registro, identidades y metadatos permanecen separados de LVGL y
Giac; los descriptores inmutables están en flash. No se crean miles de widgets ni
árboles de prefijos, ni se amplía el pool LVGL de 64 KiB.

La base real se preservó en `out/unit-catalog-02/A-received`: 1.402 archivos,
53.331.761 bytes de fuentes, manifiesto SHA-256, parche binario desde HEAD,
archivos nuevos, índice, worktrees, instrucciones y ejecutable con sus DLL.
La rama era `main`, HEAD `15b9536ff848549c08f883ca960b1beeeb28bdb5`, con el
candidato local posterior a UNIT-CATALOG-01. No se reconstruyó desde HEAD.
Los checkpoints separados de Toolbox, ±∞ y unidades de la entrega anterior
siguen en `out/unit-catalog-01/`; esta ampliación no mezcla ni rehace esos deltas.

El checkpoint final `out/unit-catalog-02/B-expanded` conserva de nuevo fuentes,
archivos nuevos, ejecutable publicado y runtimes, manifiesto SHA-256 y parche
binario exclusivo respecto a `A-received`. El listado exacto de archivos
añadidos, modificados o eliminados está en
`out/unit-catalog-02/changed-files.json`; `final-delivery.json` reúne hashes de
entrega y rutas de evidencia. No depende de reconstruir los cambios desde un commit.

No se usaron stash, reset, clean, staging, commit, push, despliegue ni flash.
`.vscode/settings.json` mantiene SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.

## Solution

### Una fuente reproducible y cobertura verificable

`src/math/units/registry.json` genera las tablas de unidades, referencias,
entradas de Toolbox y diferencias regionales. `scripts/generate-unit-catalog.py
--check` verifica los artefactos. El [inventario](UNIT_CATALOG_02_INVENTORY.md),
la [cobertura fila por fila](UNIT_CATALOG_02_COVERAGE.md) y el
[informe científico](UNIT_CATALOG_02_DATA.md) detallan fuentes y convenciones.

| Elemento | Cantidad | Significado |
|---|---:|---|
| Definiciones de unidad | 202 | 55 anteriores y 147 adicionales |
| Accesos base de unidad | 312 | Incluyen potencias y composiciones |
| Variantes ofrecidas de esos accesos | 3.672 | Incluyen prefijos, no son otras tantas definiciones |
| Referencias base | 58 | 14 constantes físicas y 44 escalas/contextos |
| Variantes de referencia | 154 | Solo las familias declaradas admiten prefijos |
| Prefijos oficiales | 24 | Más una selección neutral, no otro prefijo oficial |
| Identidades matemáticas anteriores | 152 | Conservadas; 140 descubribles y 12 de compatibilidad |

Las 202 definiciones incluyen 193 relaciones exactas, un valor medido, cinco
correspondencias históricas y tres convenciones aproximadas. No se transforma
un decimal redondeado en igualdad exacta. Se conservan BIPM 9.ª edición V4.01
(junio de 2026), CEM, NIST y las fuentes específicas verificadas; las constantes
usan CODATA 2022, no valores físicos antiguos de UCUM. Las limitaciones de
acceso a determinadas fuentes se registran expresamente en el informe científico.

### Prefijos, variantes y nombres

EXE inserta la variante actual; DERECHA abre su escala completa y centrada.
BACK recupera selección y scroll del padre. `Wh` y `Ah` son aliases de las
composiciones `W·h` y `A·h`; sus prefijos afectan al componente W o A, no a la
hora. `kWh` y `mAh` se buscan directamente y se guardan como variantes exactas.

`Cal` identifica la kilocaloría **termoquímica**, igual que esa selección de
`kcal`. La caloría de Tabla Internacional conserva otra identidad y etiqueta.
Las coincidencias reales, como `pH` —picohenrio y escala de acidez— o `dB`
—decibyte y decibelio— se muestran separadas con nombre y categoría. No se
normalizan símbolos a minúsculas. `mA/MA`, `MHz/mHz`, `Pa/pA` y `s/S` siguen
distinguiéndose. Las equivalencias Unicode se limitan a la frontera de unidades.

Masa sigue anclada en gramo; kg es kilo+gramo. Las potencias permanecen dentro
de la estructura: `(cm)²`, `(cm)³` y potencias hasta cuatro. Las composiciones
contienen hasta cuatro términos identificados, con signos de potencia explícitos.
`L/100km` construye `L/(100·km)` con el número 100 real en el denominador.
Las tasas de muestras, eventos y operaciones tienen componentes de recuento y
tiempo separados. Los dominios de información/recuento no se reducen a escalares
SI por compartir exponentes dimensionales nulos.

### Referencias físicas y escalas contextuales

`ReferenceRegistry.h` y `NodeQuantityReference` separan estas identidades de
`UnitId`, letras libres, memorias y constantes matemáticas. El namespace de
Toolbox es `0x4000 | ReferenceId`, frente a `0x8000 | ItemId` de las unidades;
una identidad con ambos bits reservados se rechaza. Los codecs cerrados usan
cinco bytes y firmas distintas `0x52`/`0x55`, con validación transaccional.

Las constantes guardan dimensiones, nominal, incertidumbre, exactitud, fuente
y fórmula descriptiva. pH, Mach, dB, meses, palabras de máquina y condiciones
de gas conservan su contexto: no se inventa un factor lineal universal. Las
fórmulas del registro no son llamadas CAS ejecutables.

Cada referencia construye privadamente su notación y se edita como un átomo.
Los subíndices y fracciones usan los nodos comunes; `Nm³` y `Sm³` tienen una
potencia real. Las letras base de constantes físicas usan la cursiva matemática
STIX; los subíndices descriptivos, unidades y escalas permanecen en redonda.
La política de letras libres y tinta negra de la entrega anterior no cambia.

La pequeña fuente de apoyo Montserrat añade `è`, `²` y `·`; se conservan el
generador y la licencia. El espacio de símbolos como `fl oz` usa el avance
235/1000 em del STIX original, compartido por medida y dibujo y sin glifo vacío
dibujado como caja. Las ayudas de texto expresan las matemáticas que no caben
en esa fuente mediante notación ASCII legible; los previews mantienen STIX.

### Idioma del sistema y persistencia

La preferencia única vive en `src/i18n/Locale.h`; Tutor conserva aliases
compatibles. US muestra `Meter/Liter` y UK `Metre/Litre`. Las variantes españolas
comparten traducciones donde no hay una diferencia documentada. No se cambia
símbolo, sintaxis CAS, separador decimal, unidad angular ni identidad favorita.

Los IDs antiguos de inglés/español siguen siendo 0/1; UK/Latinoamérica usan 4/5.
El registro web/native ST01 conserva diez bytes y el WROOM NST2 conserva dieciséis.
Windows guarda el candidato completo antes del reemplazo y conserva el anterior
si el destino está bloqueado. Favoritos mantiene sus dos registros recuperables,
24 favoritos y 12 recientes de sesión. Solo la inserción confirmada actualiza
recientes. Véase [SYSTEM_LOCALES_01](SYSTEM_LOCALES_01.md).

## Memory & Typographic Profile

Los tamaños, secciones de firmware y marcos propios de pila se registran en
`out/unit-catalog-02/firmware/resources-firmware.json`. Son imágenes compiladas;
no se instalaron en hardware. Los datos grandes se mantienen inmutables y no
se crean objetos nuevos de LVGL para este catálogo.

| Compilación final | Imagen `.bin` | Flash enlazada | RAM estática | IRAM text |
|---|---:|---:|---:|---:|
| WROOM N16R8 | 6.234.032 B | 6.233.673 B | 119.608 B | 60.407 B |
| CAM `esp32s3_n16r8` | 6.146.256 B | 6.145.885 B | 118.200 B | 59.039 B |

Respecto a UNIT-CATALOG-01, el incremento de flash enlazada es 407.716 B
en WROOM y 407.552 B en CAM; **RAM estática e IRAM no cambian**. La partición
de aplicación WROOM admite 6.553.600 B: quedan 319.568 B respecto a la imagen,
con el 95,1 % ocupado según el enlazador. Este margen condiciona la integración
del motor posterior; no se ha ampliado la partición ni alterado el hardware.

DWARF de Xtensa confirma `NodeUnit = 52 B` sin incremento y
`NodeQuantityReference = 52 B`, aparte de sus nodos de notación. `Session`
mantiene 1.380 B, `Provider` 32 B, `Store` 164 B y `Identity` 4 B. Marcos propios
de pila medidos por desensamblado: `scanUnits` 704 B, búsqueda 512 B,
refresh 432 B, preview 400 B, construcción de unidad 128 B y construcción de
referencia 96 B. Son marcos individuales, no un máximo acumulado de pila.

El registro/proveedor enlazado alcanza **471.186 B de `.flash.rodata`** en ambos
ELF: 238.469 B de tablas y 232.717 B de cadenas alcanzables, deduplicadas por
dirección y sufijos compartidos. Es su footprint final, **no un delta exclusivo**:
algunas cadenas pueden compartirse con otros módulos. No incluye código,
fuentes tipográficas ni relleno. Desglose y método reproducible en
`out/unit-catalog-02/registry-flash-cost.json` y `audit-registry-flash.py`.

En host, `NodeUnit` sigue ocupando 64 bytes y `NodeQuantityReference` ocupa 72,
más su notación construida bajo demanda: hasta seis nodos en los datos actuales.
Layout y dibujo no reservan memoria. La navegación construye únicamente las
vistas previas visibles bajo demanda y nunca llama a Giac.
La búsqueda mantiene sus buffers acotados y una caché de clasificación de símbolo
de 66 bytes; no materializa todos los resultados ni árboles de variantes.

Los 100 ciclos comparables, tras cuatro calentamientos y con 60 cálculos para
llenar historial en cada ciclo, conservaron HOME en 67 objetos, tres temporizadores,
19.352 bytes libres y cero handles. Mínimos muestreados: **5.984 bytes libres** y
**5.000 bytes de bloque mayor** en el pool configurado de 65.536 bytes. Son muestras
host; no representan máximo de heap/pila ni latencia ESP32.

Los ámbitos instrumentados de `pool-lifecycle/cycles.log` incluyen llamadas
anidadas, por lo que sus tiempos no se suman. Muestras bajo carga concurrente
de compilación/navegadores; no son un benchmark de dispositivo:

| Ámbito host | Muestras | Media | Máximo observado |
|---|---:|---:|---:|
| Apertura | 310 | 2,503 ms | 29,365 ms |
| Refresco de filas/previews | 2.580 | 2,941 ms | 193,913 ms |
| Entrada de texto/búsqueda | 104 | 32,053 ms | 269,097 ms |
| Activación | 310 | 1,269 ms | 19,394 ms |
| Cierre | 309 | 0,334 ms | 14,183 ms |

## Pruebas y recuperación

- Registro: 2.187 controles, 106 factores independientes, cruces CODATA y
  17 mutaciones negativas. Cobertura exacta de las 353 filas del adjunto.
- Editor ampliado: 69.665 comprobaciones, construcción de las 3.672 variantes
  de catálogo y 154 de referencia, namespaces, cuatro idiomas, clones, codecs,
  potencias, coeficientes, favoritos recuperados y conservación de Ans. La
  pasada final incluye etiquetas y aliases completos de las referencias de
  gas 137/138 en los cuatro idiomas (`expanded-host-final.log`).
- Registro atómico: 26.500 comprobaciones de unidades y 6.600 de referencias.
- Toolbox: las 207 rutas previas y las 36 de UNIT-CATALOG-01 pasan; la ampliación
  añade 42 recorridos reales y una inspección adicional de consumo de combustible.
- Vistas previas/glifos: 203.240.704 comprobaciones; máximo observado 115×48 px.
  Spacing: 151.538 checks/36 fixtures; brackets: 1.645/17; notación: 33 casos.
  Oráculos independientes verifican layout/dibujo/cursor, sin promover goldens.
- Las **900 trazas matemáticas anteriores** coinciden en todos los campos salvo
  los tiempos excluidos por el contrato de comparación. Composición Tutor e
  i18n pasan 89 y 2.350 comprobaciones adicionales.
- Entrada/edición de Calculation: 113 casos; aislamiento de capacidades en
  Equations, Calculus y Grapher: 279 recorridos; procedencia de infinito:
  22 comprobaciones. Las cantidades no se convierten en letras en aplicaciones
  sin esa capacidad. Se conservan sus resultados en `calculation-input/`,
  `contexts/` y los logs host de `infinity_presentation_checks`.
- Ajustes nativos: cuatro regiones guardadas y recuperadas con imágenes iguales,
  y tres controles de versión/idioma inválidos. Codecs: 579 comprobaciones;
  filesystem real: 42; contrato y ocho escenarios de persistencia WROOM simulada.
- Recuperación: fallos persistentes de apertura, preview, búsqueda, inserción y
  guardado; fallos de las cuatro filas; 100 ciclos de Toolbox y 50 de FORMAT bajo
  pool fijo. No se pierde la entrada ni se publica media composición.

Los logs iniciales de diagnósticos fallidos se conservan junto a las pasadas
finales. La primera selección de `pH`/`dB` del harness asumía ausencia de colisiones;
la prueba final comprueba ambos resultados legítimos. Un sondeo de fallo en la
quinta asignación de un átomo simple no alcanzaba esa asignación; se repitió con
una unidad elevada que sí cubre los puntos especificados, sin debilitar el gate.

### Web, idiomas y cierre del artefacto final

La matriz anterior a las dos últimas etiquetas regionales pasó **24 combinaciones
distintas**: Chromium, Firefox y WebKit × shell/componente × cuatro regiones.
En dos barridos acumulados WebKit cerró el target durante la primera recarga
del componente después de completar los cuatro idiomas del shell. El segundo
cierre ocurrió sin el gate de unidades en paralelo; no se ha demostrado su causa
ni se atribuye a un OOM. El componente pasó sus cuatro regiones en un proceso
nuevo. Los dos fallos se conservan junto al consolidado de 20+4 casos en
`out/unit-catalog-02/web/`; no se cuentan repeticiones como casos nuevos.

La corrección final afecta solo a `UnitVariantMetadata.inc` y
`ReferenceToolboxEntries.inc`: US muestra `Normal/Standard cubic meter of gas`
y UK conserva `metre`. Sobre el paquete reconstruido pasaron **10 casos finales
de idioma/referencias**: cuatro regiones en Chromium componente, cuatro en WebKit
componente y US/UK en Firefox componente. Conservan recargas IDBFS reales, píxeles
de Ajustes, `2+2`, la búsqueda de Metro y ambas referencias de gas. Las consultas
idénticas verifican IDs 16521/16522 y etiquetas regionales distintas, sin llamadas
a Giac. El runner aísla cada superficie en un navegador nuevo y mantiene las
cuatro regiones y sus recargas en el mismo contexto. No hubo cierres inesperados
ni errores de página en esta pasada.

El gate de unidades se repitió sobre el mismo paquete final y pasó sus **seis
superficies** (shell y componente en los tres navegadores), incluidos favoritos,
prefijos e IDBFS. Comandos, resultados, originales 320×240 y hashes están en
`out/unit-catalog-02/web/final-labels/`. El WASM final mide **7.670.364 bytes**,
SHA-256 `a558aab38b337a1e7cbf205e8c698924e1cd44a1fe036044229d95ae6d05126a`.
El paquete anterior y su matriz de 24 permanecen separados en `web/prior-package/`
y `web/locales-combined.json`; no se presentan como 24 repeticiones del último
binario. No se cambiaron máscaras/goldens ni se añadieron esperas fijas al harness.

### Motor headless y regresión matemática

WASM-MATH se recompiló con las cabeceras actuales: cuatro unidades de traducción
y enlace final. Su runtime y WASM son idénticos byte a byte al paquete preservado
de UNIT-CATALOG-01; el WASM mide **4.056.592 bytes**, SHA-256
`730fb5b8155e2957571eff4450a46deca0efaa46ffc07acda11f8e4c665377f2`.
`tests/wasm/math.mjs` pasó en Chromium, Firefox y WebKit, incluidos worker,
evaluación, compilación 1D/2D, lotes y ciclos de cierre/reapertura.

La auditoría de 322 dependencias no encontró diferencias con las fuentes
compiladas ni objetos enlazados de Toolbox, LVGL o del proveedor de unidades.
La API pública permanece intacta. Los informes están en
`math-headless-report.json` y `math-headless-closure-report.json`, bajo
`out/unit-catalog-02/`. Las dos tablas regionales finales pertenecen al proveedor
UI, sin cambios posteriores del AST, layout ni motor; no se vuelven a contar
las 900 trazas como otra ejecución a causa de esas etiquetas.

## Risks & Edge Cases

- La aritmética dimensional, los orígenes de temperatura y la política de
  incertidumbre siguen pendientes; toda cantidad se detiene antes de simplificar.
- Igual dimensión no fusiona Hz/Bq, Gy/Sv, energía/momento, ángulos o recuentos.
- Las escalas con contexto requieren metadatos adicionales del futuro motor.
  La ayuda identifica esa condición y no promete una conversión fija.
- Sigue vigente el límite anterior de `parMixed` y de Grapher con pool fijo;
  no se extiende esta validación host a comportamiento físico no probado.
- El wrapper de filesystem nativo conserva su API de rutas estrechas; no se
  afirma una nueva migración general a rutas Unicode de Windows.
- WROOM conserva 319.568 B de margen de imagen en la partición actual. El motor
  dimensional siguiente deberá presupuestar ese espacio y evitar duplicar tablas.

## Alternatives Considered e integración siguiente

Se reutilizó el registro y el popup en lugar de añadir otra aplicación o pestaña.
Las referencias usan un átomo distinto para evitar que un símbolo familiar sea
interpretado como variable o unidad de conversión fija. Reutilizar la notación
común mantiene sincronizados el cursor, las potencias y los estilos de fuente.

CALC-UNITS-01 debe sustituir la guarda por la ruta de cantidades, resolver los
dominios y contextos declarados, preservar incertidumbres y tratar explícitamente
temperaturas absolutas/diferencias. Debe consumir IDs y componentes, nunca analizar
el texto de la pantalla. Favoritos, prefijos y búsqueda ya conservan esas identidades.
El ejemplo futuro `(2 s / 5 m) × (10 km / 40 s²) → 100 s^-1` continúa pendiente;
la selección automática de Hz también requiere una política de magnitudes.

## Archivos y galería

Los archivos principales son `src/i18n/Locale.h`, `src/apps/CompactSettingsRecord.h`,
SettingsApp, los registros/generador de `src/math/units/`, ToolboxCatalog,
Toolbox, MathAST, CursorController, MathRenderer y CalculationEngine. Los tres
subconjuntos Montserrat se regeneran con `scripts/generate_tutor_spanish_fonts.py`.
Las pruebas nuevas están en `tests/host/*locale*`, `*reference*`,
`expanded_unit_catalog_checks.cpp`, los scripts de catálogo ampliado e idiomas,
sus fixtures independientes y `tests/wasm/system-locales.mjs`.

La galería autocontenida está en `out/unit-catalog-02/gallery/index.html`, con
el ZIP en `out/unit-catalog-02/gallery.zip`: 89 PNG únicos de 320×240 que
representan 95 capturas, cuatro regiones, recursos relativos y hashes comprobados,
sin archivos de fuentes. Se deduplican seis pares de imágenes idénticas manteniendo
su procedencia en el manifiesto. Incluye
prefijos de Wh, calorías, variantes de galón, contextos ambiguos, referencias,
ayudas y una expresión editada con el aviso de cálculo provisional.

La publicación local usa exclusivamente `scripts/publish-emulator-windows.ps1`,
con smoke, runtimes, sustitución atómica y copia anterior. Los builds permanecen
fuera de `C:\.piobuild\numOS\emulator_pc`; el lanzador habitual mantiene esa ruta
estable. No se cierra una sesión del usuario para sustituir un ejecutable bloqueado.

El candidato final ya está publicado, con SHA-256
`d1663ac74aeddae9b5eb136a16bf351d307755078bb4d13f5ddbbee877393ed9`.
El ejecutable anterior se conserva
como `previous-program.exe`, SHA-256
`3404c8a11a0873f3f96118b8226cf75606d1892fde5b6ff2150872a89053a86d`.
El lanzador habitual pasó el smoke de inserción de `Cal`, la guarda provisional,
el cálculo ordinario `2+3=5` y la vuelta a HOME; logs en
`out/unit-catalog-02/publication.log` y `launcher-final.log`.
