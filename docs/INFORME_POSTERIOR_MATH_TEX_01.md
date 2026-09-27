**Informe consolidado de trabajos posteriores al primer encargo del chat**

Fecha de consolidación: 27 de septiembre de 2026.

Este documento excluye la entrega inicial solicitada en el primer prompt,
MATH-TEX-01. Comienza con la observación posterior sobre la longitud del signo
menos y recoge todas las actuaciones siguientes: emulador, sesiones de rotura,
corchetes, recuperación del renderer, resultados, FORMAT/ENG, placa, placeholders
y tildes. Los arreglos previos de entrada se mencionan únicamente como trabajo
preservado y sometido a regresión, no como una entrega nueva de este informe.

Los hechos se han contrastado con los informes de cada entrega, sus manifiestos,
reproducciones y registros. Los estados «sin corregir» de informes históricos se
interpretan en su fecha: este informe indica cuándo se corrigieron después.
En esta consolidación no se ha vuelto a compilar ni a escribir la placa.

**Estado final documentado**

El menos matemático utiliza U+2212 de STIX. Los corchetes se introducen, conservan
la agrupación y escalan con glifos STIX. Se han reparado las cuatro familias de
viewport/recorrido y las cuatro familias de resultados confirmadas en las
sesiones de rotura. Calculation dispone de selección contextual de formatos,
alternancia exacto/decimal y ENG. El menú se rediseñó y sus tildes se corrigieron.
La entrada completamente vacía de Calculation queda sin caja; todas las ranuras
pendientes de plantillas siguen visibles, también después de EXE y en historial.

La última aplicación WROOM instalada durante estas actuaciones es la de la
corrección de tildes/entrada vacía, verificada por digest y con arranque correcto.
El emulador de Windows también está actualizado. Los paquetes web se prepararon
localmente; no se publicaron. El trabajo permanece sin commit.

**1. Corrección posterior del signo menos**

La comparación de `5^(-5-6)` reveló que se dibujaba el guion ASCII U+002D, no el
menos matemático U+2212. La diferencia era de glifo, no otro defecto de separación
entre átomos. Se eligió el U+2212 ya incluido en STIX, sin estirar el guion,
modificar bitmaps ni inventar grosor o tamaño.

| Tamaño nominal | Tinta anterior | Tinta del menos matemático | Avance anterior → actual |
|---|---:|---:|---:|
| 18 px | 5×3 px | 11×2 px | 6 → 13 px |
| 12 px | 4×1 px | 8×2 px | 4 → 9 px |
| 8 px | 3×1 px | 6×1 px | 3 → 6 px |

Medida y dibujo resuelven el mismo carácter, incluido el siguiente glifo al
calcular kerning. Esto comprende operadores y resultados numéricos negativos.
El operador semántico, AST y serialización siguen usando `-`; no se altera la
asociación, evaluación ni política vertical. No se añade un buffer de conversión.

`5^(-5-6)` pasa de 30 a 40 px de ancho y `5^(-5^(-6))`, de 28 a 36 px, por los
avances reales. Los ascensos/descensos se mantienen en las 36 fixtures. Se
verificó la correspondencia con la implementación de símbolos de KaTeX como
referencia host, sin incorporarlo al producto.

La batería independiente conserva 19 fallos detectores contra la versión previa
y pasa tras el cambio. La observación del carácter enviado realmente a LVGL
detectaba U+002D en 24 fixtures anteriores y deja de encontrarlo en el candidato.
Se ejecutaron regresiones de entrada, otras superficies, notación y web. La
regresión de espaciado reutilizada pasó 151539 comprobaciones; no se atribuye a
esta fase la creación del sistema de espaciado del primer encargo.

Evidencia: apéndice «Actualización: menos matemático» de
[MATH_TEX_01.md](MATH_TEX_01.md) y `out/math-tex-01/minus-followup/`.

**2. Puesta en funcionamiento del emulador de Windows**

El launcher no encontraba el ejecutable en las ubicaciones que consulta. Los
builds de trabajo estaban en una caché separada. Se colocó el ejecutable validado
en `C:/.piobuild/numOS/emulator_pc/program.exe`, junto con las DLL existentes:
`SDL2.dll`, `libgcc_s_seh-1.dll`, `libstdc++-6.dll` y `libwinpthread-1.dll`.

