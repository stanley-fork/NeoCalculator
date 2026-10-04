# TOOLBOX-UNITS-CLOSEOUT-01

Estado: cierre técnico y aceptación completados. Firmware ordinario restaurado y comprobado; commits locales de código creados.

## Architecture Assessment

Se conserva la base acumulada desde `15b9536ff848549c08f883ca960b1beeeb28bdb5`,
en `main`, con índice inicialmente vacío. La separación entre editor tipado,
registro inmutable, proveedor Toolbox, coeficiente escalar Giac y cantidades
propietarias se mantiene. Este cierre no añade capacidades dimensionales ni
cambia layout, fuentes, particiones, dependencias o el pool LVGL de 64 KiB.

La autorización de esta conversación permite actualizar la PCB sin pedir otra
confirmación. Se utiliza exclusivamente para la aplicación y pruebas normales
de ajustes, favoritos y STO. La inyección de fallos de almacenamiento continúa
en host; no hay borrado ni sustitución directa de datos físicos.

## Solution

### Preservación y fronteras

Evidencia local duradera, ignorada por Git: `out/toolbox-units-closeout-01/`.
`A-received/` contiene el snapshot completo de 1.440 archivos, parches binarios
separados de índice y working tree, archivos nuevos con hashes, estado de rama
y worktrees, configuración protegida y ejecutable publicado con cuatro DLL y
copia anterior. Se comprobó su identidad con `CALC-UNITS-01/C3-final`.
Se preservan también los checkpoints históricos `A-catalog`, `C1-core`,
`C2-product`, `C3-final` y sus manifiestos: el delta de 40 archivos desde
`A-catalog` nunca se trató como el delta completo desde HEAD.

Las fronteras de código propuestas son:

1. Toolbox, publicación segura, catálogo tipado y locales: el checkpoint real
   `A-catalog`, más la corrección de ALPHA físico y limpieza documental.
   Son 114 archivos respecto a HEAD. No se inventa una versión intermedia de
   los nodos, proveedor y locales compartidos para obtener tres commits.
2. Cantidades en Calculation y correcciones de este cierre. Se materializa el
   candidato final en otro árbol y build directory independientes.

Los árboles limpios están bajo `C:/.codex-cache/toolbox-units-closeout-01/`;
`catalog-snapshot.json` y `quantities-snapshot.json` identifican fuentes y
árboles Git. Ambos compilan native y WROOM con herramientas fijadas; el final
compila además CAM. Las pruebas del primer árbol usan sus propias fuentes,
fixtures y objetos, sin archivos no guardados de la capa posterior.

Herramientas conservadas: espressif32 6.12.0, Arduino ESP32
3.20017.241212 (ESP-IDF 4.4.7), Xtensa 8.4.0+2021r2patch5, esptool 4.9.0,
LVGL 9.5.0, TFT_eSPI 2.5.43 y Giac 1.4.9+khicas.57; native usa MinGW 15.2.
`catalog-verified-build.json` y `quantities-verified-build.json` registran los
comandos y exit code 0. Los avisos de metadata Git de los árboles sin `.git`
se distinguen de fallos de compilación.

### Defectos reproducidos y correcciones estrechas

- **ALPHA físico en Toolbox:** el resolver global consumía ALPHA antes de que
  el popup pudiese abrir Buscar desde su lista. La búsqueda por texto del harness
  ocultaba el defecto. `SystemApp` entrega primero los modificadores al modal;
  el adaptador físico native reproduce esa misma frontera. La reproducción
  falla antes y pasa después con diez aserciones, tecleando `metre` mediante
  contactos generados, sin inyectar texto. Se añade
  `scripts/test-toolbox-physical-modifiers.py`; ambos snapshots lo incluyen.

- **Límite de nodos:** una fila con 192 signos unarios y una unidad llegaba a
  Giac (`error=none`, una llamada), porque el visitante de filas consumía los
  operadores sin contarlos como nodos. `Quantity.cpp` comprueba primero el
  número de hijos y carga también los operadores al presupuesto. Ahora devuelve
  `expression_limit` sin llamar a Giac. `B-node-limit/` conserva antes/después,
  parche y hashes. No se cambian la precedencia compartida ni los límites 192/16.
