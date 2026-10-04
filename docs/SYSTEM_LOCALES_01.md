# SYSTEM-LOCALES-01 — Preferencia regional del sistema

Fecha: 3 de octubre de 2026. La selección de idioma pertenece ahora al sistema
NumOS, con cuatro variantes: **English (US)**, **English (UK)**,
**Español (España)** y **Español (Latinoamérica)**. Esta entrega permite ir
traduciendo las aplicaciones progresivamente: no declara traducido todo el SO.

## Preferencia única y compatibilidad

`src/i18n/Locale.h` es la fuente de la identidad regional. Contiene una sola
variable de presentación `numos::i18n::productLocale`, independiente de las
instantáneas matemáticas, de las unidades, del CAS y de las memorias de variables.
`src/math/tutor/Locale.h` conserva los nombres antiguos como aliases de esa misma
preferencia; no crea un segundo idioma para Tutor.

| Variante | Etiqueta | ID persistente | Posición en Ajustes |
| --- | --- | ---: | ---: |
| Inglés estadounidense | `en-US` | 0 | 1 |
| Inglés británico | `en-GB` | 4 | 2 |
| Español de España | `es-ES` | 1 | 3 |
| Español de Latinoamérica | `es-419` | 5 | 4 |

Los IDs 0 y 1 mantienen el significado de los registros antiguos de inglés y
español. Los IDs 2 (`French`) y 3 (`Pseudo`) siguen disponibles para las pruebas
internas de presentación de Tutor; no se ofrecen ni se aceptan como preferencias
persistentes de producto. Un ID desconocido se normaliza a English (US).
La posición de la fila nunca se guarda como identidad.

`baseLocale()` permite a una traducción compartida EN/ES atender a ambas regiones
sin perder el ID regional. `isSpanish()` incluye España y Latinoamérica.
`isEnglishUK()`, `localeTag()` y `localeDisplayName()` permiten a los catálogos
elegir diferencias concretas, como `meter`/`metre` y `liter`/`litre`, sin modificar
símbolos ni definiciones matemáticas. Las diferencias regionales no existentes
se resuelven a la traducción común; no se inventan cambios de vocabulario.

## Interacción y superficies adaptadas

La última fila de Settings/Ajustes se llama **Language/Idioma**, sin la
restricción «Tutor». Muestra el nombre regional completo. DERECHA o EXE avanzan
US → UK → España → Latinoamérica; IZQUIERDA recorre el orden inverso. El pie
indica la posición `1/4`…`4/4`. Las repeticiones de una tecla mantenida no cambian
varias veces el idioma. No se añaden widgets ni otra pantalla de configuración.

En esta entrega:

- Ajustes traduce su título, filas, precisión, unidad angular y ayudas; los
  nombres regionales utilizan los glifos españoles ya presentes.
- Calculation utiliza el idioma base para los mensajes y FORMAT que ya tenían
  versiones EN/ES, incluido el aviso de cálculo de cantidades aún no disponible.
- Tutor comparte sus mensajes entre las dos variantes de cada idioma, conserva
  los locales internos de prueba y no vuelve a resolver las matemáticas al
  cambiar de idioma.
- Equations sincroniza el idioma al reabrirse. Si conserva un resultado o unos
  pasos, actualiza la presentación conservando la instantánea y la página.
- Toolbox y el registro de unidades consumen la misma preferencia regional.
  Su integración y cobertura se describen en el informe del catálogo ampliado.

Las aplicaciones y textos que todavía estaban solo en inglés siguen siendo
trabajo de traducción posterior. La preferencia no cambia separadores de
expresiones, sintaxis CAS, unidades de medida, modo angular ni memorias del
usuario. `es-419` es una selección común latinoamericana; no implica que todos
los países compartan convenciones legales de medida.

## Persistencia y recuperación

El codec de `src/apps/CompactSettingsRecord.h` conserva el registro web **ST01 de
10 bytes, versión 1**, y se reutiliza en el emulador Windows. El idioma ocupa el
mismo byte 9. La representación explicita cada byte y no depende del padding ni
de la endianidad de una estructura C++. Se valida tamaño exacto, firma, versión,
booleanos y precisión antes de modificar ajustes. Una versión desconocida o un
registro incompleto no cambia el estado activo.

El registro WROOM **NST2 de 16 bytes** conserva versiones 2/3 y checksum.
Su byte 10, cuyo nombre de campo heredado es `tutorLanguage`, guarda ahora el ID
de sistema. No cambia el formato ni la política existente de brillo. No se añade
persistencia nueva a la plataforma CAM en este cambio.