Se conservó un manifiesto con orígenes, destinos y hashes y una captura de
arranque en `out/emulator-launch-2026-09-26/`. Las entregas posteriores actualizan
ese mismo ejecutable. No se presenta esto como una reescritura del launcher ni
como una actualización de la PCB. La invocación sigue siendo:

```powershell
.\scripts\run-emulator-windows.ps1
```

El último ejecutable comprobado en esa ubicación tiene SHA-256
`51caddd27818b1588abd0b1644d457f810d56cc4299553b66685e9423ab27dd6`.

**3. Primera sesión de rotura: renderer y corchetes**

Se realizaron cuatro rondas con 84 casos distintos, introducidos por eventos
reales del editor. Se guardaron teclas, reproducciones `.numos`, árboles,
serialización, registros y capturas. Se probaron aritmética corriente, signos,
fracciones, raíces, scripts, filas largas, anidamiento, estados incompletos,
borrado, historial y desplazamiento. No hubo crashes/timeouts en las sesiones
completas, pero eso no se confundió con corrección visual.

| Hallazgo | Qué ocurría | Actuación inicial | Estado posterior |
|---|---|---|---|
| BR-01 | `[2+3]*4` perdía los corchetes y daba 14 | Se corrigió, como pediste | Da 20 y conserva `[]` en la entrada |
| BR-02 | Fórmula alta y cursor quedaban recortados; el cursor podía reducirse a una línea fuera del canvas | Solo se documentó | Corregido en Recovery 02 |
| BR-03 | Con 14 raíces se ocultaba el operando con `…`, aunque la fórmula cabía y se evaluaba | Solo se documentó | Corregido en Recovery 02 |
| BR-04 | Resultados exactos/periódicos anchos no se podían recorrer | Solo se documentó | Corregido en Recovery 02 |
| BR-05 | El desplazamiento podía superar el final del resultado y dejar el área blanca | Solo se documentó | Corregido en Recovery 02 |

Se respetó tu instrucción de no corregir todavía los demás hallazgos. Las
capturas finales de esa primera sesión conservan BR-02…BR-05 como reproducciones.

Los corchetes se admiten mediante SHIFT+paréntesis, teclado de escritorio `[`/`]`
y las rutas web correspondientes. Se reutiliza `NodeParen(DelimKind::Bracket)`.
Clonado e historial conservan su forma; Giac recibe agrupación escalar con
paréntesis para que no se interpreten como vectores.

El cierre sale del corchete ancestro más cercano, incluso desde un exponente o
fracción. Un cierre sin apertura no altera el árbol. No se borran placeholders
ni se hacen evaluables entradas incompletas.

Se extrajeron de STIX Two Math 2.12 b168a 13 variantes y tres piezas por lado en
18/12/8 px. Cuando no basta una variante, se ensamblan las piezas auténticas con
sus conectores. No son líneas dibujadas a mano ni bitmaps deformados. Los glifos
anteriores de paréntesis permanecieron idénticos. Medida, dibujo y cursor usan el
mismo plan de delimitador.

Se verificaron 1645 controles C++, 20 sesiones native y los recorridos web de
corchetes en Chromium, Firefox y WebKit, shell y componente. Los detectores que
esperan 20 fallan contra el ejecutable anterior y pasan después.

Evidencia: [RENDERER_BREAKING_01.md](RENDERER_BREAKING_01.md) y
`out/renderer-breaking-01/`.

**4. Reparación profunda del renderer y su viewport**

Tras tu autorización se corrigieron BR-02…BR-05. El viewport sigue el rectángulo
completo del cursor y dispone de desplazamiento vertical. En el reproducer el
cursor pasó de `y=235…235`, fuera del canvas, a `217…233`, con su altura completa.

Se eliminó el recorrido recursivo del dibujo y del antiguo Finder. Los nodos
guardan posiciones transitorias de presentación y se recorren mediante sus
enlaces padre/hijo. El cursor consulta el origen de su fila y el mismo
`childXOffset`. No se crea un vector por frame ni un nuevo límite silencioso de
profundidad. El layout recursivo previo sigue existiendo: no se afirma que haya
desaparecido toda recursión del motor.