- **Test autosuficiente:** `quantity_calculation_checks` dependía de un padre
  ignorado de su filesystem ya existente. La ejecución en árbol limpio falló en
  `seed durable scalar`. El test crea sus directorios antes de montar su raíz;
  no cambia la persistencia del producto.
- **Generación documental:** se eliminan espacios finales emitidos por los
  generadores de inventario/cobertura y en el inventario histórico. Se comprueba
  que no cambia ninguna tabla C++ generada.

La revisión enfocada contrasta signo/división/productos implícitos y plantillas
incompletas con el visitante escalar común. Distingue `2·m²` de `(2·m)²` y
rechaza profundidad y exponentes fuera de contrato. La detección precede a la
simplificación: cero y potencia cero no borran unidades o referencias inválidas.

Ans y A–F resuelven cantidades aunque no haya `NodeUnit` literal. Sus espejos
Giac son `undef`, no el coeficiente aislado; un resultado escalar posterior
restaura la ruta escalar. La vista convertida es independiente del canónico
inmutable y del historial. Coeficiente, descriptor, AST y datos de sesión se
preparan antes de publicar, y el receptor comprueba su generación.

`VariableManager` escribe y verifica el temporal completo antes del rename;
no elimina el archivo activo. Guardar una cantidad de sesión confirma primero
el cero durable del slot, evitando resucitar el escalar antiguo. No se añade
persistencia de cantidades ni se rediseña este contrato.

### Controles de software

| Control | Evidencia y resultado |
|---|---|
| Puerta rápida Calculation | `alpha-quick-gate/`, PASS final: 113 recorridos por eventos |
| Núcleo de cantidades | `quantities-host-final/` en caché, PASS; copia esencial en `essential/` |
| Cobertura matemática | 42 casos dirigidos, 60 escenarios racionales generados, 105 factores independientes; 190 definiciones admitidas, 1.918 variantes atómicas y 3.612 variantes de catálogo admitidas; 60 diferidas |
| Límite corregido y agrupación | 13 aserciones enfocadas; antes falla, después pasa |
| Infinito compacto | Suite de presentación y controles de lista, PASS |
| Producto final | 23 recorridos en `native-events/` y repetición aislada del snapshot final |
| Asignación | `faults-final/`: 22 puntos persistentes, PASS |
| Almacenamiento | `storage/`: escritura corta, readback y rename, PASS |
| Mutaciones | 12 detectores rechazados, PASS final |
| Datos normativos | `data-oracles.json`, `unit-data.log`, generador `--check`, PASS |
| Snapshot catálogo | native/WROOM, constructor, alfabetos, catálogo, idiomas, infinito y entrada; 42 aserciones de almacenamiento regional, PASS |
| Trazas escalares | `corpus900/comparison.json`: 900/900, sin promover goldens ni cambiar máscaras |
| Pool fijo | `pool-cycles/`: cuatro calentamientos y 100 ciclos con historial lleno, PASS |
| Navegadores | `web-alpha-quantities/`: Chromium, Firefox y WebKit × shell/componente, favoritos y recarga IDBFS, PASS |
| WebKit acumulado | `webkit-alpha-accumulated/`: ocho combinaciones locale/superficie, PASS con eventos y logs conservados |

Las 3.612 variantes no son definiciones distintas. El registro conserva 202
definiciones, 312 accesos, 3.672 variantes de unidades ofrecidas y 58 referencias
tipadas con 154 variantes. Afines, incertidumbre, referencias contextuales y
familias de constantes físicas pendientes conservan su diagnóstico específico.

La matriz anterior de notación/layout/cursor, Steps, TABLE, VAR/STO, FORMAT/ENG,
contextos, viewport y WASM-MATH se conserva con sus hashes. Las correcciones afectan al presupuesto del evaluador de cantidades y a la
entrega de modificadores al modal; no cambian el AST, renderer ni motor headless.
Sobre el adaptador final se repiten la puerta rápida, Toolbox/contextos,
cantidades, cuatro locales y las seis superficies web. El barrido WebKit
acumulado final pasa ocho combinaciones y conserva también sus logs. La API WASM-MATH sigue siendo escalar, sin
Quantity/Toolbox/LVGL. No se le atribuye soporte dimensional.