En native/web primero se escribe y cierra un candidato completo `settings.tmp`;
solo entonces se reemplaza `settings.dat`. La implementación Windows de
`LittleFSClass::rename` usa reemplazo de destino con `MoveFileExA`, coherente con
la API de rutas estrechas ya utilizada por el wrapper. En POSIX/web se conserva
`rename`. No se borra el registro anterior para forzar la sustitución. Si esta
falla, se mantiene el anterior y se puede reintentar. La simulación de un destino
bloqueado comprueba también la conservación del candidato completo.

Esta mejora no transforma el wrapper entero a rutas Unicode de Windows: conserva
el contrato existente de `fopen`/rutas estrechas. Los harnesses usan rutas relativas
o raíces ASCII aisladas. No se afirma soporte nuevo de cualquier ruta Unicode.

## Validación y límites de las mediciones

Pruebas enfocadas incorporadas:

- `tests/host/system_locale_checks.cpp`: 579 comprobaciones de IDs y orden,
  idiomas base, roundtrip de ambos codecs, 256 bytes de idioma posibles,
  registros inválidos y conservación del estado anterior. Ejecutadas sin fallos.
- `tests/host/system_locale_storage_checks.cpp`: 42 comprobaciones del VFS real
  en Windows: guardados repetidos, recuperación, candidato inexistente, destino
  bloqueado y reintento tras desbloquear. Ejecutadas sin fallos.
- `tests/host/tutor_i18n_checks.cpp`: comprueba que los mensajes de UK y
  Latinoamérica coinciden con las traducciones base disponibles; incluye IDs
  regionales y rechazo seguro de los locales de prueba en almacenamiento.
- `scripts/test-system-locales.py`: selección por eventos del editor real de
  Ajustes, persistencia de las cuatro variantes, arranque de otro proceso,
  comparación de la pantalla 320×240 tras recargar, retorno a `2+2`, versiones e
  IDs inválidos. Ejecutados los siete casos (cuatro variantes y tres controles
  negativos), sin fallos. Las cuatro pantallas recargadas coinciden píxel a píxel
  con las seleccionadas, excluyendo la barra de estado. Las capturas originales
  320×240 y el resultado están en
  `out/unit-catalog-02/locales/events/`. La revisión visual confirma que se leen
  completos `English (UK)` y `Español (Latinoamérica)`, sin solapamientos.
- `tests/host/production_demo_contract_test.py` y
  `tests/host/run_production_storage_tests.py`: contrato y ocho escenarios de
  arranque WROOM con VFS simulado, sin fallos. Los arranques no formatean ni
  escriben almacenamiento. Se conserva el log en
  `out/unit-catalog-02/locales/production-storage.log`; no es una prueba física.
- `tests/wasm/system-locales.mjs`: **24 combinaciones distintas verificadas en
  el candidato anterior a la corrección final de dos etiquetas del catálogo**
  (Chromium, Firefox y WebKit × shell/componente × cuatro regiones). En cada
  superficie se reemplaza la preferencia mediante teclas, se fuerza el flush
  público y se recarga la página en el mismo contexto IDBFS. Las pantallas de
  Ajustes conservan sus píxeles tras la recarga; `2+2` vuelve a dar `4`, la misma
  búsqueda `metre` selecciona la misma identidad tipada y las variantes US/UK
  muestran respectivamente `Meter`/`Metre`. No se expone memoria ni filesystem.
  Evidencia consolidada: `out/unit-catalog-02/web/locales-combined.json`.
- `tests/wasm/unit-catalog.mjs`: las seis superficies navegador/producto pasan
  el recorrido de Toolbox, unidades y favoritos con recarga IDBFS real.
  Evidencia: `out/unit-catalog-02/web/units/results.json`.

**Límite observado del barrido web acumulado:** en dos ejecuciones WebKit cerró
el target durante la primera recarga del componente después de completar los
cuatro idiomas del shell. El segundo cierre se reprodujo sin ejecutar el gate
de unidades en paralelo. Su causa no está demostrada; no se atribuye a un OOM
ni se presenta como una reparación del producto. Los cuatro casos del componente
pasaron al ejecutarlos en un proceso nuevo. El consolidado contiene los 20 casos
originales y esos cuatro adicionales, sin contar las repeticiones como cobertura
nueva. Los fallos se conservan en `web/locales-final.log` y
`web/locales-webkit-isolated.log`, bajo la misma raíz de evidencia.