Los resultados exactos, periódicos y extendidos se desplazan según su anchura
real. Se limitan ambos extremos y se recalculan al cambiar contenido o ventana.
Se comprobaron `2^200`, `1/97` y el recorrido excesivo de `1/7`. La entrada
evaluada también se puede recorrer con SHIFT+flechas. UP/DOWN sin SHIFT conserva
el historial. Más adelante, ENG añade su interacción específica con LEFT/RIGHT.

Siete detectores SDL fallan antes y pasan después. La suite de viewport pasa
2553 comprobaciones; se verifica coincidencia de posiciones, ida/vuelta,
redimensionado, scripts profundos y ausencia de asignaciones propias por frame.
Se repitieron las 84 sesiones anteriores, pruebas de otras apps y 900 trazas de
tutor/composición sin diferencias matemáticas. El recorrido gráfico host llegó
a 90 raíces: eso no significa que el serializador de producto deba evaluar 90.

Evidencia: [RENDERER_RECOVERY_02.md](RENDERER_RECOVERY_02.md) y
`out/renderer-recovery-02/`.

**5. Sesión separada de rotura de resultados**

Se investigaron 67 expresiones pequeñas en cuatro rondas, buscando respuestas
que el producto podía dar correctamente sin recurrir a overflow o límites
físicos. En esa fase no se cambiaron los resultados, tal como pediste.

Se confirmaron once ejemplos, agrupados en cuatro causas:

| Familia | Reproducciones | Fallo observado |
|---|---|---|
| Factorial | `5!`, `0!`, `3!+2` | La tecla no insertaba factorial; se obtenían 5, 0 y 5 |
| Ans complejo | `5 EXE`, `√(-1) EXE`, `Ans²` | Ans seguía siendo 5; salía 25 |
| Simplificación exacta | `27^(1/3)`, `2x+3x`, dos identidades trigonométricas | Se mostraban expresiones sin la reducción que Giac ya podía obtener |
| Dominio de logaritmos | `log₁(2)`, `log₁(0.5)`, `log₀(2)` | Se aceptaban infinito o cero con estado OK |

Se separaron las igualdades falsas de las expresiones equivalentes sin simplificar.
Tampoco se contaron como fallos nuevos ramas principales complejas, la política
existente de infinito, errores explícitos o un texto interno no reducido cuando
la pantalla sí mostraba la respuesta correcta. Los errores de alias del runner
se corrigieron en el runner, sin atribuirlos al producto.

Evidencia histórica: [CALCULATION_RESULTS_BREAKING_01.md](CALCULATION_RESULTS_BREAKING_01.md).
Sus once ejemplos se corrigieron en la entrega siguiente.

**6. Reparación de resultados, Ans y raíces**

| Caso | Comportamiento corregido |
|---|---|
| Factorial | Se inserta un operador postfix real: 120, 1 y 8 para los tres detectores |
| Factorial sobre resultado | Prepara `Ans!`, que se evalúa con EXE |
| Factorización prima | Se traslada al menú FORMAT, evitando confundirla con `!` |
| Ans después de `i` | `Ans²` da −1, no el cuadrado del número anterior |
| Simplificación | Se obtienen 3, `5x`, 1 y 1 en los cuatro detectores |
| Logaritmos inválidos | Bases 0 y 1 se rechazan como indefinidas antes de perder el dominio original |

Giac conserva la autoridad de evaluación. La simplificación no se introduce en
layout y evita cancelar denominadores simbólicos perdiendo restricciones. Por
ejemplo, no se fuerza la cancelación de `(x²−1)/(x−1)` ignorando `x≠1`.

Los decimales escritos se entregan para evaluación exacta como razones de
enteros, sin intentar reconstruir una fracción a partir de un `double` ya
redondeado. Se conserva la pareja de árboles tipados exacto/aproximado.

Ans, PreAns y A-F pueden mantener en sesión el resultado exacto complejo o
simbólico. No se inventa un escalar sustituto ni se cambia el formato persistente:
los valores que no admite `ExactVal` no adquieren persistencia nueva. x/y/z
conservan su papel de símbolos libres.