### PCB y aceptación

Se redescubrió COM9, USB `303A:1001`, serie/MAC `44:B1:76:A7:B7:2C`, ESP32-S3
con 8 MB PSRAM y 16 MB flash. Secure Boot y Flash Encryption desactivados.
La tabla leída y el selector OTA válido, secuencia 1, identifican `app0` en
`0x10000`, longitud `0x640000`. No se asumieron puerto ni identidad históricos.

La aplicación anterior leída ocupa 5.707.408 B y tiene SHA-256
`251ded7b3264b89b674c5a0495f09a155fa07816610d607873c9296bf39d95c1`.
Su checksum y hash de imagen son válidos. Se conservan imagen, partición completa
y prefijo de 64 KiB en `board-before/`; esos backups no se distribuyen.

La sonda temporal privada está separada del producto en `probe-overlay/` y
`probe-overlay.json`. La escritura solo de aplicación, `verify_flash`, lectura
independiente y comparación del prefijo pasan (`probe-switch/`). La imagen de
sonda tiene SHA-256
`b339827665a9acf879b6d148e4c1f97cd94f92918d3c99c9c49f0b541e16fb75`
La sonda anterior y el intento de lectura USB fallido antes de escribir quedan
en `probe-switch-before-alpha/` y `probe-switch-alpha-read-error/`. La segunda
verificación usa 115.200 baudios; no se ignora el error de transporte.
Un primer intento sin respuesta se conserva: `esptool run` dejó el chip en el
cargador; un RESET USB observado inició el firmware. No se reescribió la imagen.

El transporte de pruebas usa acuses numerados, la actualización normal de la
aplicación y la frontera conocida de 50 ms de SerialBridge. HOME espera además
que la observación de teardown pendiente llegue a cero. No se cargan expresiones
terminadas: las cantidades se construyen mediante teclas, selección del
proveedor real, EXE y RIGHT; se inspeccionan identidades y agrupación del AST.

Han pasado los 14 recorridos principales, con suma/superficie/raíz/masa/velocidad/
electricidad/energía/cancelación, conversión por componentes, Ans tras cambio
de vista, historial, recuperación de error y formatos. El ejemplo compuesto
produce `100 s^-1` y se presenta como `100 Hz` al elegir ese destino.
Los resultados esperados proceden de los oráculos independientes conservados.

Incidente del harness físico conservado: se reutilizó inicialmente el índice de
Idioma del emulador; en WROOM la fila adicional Brillo lo desplaza de 4 a 5.
Dos RIGHT variaron 144→160. Se corrigió la navegación y se restauró 144 mediante
Ajustes, con confirmación de guardado. No fue una corrección del producto.

La revisión humana del ejemplo compuesto queda diferida. El usuario confirmó
que el resultado y la unidad de `72 km/h` se ven claros en el LCD, y señaló
el exceso de pasos y un retraso perceptible al abrir el selector. Se conserva
su respuesta literal en `human-review.json`; las medidas se detallan abajo.

La campaña posterior también confirma:

- ALPHA mediante contactos físicos generados, EXE/RIGHT, Metro centrado
  (selección 12, primera fila 10, 25 opciones), km y µm, mA/MA y cancelación.
  Cuatro eventos mantenidos de EXE no insertan ni evalúan de nuevo.
- `1/2 m/s²` cambia el componente tiempo a horas conservando el exponente:
  `6480000 m/h²`. Cancelar conserva la vista; elegir destino no añade Recientes.
- PreAns y recuperación de una entrada literal antigua tras resultados escalares.
  Un error dimensional no sustituye Ans; la cancelación total vuelve a escalares.
- Diagnósticos de `2 m+3 s`, ceros estrictos, `0*(2 m+3 s)`, `1/(0 s)` y
  `ln(2 m)`, con recuperación `2+2` entre grupos. `0*Mach`, Celsius, radio de Bohr
  y velocidad de la luz conservan sus capacidades pendientes específicas.
- `ln((2 m)/(1 m))`, ángulos explícitos en RAD/DEG y restauración del modo
  global para `sin(90)` después de éxito y error.