El runner reproducible aísla ahora cada superficie en su propio proceso de
navegador, mantiene las cuatro regiones y sus recargas dentro del mismo contexto,
y registra eventos `crash`, `close` y `disconnected`. Su sintaxis se comprobó con
`node --check`; la cobertura anterior prueba los mismos casos bajo ese aislamiento
equivalente. No cambia aserciones, máscaras ni goldens y no usa esperas fijas.
Los comandos de compilación/empaquetado y hashes de los archivos web probados
están en `out/unit-catalog-02/web/commands.json` y `package-hashes.json`.

### Artefacto final: etiquetas regionales de las referencias de gas

La última corrección del generador hace llegar las etiquetas estadounidenses
de las referencias 137/138 a las tablas del proveedor: `Normal cubic meter of
gas` y `Standard cubic meter of gas`; UK conserva `metre`. Solo cambiaron las
tablas `UnitVariantMetadata.inc` y `ReferenceToolboxEntries.inc`, sin cambios
del AST, layout, codec ni política de idioma.

Sobre el paquete reconstruido se verificaron **10 casos distintos de idioma**:
las cuatro regiones en el componente Chromium y WebKit, más US/UK en Firefox.
Cada caso conserva el guardado y la recarga IDBFS, `2+2`, búsqueda de Metro y
ahora ambas referencias de gas. Las consultas ASCII idénticas `Normal cubic`
y `Standard cubic m` seleccionan IDs de catálogo 16521/16522; las capturas US/UK
deben diferir por las etiquetas regionales. La revisión de los originales
320×240 confirma nombres completos y símbolos `Nm³`/`Sm³`. Navegar y buscar
no llama a Giac. No hubo cierres inesperados ni errores de página en esta matriz.
El gate `unit-catalog.mjs` se repitió también sobre este artefacto y pasó las
seis superficies: shell y componente en Chromium, Firefox y WebKit, incluidos
favoritos y recarga IDBFS. Sus resultados están en `final-labels/units/`.

La evidencia final se conserva separada en
`out/unit-catalog-02/web/final-labels/`: `locales-combined.json`, los tres
subdirectorios `locales-*`, `commands.json` y `package-hashes.json`. El WASM
probado mide **7.670.364 bytes**, SHA-256
`a558aab38b337a1e7cbf205e8c698924e1cd44a1fe036044229d95ae6d05126a`.
El paquete anterior permanece en `web/prior-package/`; su matriz de 24 casos
no se presenta como una repetición completa sobre este último binario.

Los runners EN/ES anteriores avanzan ahora dos veces desde US para seleccionar
España. NativeHal ya creaba una raíz temporal limpia para cada ejecución con
`--script` o `--deterministic`. Los runners Toolbox y unidades hacen además
explícita una raíz única por caso, conservando la compartida en las pruebas de
favoritos que verifican un roundtrip.

La preferencia sigue ocupando un byte; los metadatos son inmutables. El selector
reutiliza las mismas filas, etiquetas y fuentes. No introduce reservas de heap en
layout matemático, dibujo ni cálculo de cursor. El codec usa un registro acotado
de 10 bytes; la ayuda temporal de Ajustes usa un buffer local de 64 bytes. Estas
cifras describen objetos concretos, no el máximo de pila/heap de ESP32. Los tamaños
finales de RAM, flash e IRAM corresponden al informe de compilación de la entrega.

## Archivos

Infraestructura: `src/i18n/Locale.h`, `src/math/tutor/Locale.h`,
`src/apps/CompactSettingsRecord.h`, `src/demo/DemoSettingsRecord.h`.

Producto: `src/apps/SettingsApp.h/.cpp`, `src/apps/CalculationApp.cpp`,
`src/apps/EquationsApp.cpp`, `src/math/tutor/Messages.inc`,
`src/hal/FileSystem.cpp`, `src/hal/NativeHal.cpp`.

Pruebas: los tres checks host de idiomas citados arriba,
`scripts/test-system-locales.py`, `tests/wasm/system-locales.mjs`, los dos
harnesses de contrato/almacenamiento de producción y los preparativos EN/ES de los runners
`test-toolbox.py`, `test-unit-catalog.py`, `test-calculation-format-lifecycle.py`,
`test-calculation-format-presentation.py`, `test-tutor-i18n-failure.py` y
`test-tutor-i18n-ui.py`.