Tus capturas de raíces revelaron dos problemas adicionales. El índice del
radical se elevaba usando em en vez de la altura del radical y no reservaba bien
su anchura. Se guardaron origen, baseline y reserva del grado para compartirlos
entre medida, dibujo y cursor. Se corrigió también el extremo inclusivo de la
barra. El radical sigue siendo el vectorial existente; no se afirma haberlo
convertido en un radical ensamblado STIX.

La serialización de raíces anidadas contaba filas envolventes y emitía paréntesis
redundantes. Se distingue ahora profundidad semántica de frames de recorrido.
Se conservan presupuestos de 400 nodos, 2000 bytes y profundidad semántica 40,
con guardia de 82 frames. En `√…√2`, 39 raíces evalúan; 40/41 se rechazan por
límite de expresión, sin presentarlas como un error sintáctico inexplicable.

Los once detectores matemáticos fallan contra el baseline y pasan después.
Además se ejecutaron 38 oráculos numéricos independientes, controles de dominio,
STO A complejo, PreAns y 911 comprobaciones de geometría/formato.

Evidencia: [CALCULATION_FORMAT_01.md](CALCULATION_FORMAT_01.md) y
`out/calculation-format-01/`.

**7. Nuevo comportamiento de FORMAT y ENG**

FORMAT corto alterna exacto/decimal. Desde un formato secundario recupera el
exacto. Cambiar la presentación no reconstruye el valor canónico desde los
dígitos pintados ni altera Ans. Un decimal como `0.1` puede presentarse por
defecto como `1/10`; `2π` conserva su forma exacta y alterna con su aproximación.

Se implementó el acceso SHIFT → ALPHA → FORMAT con resultado válido. No es
necesario mantener las teclas pulsadas. SHIFT+FORMAT conserva TABLE. No se
implementó apertura por pulsación larga ni se añadió un acceso desde TOOLBOX.

El menú se rediseñó tras tu valoración: panel claro, foco azul, filas alineadas,
marca del formato activo, contador, desplazamiento y hasta cinco filas visibles,
con altura adaptada al número de opciones. Flechas seleccionan, EXE aplica,
BACK cierra; las superficies con puntero permiten elegir una fila.

| Formato | Disponibilidad y comportamiento |
|---|---|
| Exacto / normal | Fracción simplificada, constantes, radicales o expresión canónica, según resultado |
| Decimal | Aproximación tipada disponible del motor |
| Periódico / extendido | Cuando lo admite el resultado y su representación |
| Mixta | Fracciones impropias compatibles; el signo se aplica al número completo |
| SCI | Notación científica con la precisión disponible del compañero numérico |
| ENG | Exponente múltiplo de tres, también para valores pequeños y negativos |
| FIX | Selección de 0…9 decimales, conservando el valor original |
| Factores primos | Enteros de magnitud 2…10¹², con coste acotado |
| Cartesiana | Forma `a+bi` de complejos |
| Polar/fasor | `r∠θ` para complejos numéricos |
| Exponencial | `r·e^(iθ)` para complejos numéricos; fase en radianes |
| RAD/DEG/GRA | Resultado con procedencia angular reconocida o fase polar |

Las opciones se calculan según el resultado. `1/2` no ofrece RAD/DEG/GRA ni
polar; `x+i` no se trata como un complejo numérico plenamente determinado.
`sin(...)` produce una razón y una función hiperbólica no se considera angular
por su nombre. La procedencia de ángulo se reconoce conservadoramente desde el
AST, no por el valor numérico. Se conserva en historial, pero no constituye un
sistema general de unidades ni se propaga a través de Ans/A-F.

FIX redondea de forma decimal determinista, con empates alejándose de cero y sin
cero negativo: `9.995 → 10.00`, `−0.005 → −0.01`, `−0.004 → 0.00`. La expansión
se limita a 256 dígitos enteros. SCI no tiene todavía selector independiente de
cifras significativas.

ENG corresponde a SHIFT+`×10^x` sobre un resultado. `1234 EXE` pasa a
`1.234 × 10³`; repetir ENG muestra `1234 × 10⁰`. LEFT revierte el desplazamiento,
RIGHT continúa; los pasos adicionales están limitados a seis grupos por sentido.
FORMAT recupera el exacto. La tecla `×10^x` sin SHIFT conserva su plantilla de
entrada con base 10 real. La mantisa/exponente se construye con dígitos, sin una
reconversión innecesaria a `double`.