- Favoritos `mm` y `km/h` mediante el flujo normal, conservados tras RESET.
  La lista inicial estaba vacía; se registran las dos adiciones de prueba.
- A estaba libre (cero). STO guarda primero 7 y después una cantidad de sesión;
  A/(2 s) produce `1 m/s`. Tras confirmar guardado y reiniciar, A vuelve a cero,
  sin resucitar 7 ni simular una cantidad persistente. B–F permanecen intactas.

`physical-summary.json` separa eventos, aserciones y repeticiones. Se conservan
los intentos fallidos del harness: una consulta ambigua seleccionó diferencia
Celsius antes de precisarse «Degree Celsius»; otro test esperaba reevaluar
PreAns con su estado antiguo, aunque la referencia cambia con Ans. Se corrigieron
las expectativas y consultas, sin cambiar el producto ni ocultar esos logs.

Se ejecutaron dos ciclos de calentamiento y quince mixtos, distribuyendo los
cuatro locales, con sustitución completa del historial de 50 entradas antes de
comparar HOME. Los cálculos y recorridos pasaron. La variación de memoria se aisló y se
completó una segunda serie estabilizada, como se detalla más abajo.

## Memory & Typographic Profile

| Imagen ordinaria corregida | WROOM | CAM |
|---|---:|---:|
| Imagen | 6.260.112 B | 6.172.688 B |
| Flash enlazada | 6.259.745 B | 6.172.325 B |
| RAM estática | 119.672 B | 118.264 B |
| Texto IRAM | 60.407 B | 59.039 B |
| Margen de imagen en partición de 6.553.600 B | 293.488 B | 380.912 B |

Respecto a C3, WROOM aumenta 128 B de imagen y 124 B de flash enlazada;
CAM aumenta 144 B de imagen y 152 B de flash enlazada. Las diferencias de
alineación/integridad se distinguen de las secciones enlazadas. RAM/IRAM siguen
iguales. La sonda de tiempos ocupa 6.281.344 B; la ampliación privada para contar
bloques vivos ocupa 6.281.488 B. Ninguna es la imagen ordinaria.

Las recompilaciones desde dos directorios tienen código y datos idénticos por
comparación de secciones ELF. La diferencia completa en los binarios se limita
al SHA del ELF embebido (`0xb0..0xcf`) y checksum/hash final; se conserva el
contraste byte a byte en `final-directory-comparison.json` (y las comparaciones previas conservadas). No se infiere equivalencia por tamaño.

No cambia el tamaño de nodos ni se añade heap al conteo corregido. La geometría
de medida/dibujo/cursor conserva su código y pruebas. Los frames propios se
registran por separado: `contains` conserva 1.696 B, que no representan la pila
total del recorrido. La sonda comprueba `sizeof(StackType_t)==1` en el puerto
Xtensa fijado y registra el high-water en bytes, sin multiplicarlo por cuatro.
Las muestras físicas se separan por imagen de sonda y ámbito.

### Tiempos físicos repetidos

Milisegundos en la WROOM identificada, con sonda. Primera muestra de cada
recorrido y repeticiones calientes, sin reiniciar Giac entre casos; no se afirma
que cada primera muestra sea un arranque frío. Se conservan todas las muestras,
incluidas las lentas, en `physical-timing-detail.json`, `physical-summary.json`
y los eventos por comando.

| Ámbito | n | Primera | Mediana | Máximo | Mediana sin primera |
|---|---:|---:|---:|---:|---:|
| Ejemplo compuesto | 11 | 43.388 | 43.702 | 43.828 | 43.711 |
| m/cm → escalar | 11 | 30.856 | 31.172 | 31.250 | 31.177 |
| Abrir selector: longitud | 11 | 688.657 | 688.841 | 692.158 | 688.842 |
| Abrir selector: tiempo | 11 | 763.905 | 764.003 | 764.332 | 764.008 |
| Abrir selector: unidad completa | 11 | 862.528 | 862.625 | 864.796 | 862.631 |
| Aplicar km/h completo | 11 | 7.392 | 7.385 | 7.392 | 7.383 |
| Abrir prefijos de Metro | 11 | 27.398 | 27.396 | 27.744 | 27.392 |
| Cambiar prefijo UP/DOWN | 22 | 20.627 | 22.622 | 24.681 | 24.569 |
| 2+2 | 11 | 20.275 | 19.987 | 20.470 | 19.981 |
| 66,63/(2×10^-3) | 11 | 23.680 | 23.652 | 24.269 | 23.651 |
| 6 km / 300 s | 11 | 33.051 | 33.030 | 33.475 | 33.028 |
| 123456789123456789 m | 11 | 38.702 | 38.614 | 39.089 | 38.600 |
| Recuperar historial | 11 | 6.939 | 6.935 | 6.964 | 6.934 |