Las conversiones angulares de salida no cambian el modo global de entrada.
Para conservar fases exactas como `π/4` incluso con entrada DEG se reutiliza una
llamada protegida de Giac en radianes que restaura el contexto al salir.

**8. Comprobación e instalación en la placa**

Al comunicar que ENG y S+A no funcionaban se identificó la WROOM de producción,
USB `303A:1001`, serie `44:B1:76:A7:B7:2C`, COM9. La lectura de su aplicación
confirmó que todavía llevaba el firmware anterior a FORMAT. Los primeros builds
WROOM/CAM no se habían flasheado y no debían confundirse con el emulador.

Se conservó una copia completa de los 16 MiB de flash y la aplicación previa.
La copia del dispositivo queda local: no se distribuye en los ZIP visuales.
Se trabajó sobre OTA0, offset `0x10000`, capacidad `0x640000`, manteniendo
bootloader, particiones, configuración, modo/frecuencia de flash y seguridad.

Una imagen privada de diagnóstico ejecutó en el ESP32 440 eventos con
coordenadas del mapa de producción y pasó 22 puntos de control: ENG, menú,
selección, vuelta al exacto, BACK, capacidades de `1/2`, entradas incompletas,
recuperación, valores pequeños/negativos/cero y 50 ciclos del menú. La memoria
LVGL medida en los ciclos 1, 10 y 50 se mantiene en `free=38164`,
`largest=31960`, `peak=29532`. El pool configurado sigue siendo de 64 KiB.

Esos eventos recorren la resolución de planos y el despachador reales del
firmware. No son pulsaciones humanas ni verifican contactos eléctricos o el
debounce del escáner físico. Los errores de sincronización del puente serie al
preparar el runner se corrigieron en el runner, no cambiando el producto.

Se detectó y reparó además que BACK en el firmware ordinario salía de Calculation
en vez de cerrar su menú. Ahora cierra primero FORMAT o el visor de pasos.
Después se retiró la instrumentación y se instaló la imagen ordinaria, verificada
por digest y arranque. La corrección posterior de tildes se instaló de la misma
forma y conservó el idioma actual.

Última instalación documentada:

- Imagen ordinaria WROOM: 5707408 bytes.
- SHA-256: `251ded7b3264b89b674c5a0495f09a155fa07816610d607873c9296bf39d95c1`.
- Registro: `out/format-polish-02/board/identity.json`, estado `verified_booted`.
- Aplicación anterior inmediata recuperable: `78f954531a58e1d25fb01082c92a62889ec39d31edea59d3afef80d927423fbb`.
- El prefijo de flash de 64 KiB es idéntico antes/después de cada escritura.

**9. Placeholders: restauración global y excepción final**

Se detectó que `drawEmptyBaseline` ocultaba los placeholders en vistas sin cursor
y en una entrada formada por un único vacío. A tu petición se restauraron en
todos los canvases. El primer detector pasó de 21 fallos sobre 109 comprobaciones
a cero, incluyendo píxeles de los cuatro bordes y validación de incompletos.

Después aclaraste que Calculation completamente vacío debía verse limpio. Se
añadió una opción del canvas que Calculation desactiva únicamente para ese caso:
un `Empty` como único hijo directo de la raíz. El nodo sigue existiendo, con su
medida y cursor; solamente no se pinta su casilla.

El estado final conserva las casillas en bases y exponentes vacíos, fracciones,
raíces, plantillas anidadas, vistas de solo lectura e historial. No se ocultan
porque el cursor esté en otra ranura ni se convierten en expresiones evaluables.
La suite ampliada pasa 217 comprobaciones en TEXT/SCRIPT/SCRIPTSCRIPT, con y sin
cursor y con ambas políticas de raíz.

**10. Tildes del menú español**

Los textos eran correctos, pero las filas del menú utilizaban Montserrat sin el
complemento español existente. Por eso aparecían cajas en «Científica»,
«Ingeniería», «periódico» y «Fracción».

Las filas pasan a `ui::tutorFont14()`, ya utilizado en el producto, con los glifos
españoles de respaldo. No se cambia el tamaño, el kerning ASCII, las traducciones
ni STIX. La prueba consulta la fuente real de cada etiqueta y comprueba que el
descriptor del glifo no sea un placeholder.

Se reprodujeron ambos defectos —caja inicial y tildes— contra el ejecutable
conservado. Después pasan EN/ES, con 297 observaciones de caracteres del menú
por idioma. Los tres estados comparados del menú inglés conservan exactamente
los píxeles del área de contenido. Se revisaron las capturas ampliadas.

Evidencia: [FORMAT_PRESENTATION_02.md](FORMAT_PRESENTATION_02.md) y
`out/format-polish-02/`.

**11. Validación acumulada, sin sumar ejecuciones repetidas**

Las cifras siguientes identifican baterías de las fases indicadas. No son una
suma de casos únicos ni una afirmación de que todas se repitieron sobre cada
binario sucesivo.

| Área | Evidencia relevante |
|---|---|
| Estrés visual | 84 casos en cuatro rondas, después reproducidos tras Recovery |
| Capacidades/resultados | 67 sesiones; 11 detectores corregidos; 38 oráculos numéricos |
| Entrada | 113 controles rápidos; variantes de 114 incluyen el ciclo de vida adicional |
| Menos / regresión de espaciado | 19 detectores previos; 151539 comprobaciones reutilizadas de regresión |
| Corchetes | 1645 comprobaciones; 17 fixtures instrumentadas; 20 sesiones native por variante |
| Viewport | 2553 comprobaciones y siete detectores que fallan antes/pasan después |
| FORMAT | 23 sesiones; 911 comprobaciones geométricas/de formato |
| Placeholders finales | 217 comprobaciones, más reproducciones de producto EN/ES |
| Tutor/composición | 900 trazas matemáticamente invariantes en Recovery y FORMAT |
| Idiomas | 1593 comprobaciones y catálogo de 247 mensajes EN/ES en las regresiones amplias |
| Asignaciones | En FORMAT: 1280 fallos de asignación de tutor y 320 de resultados periódicos |
| Pool fijo | 50 ciclos de entrada y 50 de menú en host; posteriormente 50 de menú en ESP32 |
| Otras superficies | Equations, Calculus y Grapher; Steps EN/ES; notación/STIX |
| Builds | Native, native con pool fijo en las fases amplias, WROOM, CAM y Wasm |
| Navegadores | Chromium, Firefox y WebKit, shell y componente, con las salvedades siguientes |

No se han promocionado goldens ni ampliado máscaras para convertir fallos en
PASS. Se guardan originales de 320×240 y ampliaciones enteras nearest-neighbor.
Las capturas native no se presentan como fotografías ni revisión humana del LCD.

**12. Recursos y límites medidos**

| Fase | Coste incremental principal frente a su propio baseline |
|---|---|
| Menos U+2212 | +56 B de flash text/rodata; RAM estática e IRAM iguales |
| Corchetes STIX | +8584 B de flash; RAM estática e IRAM iguales |
| Recovery | −2032 B de flash WROOM; +12 B por nodo y +4 B por canvas |
| Resultados/FORMAT | +13012 B text y +2756 B rodata WROOM; +40 B DRAM estática |
| Placa/BACK/casillas | +168 B text y +28 B rodata WROOM; RAM estática e IRAM iguales |
| Entrada vacía/tildes | +32 B en el binario WROOM; RAM estática e IRAM iguales |

Son deltas sucesivos de naturaleza distinta —secciones frente a tamaño de imagen—,
por lo que no se presentan como un único total sumado sin matices.

En Xtensa, Recovery deja `MathNode` en 44 B, `NodeRow` en 56 B y `MathCanvas`
en 184 B. FORMAT deja `NodeRoot` en 72 B, `CalculationApp` en 840 B y
`VariableManager` en 840 B. La opción final de raíz vacía ocupa padding: no
aumenta `MathCanvas` ni `CalculationApp`. La RAM estática final WROOM es 119200 B.