`evaluateExpression` incluye motor, preparación y publicación del resultado;
no se suma otra vez con sus ámbitos anidados. El selector incluye la apertura
de Toolbox; la conversión confirmada es `publishOutput`. La actualización
forzada de LCD y el ACK se hacen después de esos ámbitos: no son tiempos del
primer píxel. El transporte serial y los observers también pertenecen a la
configuración de sonda y no se extrapolan a un máximo absoluto del producto.

El usuario confirmó legibilidad de 72 km/h en el LCD y describió demasiados
pasos y un tirón al elegir unidad. Se reprodujo la latencia: la apertura del
selector completo para velocidad tiene mediana 862,625 ms, frente a 7,385 ms
para aplicar la conversión. El recorrido por componentes abre dos selectores.
Ya funciona elegir km/h completo en una sola selección; puede recuperarse de
Favoritos. La propuesta de accesos rápidos y la optimización del selector
quedan registradas, sin presentarlas como implementadas en este cierre.

La búsqueda mediante un evento de texto con la consulta completa tiene un
ámbito distinto del tecleo físico carácter a carácter. En `timings`, 66
consultas dan mediana 385,717 ms y máximo 798,571 ms. El recorrido ALPHA físico
se valida aparte. Las aperturas históricas de esta campaña incluyen máximos
superiores a dos segundos para otros destinos; no se eliminan de los logs.

### Investigación y cierre del control de memoria

Las primeras fronteras HOME de la campaña mixta mantienen constantes memoria
interna (142.924 B libres, bloque mayor 98.292 B), LVGL (41.220 B libres, bloque
mayor 29.736 B), 71 objetos, cinco pantallas y tres temporizadores. La PSRAM
libre desciende ligeramente, incluso repitiendo la misma carga y locale.

Se añadió únicamente a la sonda privada `heap_caps_get_info` para distinguir
bloques vivos de variación en tamaños reservados. Su imagen tiene SHA-256
`a5c04c52dd3e38dea207ba1acd649cdce69fc009707ab80d1c41e93562adff56`;
`memory-probe-switch/` conserva escritura exclusiva de app, verificación y
lectura independiente, con prefijo de flash intacto.

Diez lotes de 50 cálculos escalares conservan 833 bloques vivos, aunque fluctúe
la ocupación reservada. Treinta observaciones sin acciones no cambian el heap.
En cambio, seis ciclos mixtos idénticos terminan con 824, 825, 826, 827, 828 y
829 bloques vivos, después de reemplazar las 50 entradas por escalares. No se
atribuye este incremento a fragmentación. Navegación, Settings sin cambios,
Toolbox, búsqueda/prefijos e inserción sin evaluar mantienen luego 829 bloques
en cinco repeticiones de cada recorrido. Se detuvo el cierre y se aislaron cálculo,
conversión, formatos e historial antes de continuar.

El aislamiento posterior deja constantes los bloques en tres repeticiones de
cantidad correcta, conversión, formato y recuperación del historial. Cada error
dimensional añade una reserva. La implementación fijada de `basic_string`
(GCC 8.4, `bits/basic_string.h:732`) conserva el buffer de destino al mover una
cadena SSO; `vector::erase` desplaza `HistoryEntry::result.error`. Así, una
entrada cuyo mensaje ya está vacío puede seguir siendo propietaria de memoria.
El modelo diagnóstico `history-string-retention.cpp` reproduce el movimiento
real y se estabiliza en 50 reservas (1.300 B solicitados, sin metadatos del heap)
durante 65 ciclos. No se cuenta ese modelo como aceptación física.