No se añaden asignaciones propias por frame en layout/draw/cursor. Sí se crean
objetos LVGL al abrir el menú y árboles/cadenas al convertir resultados fuera de
esas rutas. Retener exacto y aproximado puede aumentar el uso dinámico del
historial; no se afirma heap total constante. Los frames propios medidos no son
el pico total de pila. No se ha medido una latencia física general del producto.

**13. Pendientes y límites que no se deben dar por resueltos**

- No se afirma paridad TeX/OpenType completa. Persisten la densidad visual de
  scripts extremos y otras políticas verticales ajenas a los arreglos concretos.
- El radical conserva su dibujo vectorial; los corchetes sí usan glifos STIX.
- Sigue documentada la discrepancia histórica del detector de paréntesis
  `parMixed`; las imágenes antes/después no cambiaron y no se falseó su resultado.
- Los presupuestos del editor/serializador siguen acotados; no hay anidamiento
  ni longitud de evaluación ilimitados.
- No se ha añadido persistencia nueva para complejos/simbólicos, un sistema
  general de unidades, GRA global de entrada, DMS, prefijos SI ni selector
  independiente de cifras SCI.
- La precisión y el rango aproximado siguen siendo los del Giac actual.
- En la última comprobación, el harness rápido de WebKit capturó un menú aún
  abierto tras un clic y, en otro intento, una pantalla pendiente de refresco.
  La repetición privada con 200 ms entre grupos de teclas y 480 ms después del
  clic pasó shell y componente sin cambiar comparaciones de píxeles. Queda por
  robustecer la sincronización de ese harness; la ejecución rápida no se declara
  aprobada. Chromium y Firefox pasaron la ejecución ordinaria.
- Las pruebas de placa con eventos inyectados no equivalen a ensayar contactos
  físicos o a revisar el LCD personalmente.
- No se ha publicado la web. No se han hecho commits ni push.

**14. Entregables y trazabilidad**

Cada fase conserva su snapshot anterior, diffs, ejecutables/paquetes pertinentes,
reproducciones, pruebas y candidatos visuales. Los seis manifiestos de código
posteriores al primer encargo contienen 8, 24, 15, 33, 4 y 7 entradas,
respectivamente; por el solapamiento entre fases son **66 archivos distintos**.
Eso no significa 66 archivos creados desde cero ni incluye los binarios copiados
para preparar el emulador. Este informe consolidado es un documento adicional.

El inventario exacto, con fases y hashes actuales, se guarda en
`out/informe-posterior-math-tex-01/manifest.json`. Cuando un archivo también
pertenecía al primer encargo —por ejemplo `MathRenderer.cpp` o `MATH_TEX_01.md`—
solo se atribuye aquí su modificación posterior, no todo su contenido.

| Entrega | Galería o evidencia principal | ZIP visual |
|---|---|---|
| Menos matemático | `out/math-tex-01/minus-followup/visual/index.html` | `out/math-tex-01/minus-followup/MATH-TEX-01-visual.zip` |
| Arranque del emulador | `out/emulator-launch-2026-09-26/manifest.json`, `startup.ppm` | No corresponde |
| Breaking/corchetes | `out/renderer-breaking-01/visual/index.html` | `out/renderer-breaking-01/renderer-breaking-01-visual.zip` |
| Recovery/resultados investigados | `out/renderer-recovery-02/visual/index.html` | `out/renderer-recovery-02/renderer-recovery-02-visual.zip` |
| FORMAT/resultados corregidos | `out/calculation-format-01/visual/index.html` | `out/calculation-format-01/calculation-format-01-visual.zip` |
| Placa/placeholders | `out/format-board-01/visual/index.html` | `out/format-board-01/format-board-visual.zip` |
| Tildes/excepción de entrada vacía | `out/format-polish-02/visual/index.html` | `out/format-polish-02/format-polish-02-visual.zip` |

Los ZIP visuales incluyen sus recursos relativos comprobados y no distribuyen
archivos de fuentes. Las copias completas de flash permanecen locales.

El repositorio continúa en `main`, HEAD
`ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, con índice vacío y cambios en el working
tree. No se hizo stash, reset, clean, staging, commit ni push. El trabajo previo
se conserva. `.vscode/settings.json` mantiene SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.