Tras llenar el historial físico con 50 errores y reemplazarlo por 50 escalares,
la frontera pasa a 873 bloques y permanece ahí durante quince repeticiones de
error más reemplazo completo por escalares (1.347 eventos en total). La PSRAM
libre oscila entre 8.176.451 y 8.176.711 B, con recuperaciones. Se confirma
retención acotada de capacidad del historial, no una pérdida continua en ese
reproducer. `error-retention-physical.json` y `memory-diagnosis.json` conservan
las fronteras y la identidad del header de libstdc++ inspeccionado.

La repetición final de dos calentamientos y quince ciclos mixtos pasa con las
reservas estabilizadas, sin reiniciar Giac: 873 bloques en las 17 fronteras
comparables, con 8.176.199–8.176.527 B PSRAM libres y recuperaciones entre
ciclos. Se mantienen 71 objetos, cinco pantallas y tres temporizadores en HOME.
El bloque mayor PSRAM es 8.126.452 B; memoria interna libre 143.160 B y bloque
mayor 98.292 B. `physical-cycles-stabilized.json` conserva las 17 fronteras,
los cuatro locales y 6.838 eventos; no se mezcla esta sonda con la de tiempos.
El producto no se modifica para forzar una cifra plana ni se fuerza agotamiento.
Se supera el control de pérdida sostenida para la carga ensayada, conservando
el diagnóstico inicial que parecía fallar.

### Límites de las muestras de recursos

En los 17 ciclos originales se observan mínimos de 142.924 B internos libres
(bloque mayor 98.292 B), 8.111.195 B PSRAM libres (bloque mayor 7.995.380 B)
y 55.684 B de pila todavía sin usar sobre la tarea de 65.536 B. El mínimo LVGL
muestreado por la sonda durante esa campaña es 30.428 B libres; bloque mayor
mínimo 29.736 B. Los snapshots de objetos van de 71 a 103, pantallas de cinco
a seis y temporizadores de tres a cuatro, volviendo a la frontera HOME descrita.
No se infiere un máximo absoluto del recorrido entre muestras. Los tiempos
native, el frame de 1.696 B y estas medidas físicas son evidencias distintas.

### Restauración y publicación local

Las dos altas de favoritos de prueba se retiran mediante Toolbox; la lista
vuelve a su estado inicial vacío. A–F se observan en cero, y se deja HOME / RAD /
Español de España. El brillo se conserva en 144. `physical-restoration.json`
registra la verificación previa a retirar la sonda; no se modifican directamente
archivos de datos del usuario.

La imagen ordinaria WROOM preparada y comprobada sin símbolos ni marcadores de
sonda tiene SHA-256
`5f3a30891114e5d8aa25e6a9a369c6fb884b76dcd71f7d1e8e962d84c0215499`.
La actualización exclusiva de aplicación pasa `verify_flash`, lectura
independiente completa y comparación SHA-256 idéntica; los 64 KiB previos a
la aplicación permanecen iguales. `ordinary-switch/operation.json` registra
`independently_verified`. Un RESET USB observado arranca el ordinario
(`ordinary-final-boot.log`, `[BOOT] OK`) y confirma brillo 144. El usuario confirma el smoke final
sobre esa imagen sin sonda: `2 m`, vista en cm, `Ans/(2 s) → 1 m/s` y vuelta
a HOME sin problemas. La respuesta literal se conserva en `human-review.json`.
Esta observación se distingue de las aserciones automáticas con sonda.

El ejecutable Windows ya está publicado mediante el mecanismo atómico validado,
con smoke, cuatro DLL y `previous-program.exe`; el lanzador habitual pasa su
prueba final (`launcher-closeout-final.log`, ALPHA físico y búsqueda). SHA-256 del ejecutable:
`34ad4c0d17a92e73c8813f189ca7be503b32527a917528cced5384f94ad8605a`.
Las identidades de runtimes y copia anterior están en `published-artifacts.json`.

`web-package-final/` conserva el paquete probado en los tres navegadores, sin
despliegue. Su metadata conserva el HEAD inicial con `dirty`, correspondiente
al momento de compilación; los manifiestos de fuentes identifican el candidato.
No se regeneran las 36 capturas previas: solo se añade
`alpha-visual/search.png`, original 320×240, del recorrido ALPHA corregido.
`essential/` y `essential-software-manifest.json` conservan las evidencias
necesarias fuera de cachés. Los backups de placa y fuentes tipográficas no se
distribuyen en ningún paquete.

### Commits locales y estado de Git

HEAD inicial: `15b9536ff848549c08f883ca960b1beeeb28bdb5`, rama `main`.
Después de los controles y de restaurar/comprobar el ordinario se crean:

1. `7a21767f3612aea0a13a676edc8b0f605f544b95` —
   `feat(toolbox): add typed unit catalog and regional locales`.
   114 archivos; árbol `6b24334220a69542cb73b9c0d3f3de0eb516e410`.
2. `05fe60ddde3814ca4fe07069b7f502a30405be6a` —
   `feat(calculation): evaluate quantities and convert output units`.
   Delta de 41 archivos; árbol `94e39abd5daa763ec708ed3c812f927470a076ec`.
3. El commit documental que contiene este informe y los seguimientos de
   `TOOLBOX_01.md` y `CALC_UNITS_01.md`. Su hash y HEAD final se registran después
   de crearlo en `out/toolbox-units-closeout-01/git-closeout-state.json`.

Antes de cada commit se revisan `diff --cached --check`, stat y diff completo,
con copias en `*-final-cached.*`. La selección usa rutas explícitas y los blobs
de los árboles validados; conserva en el working tree la capa posterior.
Se excluyen out/, binarios, galerías, backups, sonda y configuración ajena.

La revisión final detectó que el snapshot catálogo había perdido únicamente
el bit ejecutable de `scripts/emulator_sources.py`. Se preserva el modo 100755
de HEAD; no cambia un byte de fuente, objeto ni imagen. El árbol de cantidades
ya conservaba ese modo. `snapshot-mode-preservation.json` registra ambos árboles
anteriores y finales. No se repiten compilaciones por esa corrección de metadata.

Las fuentes de ejecución de ambos commits corresponden a sus snapshots probados;
la aceptación física pertenece al conjunto final, no al binario intermedio del
catálogo. El commit documental no modifica runtime, por lo que conserva las
identidades verificadas del firmware, emulador y paquete web.

El estado final conserva el índice y archivos versionados limpios. Queda fuera
de los commits `docs/RESUMEN_CHAT_TOOLBOX.png`, imagen preexistente preservada,
por la exclusión de galerías/binarios solicitada. El estado exacto y la secuencia
se guardan en `git-closeout-state.json` tras el último commit.
`.vscode/settings.json` permanece intacto (SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`).
No se hace push, despliegue web, cambio de particiones, borrado de almacenamiento,
firma generada ni reescritura de commits anteriores.

## Risks & Edge Cases

- Las capacidades diferidas siguen diferidas; no hay nuevas coerciones de cero,
  temperaturas afines, incertidumbre, tutor dimensional ni API headless de cantidades.
- Una pasada acumulada satisfactoria de WebKit no explica los dos cierres antiguos
  de UNIT-CATALOG-02. Se conservan sin causa demostrada.
- `parMixed` y Grapher con pool fijo mantienen su alcance histórico.
- La inyección de acciones demuestra rutas semánticas; no demuestra contactos,
  rebotes o que dedos humanos hayan probado todas las teclas.
- Los tiempos de operación no son primer píxel LCD. Las muestras de heap y pila
  incluyen la instrumentación indicada; no son máximos absolutos del producto.

## Alternatives Considered

Se utiliza el checkpoint verificable de catálogo como primera frontera y se
mantienen juntas sus capas compartidas. Separarlas en commits especulativos
habría creado estados sin evidencia. Se corrige el presupuesto local de filas,
sin añadir un segundo parser, cambiar límites o ampliar memoria. No se compacta
el catálogo ni se recortan idiomas para alojar la sonda.

BASE_TOOLBOX_UNITS_CERRADA: SÍ
