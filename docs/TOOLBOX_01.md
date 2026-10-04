# TOOLBOX-01 — catálogo, búsqueda e inserción

Toolbox compartida con 140 entradas visibles y 152 identidades estables, con favoritos persistentes,
recientes de sesión y búsqueda EN/ES. Se abre en los editores compatibles de
Calculation, Equations, Calculus y Grapher. El inventario reproducible está en
[TOOLBOX_01_CATALOG.md](TOOLBOX_01_CATALOG.md), generado desde
`src/math/toolbox-catalog.json` junto con `ToolboxEntries.inc`.

## Infinito y disponibilidad del emulador — 2 de octubre de 2026

En Variables → Special Characters, EXE sobre **Infinity** inserta **∞** sin signo
escrito; conserva el significado positivo. DERECHA abre tres variantes: **+∞**,
**−∞** y **±∞**. Las identidades `410:0` a `410:3` distinguen cada forma en búsqueda,
favoritos y recientes. La lista principal mantiene sus nueve caracteres especiales.

El signo negativo se serializa como un átomo protegido `(-infinity)`: se verifican
producto implícito y potencia. Esta última envuelve el átomo con signo en un
`NodeParen`, preparado antes de mover la base: muestra `(−∞)²` y conserva el editor
si falla una reserva. **±∞ representa ambos valores**, serializados como
`[-infinity,infinity]`; el resultado es una lista, no un escalar. La ayuda EN/ES
explica este significado. El infinito positivo sin signo tiene un tipo propio y
no se confunde con el infinito complejo sin dirección de Giac. No se añaden campos
al AST ni reservas en layout/draw/cursor. Se mantienen las capacidades: infinito
está disponible en Calculation; no se amplía el contrato de los otros editores.

La versión publicada reside permanentemente en
`C:\.piobuild\numOS\emulator_pc\program.exe`, con sus DLL. PlatformIO compila
ahora por defecto en `C:\.piobuild\numOS-build`. El lanzador prioriza la versión
publicada. `scripts/publish-emulator-windows.ps1` valida candidatos en una carpeta
separada, publica mediante sustitución atómica y conserva el ejecutable anterior;
si está en uso, lo mantiene. La regla está en el AGENTS.md local y en las
instrucciones globales de este workspace. Los ensayos de publicación, bloqueo,
fallo y limpieza aislada están en `out/emulator-availability-01/`.

El resumen de toda la conversación está en [RESUMEN_CHAT_TOOLBOX.md](RESUMEN_CHAT_TOOLBOX.md).
Las nuevas pruebas y capturas están en `out/toolbox-infinity-01/`.
Pasan 207 recorridos nativos y, tras la agrupación de potencias, 16 recorridos
adicionales de infinito, 21.422 checks host de catálogo/persistencia, las pruebas
de editor y resultados, rasterizado en tres tamaños y 1.645 checks de corchetes.
La pasada web final cubre Chromium/Firefox/WebKit en shell y componente. Compilan
native, WebAssembly, WROOM y CAM; el candidato final está publicado y probado con
el lanzador habitual. RAM estática e IRAM permanecen iguales a la revisión anterior.
El perfil de los ELF finales está en
`out/toolbox-infinity-01/firmware/resources-firmware.json`.
Los apartados siguientes documentan las revisiones anteriores y sus mediciones.

## Variables, glifos y correcciones de navegación — 30 de septiembre de 2026

Estado actual: seis categorías, con **Variables → Latin Alphabet / Greek Alphabet /
Special Characters** (nombres localizados en español). Latín contiene las 26 letras
a-z; griego las 24 letras alfa-omega. **EXE inserta la minúscula; DERECHA abre las
dos variantes con la mayúscula seleccionada, y EXE la inserta**. La variante también
se puede buscar y guardar como favorita. Se eligió este recorrido porque muestra
la decisión y coincide con las variantes de logaritmo; no depende de un SHIFT
oculto. El estado previo del teclado sigue restaurándose al cancelar.

Caracteres especiales reúne π, e, i, infinito y las formas ϑ, ϕ, ϵ, ϖ, ς. Las letras
e/i/I/π permanecen variables libres, distintas de las constantes de Giac: se usa
una correspondencia cerrada con identificadores matemáticos Unicode que conserva
su identidad a través de cálculo, Ans e historial. A-F mantienen su significado
previo de memorias STO. El serializador solo admite las letras griegas enumeradas,
no texto CAS arbitrario. Las capacidades de Grapher siguen limitadas a los símbolos
que entiende su editor.

Las pestañas usan relleno y subrayado para que el foco quede completo; los
indicadores de submenú son trazos de 2 px y las ayudas usan texto de 12 px. El
mensaje de búsqueda vacía queda debajo de la consulta. Esta también tiene fallback
STIX para consultas griegas, incluidas Δ y las variantes. Las constantes usan tinta
negra. Se reutilizan las fuentes existentes sin generar o añadir assets de fuente.

`MathCanvas` mide el contorno real de los átomos mediante un callback acotado sin
asignaciones: incluye descendentes y compensación izquierda, compartida con el
dibujo. Las correcciones itálicas de potencias alcanzan también los símbolos
griegos y las bases envueltas en una fila. El descenso por filas es iterativo y
limitado a 64; la tabla TeX de espaciado y las reglas de delimitadores no cambian.
El caso de Delta conserva su fuente y métricas dedicadas. La comprobación de
espaciado se adaptó únicamente donde suponía que la caja de x era su avance: STIX
pinta un píxel más allá, ahora incluido en la caja mediante un oráculo de fuente.

[Galería actual](../out/toolbox-variables-01/visual/index.html) ·
[ZIP con 45 capturas](../out/toolbox-variables-01/TOOLBOX_01_visual.zip) ·
[Manifiesto](../out/toolbox-variables-01/delivery-manifest.json).
La copia previa de fuentes, parche y ejecutable está en
`out/toolbox-variables-01/baseline/`. El lanzador habitual usa el binario nuevo:
**F6 abre Toolbox; F5 abre opciones; F8 vuelve**.

Validación de esta revisión:

- 184 recorridos nativos, con las 137 entradas visibles insertadas y evaluadas;
  13 comprobaciones finales de alfabetos/consultas tras añadir el fallback griego,
  y un caso adicional de beta en fracción y potencia.
- 279 recorridos de capacidades: 136 Equations, 136 Calculus y 7 Grapher.
- 20.708 controles host de catálogo/estructura/persistencia, incluyendo 620 puntos
  de fallo de asignación; 17 escenarios de fallo recuperable en UI.
- Cobertura de todos los glifos griegos y variantes en los tres tamaños STIX;
  comprobación de píxeles, descendentes, ausencia de azul y layout sin heap.
- 151.538 comprobaciones de espaciado y 2.553 de viewport, sin fallos; 36 fixtures
  de composición. Pasan también editor/serialización y capacidades de resultados,
  los 113 recorridos de Calculation y 20 de corchetes, más 1.645 checks de corchetes.
- 50 ciclos con historial largo y 50 ciclos HOME después de cuatro muestras de
  calentamiento, sin crecimiento acumulado, con el pool de 65.536 B. Mínimos
  observados: 5.872 B libres, mayor bloque 5.160 B. La última pasada incluye el
  fallback estático de la consulta griega.
- Chromium, Firefox y WebKit, tanto shell como componente: búsqueda vacía,
  alfabetos, mayúsculas, Δ², puntero y favoritos persistentes tras recarga IDBFS.
- Lanzador normal probado con inserción y cálculo de Δ². No se realizó flash ni
  validación física del LCD. Las medidas host no son latencia de ESP32.

El catálogo permanece en flash. Se conservan 28 widgets, cuatro previews y el
canvas que toma prestada la expresión. `Session` host: 2.080 B; preview máximo:
115 × 36 px. Las reservas de AST ocurren al insertar/refrescar, fuera de
layout/draw/cursor. El callback de medidas usa un buffer fijo de 96 B más su
estado local; el máximo de pila de tarea en placa no se ha medido. La apertura
medida en host añade 3.360 B de C++/AST (+120 B respecto al popup previo),
excluyendo el allocator C de LVGL y Giac; los ciclos tras cierre no acumulan heap.

Perfil Xtensa obtenido del ELF final: `Session` 1.380 B (+60 B), `Entry` 44 B
(en tabla de flash). Marcos propios: `measureMathAtom` 208 B, `layoutTextAtom`
48 B, `preview` 400 B, `refresh` 320 B y `open` 160 B. Son marcos individuales,
no el máximo de toda la cadena de llamadas. Se verifica la alineación de las
cajas con píxeles en host; el LCD físico queda pendiente.

| Perfil final | Imagen | Flash enlazada | RAM estática | Texto IRAM |
|---|---:|---:|---:|---:|
| WROOM | 5.750.176 B | 5.749.813 B | 119.544 B | 60.407 B |
| CAM | 5.662.496 B | 5.662.137 B | 118.136 B | 59.039 B |

La RAM estática aumenta 136 B frente al popup anterior por los descriptores de
fallback; IRAM conserva su tamaño. Las imágenes, hashes, tipos, secciones y
marcos están en el [perfil final](../out/toolbox-variables-01/firmware/resources-firmware.json).
Ambos perfiles compilan con las fuentes finales. La última reconstrucción se
hizo conservando la caché: PlatformIO invalida todo el directorio al aparecer
una cabecera nueva; no se usan las imágenes anteriores como evidencia final.

Los apartados posteriores son históricos; sus cifras no describen esta revisión.

## Revisión popup anterior — 30 de septiembre de 2026

La revisión responde a la petición de un menú más compacto y menos repetitivo.
El popup deja la expresión actual arriba, incorpora pestañas, tres funciones
destacadas y cinco categorías. Sustituye casillas de ejemplo por argumentos
matemáticos legibles y oculta 14 identidades con tecla directa, conservándolas
para favoritos anteriores. La corrección de pivote del renderer elimina las
barras desplazadas de valor absoluto; no es un icono o bitmap de sustitución.

La copia de seguridad de los 39 archivos previos, parche y ejecutable está en
`out/toolbox-popup-01/baseline/`. Se conservan la rama, índice y trabajo previo.
El ejecutable nuevo está en `C:/.piobuild/numOS/emulator_pc/program.exe`, probado
mediante `scripts/run-emulator-windows.ps1`. **F6 abre Toolbox**, F5 da opciones,
F8 vuelve. Escribir en el teclado de Windows abre la búsqueda; en el teclado
físico se entra en la pestaña Buscar y se utiliza ALPHA.

[Galería anterior](../out/toolbox-popup-01/visual/index.html) y
[ZIP visual anterior](../out/toolbox-popup-01/TOOLBOX_01_visual.zip): 31 capturas
320 × 240 y ampliaciones 4×, inspeccionadas. Incluyen el caso de expresión alta:
la banda superior sigue la posición de edición cuando la fórmula no cabe.
[Manifiesto de esta revisión](../out/toolbox-popup-01/delivery-manifest.json).

Validación de la revisión popup anterior:

- 64 recorridos de Toolbox más uno de popup sobre expresión alta; 29 fixtures
  visibles insertadas, completadas y evaluadas, con geometría de previews.
- 61 recorridos de capacidades: 29 Equations, 29 Calculus y 3 Grapher.
- 3.843 controles host de catálogo, estructura y persistencia, con 302 puntos
  de fallo de asignación; 17 escenarios de fallo recuperable de UI.
- 11.897 controles de fuentes y previews, incluidos píxeles de valor absoluto
  en tres estilos. El mismo detector falla contra el renderer de HEAD.
- 113 recorridos de Calculation, 20 de corchetes y 1.645 controles host de
  corchetes. Los controles de editor/serializador y capacidades de resultados
  también pasan.
- 50 ciclos con historial largo y 50 ciclos HOME después de cuatro muestras
  de calentamiento conservadas: sin crecimiento acumulado por recorrido,
  con el pool original de 65.536 B.
- Chromium, Firefox y WebKit, cada uno en shell y componente: teclado, texto,
  inserción por puntero, cierre al pulsar fuera y favoritos tras recarga IDBFS.
- Builds native, web Release, WROOM ordinario y CAM ordinario. No se repitieron
  los otros cinco perfiles de firmware ni el corpus completo de la entrega
  original. No se realizó flash ni validación del LCD físico.

La sesión usa 28 widgets, cuatro previews y un canvas de contexto que toma
prestado el AST vivo. `Session`: 1.960 B host / 1.320 B Xtensa (+220 B Xtensa
respecto al menú anterior); `Entry`: 36 B Xtensa en el catálogo inmutable.
La caja máxima de preview es 115 × 35 px, con padding del canvas reservado.
La construcción de previews ocurre al refrescar la lista, fuera de los caminos
calientes de layout/draw/cursor. El recorrido nuevo está limitado por un array
de 82 punteros; no incorpora recursión propia.

Perfil medido: marco propio `preview` 400 B, `refresh` 320 B y `open` 144 B.
Son marcos individuales; el máximo de pila de tarea en placa sigue sin medir.
El muestreo host de apertura añade 3.240 B C++/AST (excluye el allocator C de
LVGL y Giac). En los ciclos del pool se observaron mínimos de 6.056 B libres
y 5.688 B en el mayor bloque. Son muestras, no límites universales. No se
introducen asignaciones en el renderer ni se altera el pool de producción.

| Perfil local | Imagen | Flash enlazada | RAM estática | Texto IRAM |
|---|---:|---:|---:|---:|
| WROOM popup | 5.731.136 B | 5.730.765 B | 119.408 B | 60.407 B |
| CAM popup | 5.643.376 B | 5.643.009 B | 118.000 B | 59.039 B |

Frente a la primera Toolbox: WROOM +2.272 B de imagen y flash enlazada;
RAM estática e IRAM sin aumento. Los hashes y secciones están en
[perfil de firmware](../out/toolbox-popup-01/firmware/resources-firmware.json).
La limitación histórica de Grapher con pool fijo se mantiene; sus recorridos
se acreditan en CLIB. Las capacidades matemáticas de Grapher no se ampliaron.

## Identidad y preservación

Baseline inspeccionado: rama `main`, HEAD
`15b9536ff848549c08f883ca960b1beeeb28bdb5`. Referencias de código anteriores:
`0f48c8eb5b9d8e49a7d195918daf14c4e9fdb3d7` y
`2c79d3df81097bd3504463cb18e9e471ae5b466f`.
Índice y árbol iniciales estaban limpios. Se conservaron estado, inventario de
worktrees y parches vacíos en `out/toolbox-01/baseline/`. No se modificaron los
worktrees históricos. La puerta rápida de Calculation pasó antes de editar.

La referencia WROOM anterior era **una compilación local**, no la última imagen
instalada: imagen 5.707.440 B; flash enlazada 5.707.077 B; RAM estática 119.200 B;
IRAM texto 60.407 B. Los nuevos binarios son candidatos locales. No se hizo
flash, publicación, despliegue, staging, commit, push, stash, reset ni clean.
Tampoco se cambiaron dependencias, particiones, clocks, pool LVGL, códigos de
tecla o mapa eléctrico. `.vscode/settings.json` conserva SHA-256
`00b9475d436d8c7bfa5b998dec3de2a8fe0c1caef7092597883599d60e395fd7`.

## Auditoría de accesos

No existía un navegador matemático compartido detrás de TOOLBOX. Había acciones
contextuales de las aplicaciones, que siguen siendo independientes.

| Aplicación / estado | Antes | Ahora |
|---|---|---|
| Calculation, editor o resultado con editor vivo | Sin catálogo compartido | Toolbox sobre la expresión y cursor actuales |
| Calculation, FORMAT / Steps activos | Modal propio | El modal existente conserva el evento |
| Equations, EDITING | Sin catálogo compartido | Toolbox, capacidad Equations |
| Equations, resultado | Steps | Steps |
| Calculus, EDITING | Sin catálogo compartido | Toolbox, capacidad Calculus |
| Calculus, resultado | Steps | Steps |
| Grapher, Expressions / CONTENT / EDITING | Sin catálogo compartido | Toolbox, capacidad Grapher |
| Grapher, Templates, Calculate y otros estados | Acciones propias | Acciones propias |
| VAR / STO, FORMAT corto, SHIFT+FORMAT, SHIFT→ALPHA→FORMAT, SHIFT+EXP | Contratos existentes | Sin cambios fuera de Toolbox |

La tecla web rotulada TOOLS enviaba el antiguo F1. Se corrigió únicamente esa
asociación a TOOLBOX (72); no se modificaron los límites de la API de teclas.

## Arquitectura y transacción

`ToolboxCatalog` contiene datos inmutables y recetas tipadas. `Identity` guarda
ID y variante de 16 bits; ni posiciones, nombres traducidos ni punteros de
widgets son identidades persistentes. `ToolboxStore` mantiene orden explícito y
el registro de favoritos. `ui::toolbox` mantiene ruta, foco, consulta, ventanas
de filas y un receptor con propietario, cursor, capacidades y callback.

`prepare()` construye un AST y su ranura inicial, sin evaluar. La receta alimenta la inserción con ranuras vacías. `preview()` crea un árbol
independiente con argumentos ilustrativos x, n, a, b o z; nunca rellena el AST
del usuario ni evalúa la ilustración. `CursorController::insertPrepared()`
verifica que la ranura pertenece al árbol preparado, construye la multiplicación
explícita cuando hace falta y reserva la fila antes de publicar. La publicación
no asigna memoria. El cursor termina en el índice, base o primer argumento que
indica la receta. LEFT/RIGHT recorren argumentos de llamadas, grado/radicando y
las cuatro ranuras de la integral.

Al confirmar se cotejan propietario vivo, raíz, época y posición del cursor.
`end()` de cada app cierra su receptor antes de destruirlo. Los fallos de
construcción o incompatibilidad dejan AST, cursor, historial y recientes sin
cambios. Las excepciones de asignación se contienen en las fronteras que usan
las fábricas existentes; no se habilitaron exceptions/RTTI ni se cambiaron sus
flags. El contrato público devuelve éxito/error. El guardado es `noexcept`.

No hay captura de selección: el editor actual no publica un contrato de rangos
seleccionados. Tampoco se captura el operando anterior. Desde un resultado se
continúa en la expresión existente; solo una publicación correcta oculta el
resultado. Ans y el historial no se reescriben a partir de una representación
decimal. Los caminos existentes de continuación, FACT y recuperación permanecen.

`MathInputCalls.h` limita la serialización de `NodeCall` a 18 nombres matemáticos
y sus aridades. Incluye hiperbólicas, complejos, enteros, combinatoria y diff.
No acepta llamadas arbitrarias a Giac, procesos, archivos o configuración.
La revisión popup corrige el pivote LVGL de los glifos de delimitadores
genéricos: avance centrado y baseline derivados de la caja y `ofs_y`.
Los paréntesis STIX conservan su ruta; no cambian fórmulas de layout,
MATH constants, estilos ni cursor. Un detector de píxeles falla contra
el renderer anterior y pasa para las barras actuales en tres estilos. Las cajas de las previews se calculan con los `FontMetrics` del
MathCanvas real, cuya escala procede de las fuentes STIX.

## Contenido y capacidades

La raíz presenta valor absoluto, raíz enésima y logaritmo de base editable.
Debajo hay cinco categorías: cálculo, complejos, aritmética/combinatoria,
trigonometría y redondeo. Las 29 entradas descubribles funcionan en Calculation,
Equations y Calculus; tres variantes de logaritmo también en Grapher.
Fracción, potencia, raíz cuadrada y otras acciones con tecla directa no aparecen
en listas ni búsqueda. Las 43 identidades siguen resolviéndose para conservar
favoritos antiguos, sin remapearlos ni borrarlos.

Se mantienen hiperbólicas e inversas, factorial, binomial, gcd/lcm/resto,
re/im/conj/arg, derivada, integral definida y la constante i. VAR continúa
con las variables guardadas. Las casillas de la inserción son nodos vacíos
reales; los argumentos visibles del menú son solo ilustrativos.

Grapher conserva su serializador y clasificador existentes. Sus inversas
trigonométricas no superaron la ruta completa: el rótulo tipográfico y la
gramática numérica no admiten esa entrada. Se retiró su capacidad Grapher; no
se anunció soporte basándose únicamente en Giac. También se excluyen allí
llamadas generales, raíz indexada, integral, valor absoluto/corchetes bajo sus
recetas actuales, i y z. Las filas siguen visibles, atenuadas, con rechazo
recuperable al confirmar; un favorito no se sustituye por otro elemento.

Se difieren sumas/productos con ligadura completa, otras letras griegas sin
semántica de entrada y unidades. No se añaden categorías vacías, variables que
aparenten unidades, conversiones ni un motor dimensional.

## Interacción a 320 × 240

Al abrir se selecciona valor absoluto. El popup mide 284 × 156..180 px,
con margen lateral de 18 px y parte superior en y=54..78. Sobre él se muestra
la expresión viva, respetando el estilo del receptor y siguiendo su cursor;
no se clona su AST ni se modifica el viewport del editor. Un fondo suave evita
fragmentos duplicados de la expresión anterior en los márgenes.

Funciones, Favoritos, Recientes y Buscar son cuatro pestañas compactas.
UP desde la primera fila enfoca las pestañas; LEFT/RIGHT elige y EXE/DOWN abre.
Desde el primer resultado de búsqueda, UP vuelve antes al campo de texto.
BACK conserva selección y scroll de los niveles de categorías/variantes.
La pila tiene seis niveles y se reutilizan cuatro filas; una fila alta reduce
el número visible sin reducir las fuentes matemáticas. Los rótulos largos
usan elipsis y Ayuda presenta el texto completo.

| Control | Acción dentro de Toolbox |
|---|---|
| UP / DOWN | Recorrer filas; UP desde el primer resultado vuelve al campo |
| EXE en categoría / RIGHT | Entrar |
| EXE en elemento | Insertar su acción principal |
| RIGHT en elemento con `+` | Abrir variantes; nunca insertar |
| RIGHT sin hijos | No insertar |
| FORMAT | Añadir/quitar favorito, subir, bajar, ayuda |
| BACK / AC / TOOLBOX | Ayuda/opciones → nivel anterior → cierre |
| HOME | Cancelar y liberar el modal, siguiendo al launcher |
| Puntero | Pestaña: sección; fila: insertar; `+`: variantes; pie: opciones; título: volver; fuera: cerrar |

Logaritmo muestra la acción general log_b(a) y tres variantes mediante RIGHT.
El proveedor solicita inicialmente la variante decimal; se centra cuando hay
espacio. El asterisco indica favorito y `>` una categoría.

En escritorio F6 abre, F5 ofrece opciones y F8 vuelve. Escribir en la lista
abre directamente Buscar. Durante el modal los caracteres llegan por SDL_TEXTINPUT: `h` ya no dispara HOME ni
las letras duplican atajos matemáticos. HOME físico sigue funcionando. Se
restauran fases **lógicas** de SHIFT/ALPHA al cerrar, no teclas físicamente
pulsadas. HOME no restaura esos modificadores. RELEASE y REPEAT del EXE que
publica quedan consumidos hasta soltar o una nueva pulsación; no hay cuarentena
por tiempo. Los clics actúan tras la liberación del puntero.

## Búsqueda

Consulta UTF-8 de hasta 64 bytes, validada por puntos de código; LEFT/RIGHT mueven
su cursor, DEL borra un carácter completo y DOWN/EXE pasa a resultados. El
teclado físico utiliza el plano Text/ALPHA existente, probado con sus posiciones
reales de la matriz. No requiere teclado en pantalla ni red.

Se buscan ambos idiomas, símbolos y aliases. El orden es exacta, prefijo,
palabra/alias y parcial; los empates conservan el orden del catálogo. Las tildes
se pliegan solo para buscar. Los IDs no se normalizan. Un proveedor puede marcar
sus aliases sensibles a mayúsculas; el fixture privado distingue `m` y `M`.

Se cuenta al cambiar la consulta. Las filas se enumeran por rango, sin un buffer
que oculte coincidencias posteriores. Con N entradas, el refresco normal de
cuatro filas y la selección requiere como máximo 20N comprobaciones de rango,
más N para contar al editar. No se consulta Giac, se modifica una variable o se
reconstruye tutor al buscar, enfocar o dibujar.
Si un evento de texto sobrepasa 64 bytes, se publica primero su prefijo de
caracteres completos y después se avisa del límite. El detector compara consulta,
contador de resultados y texto visible para evitar una vista atrasada.

## Favoritos y persistencia

24 favoritos como máximo, sin expulsión ni reordenación automática; 12 recientes
distintos de sesión, actualizados solo tras insertar correctamente. Opciones
permite añadir/quitar y desplazar un favorito una posición. El estado vacío y
el límite tienen mensajes EN/ES.

Se usan `/toolbox-favorites-a.dat` y `/toolbox-favorites-b.dat` en el filesystem
existente, alternando la ranura inactiva. Registro little-endian `NTBX`, versión
1, contador, generación, pares ID/variante y CRC32: **16 + 4N bytes**, máximo
112 B por ranura / 224 B en disco. Se cierra y relee antes de declarar éxito.
Una escritura interrumpida conserva el registro anterior. Se filtran IDs
retirados, variantes desconocidas y duplicados. Un esquema futuro bloquea
escrituras. No se monta, formatea ni borra almacenamiento para recuperarse.

Se agrupan escrituras al cerrar, no al navegar. Un error conserva la sesión y
avisa «Sin guardar»; BACK permite cerrar y EXE continuar con inserción tras el
aviso. HOME prioriza salir: conserva el cambio pendiente y no afirma que sea
duradero. El aviso vuelve a aparecer al intentar guardarlo en otro cierre.
El adaptador native cerraba archivos de lectura incorrectamente por un
cortocircuito booleano; se corrigió sin notificar mutaciones por lecturas.

Native se prueba con `--fs-sandbox-dir` y raíces relativas de prueba, escribiendo
en un proceso y leyendo en otro. Web utiliza las notificaciones y el controlador
IDBFS existentes: modificación, `flushPersistence()` público, recarga y lectura
real en el mismo contexto. No expone FS, memoria Wasm o mutadores privados.

## Extensión prevista

Hasta cuatro proveedores estáticos, incluido el matemático; cada proveedor
publica filas bajo demanda, entrada inicial, identidades enumerables y builder.
La raíz acepta hasta 32 grupos por proveedor y cada proveedor hasta 4.096
identidades. No se crea un árbol de widgets ni un plugin dinámico. Deben
registrarse antes de cargar favoritos o abrir una sesión, con almacenamiento
de duración estática e IDs/grupos únicos. Los builders deben validar sus
parámetros tipados y respetar el presupuesto de previews.

Se prueban nueve variantes matemáticas privadas, selección central, búsqueda,
identidades, reordenación del catálogo y rechazo de duplicados. No llegan al
catálogo de producción. El futuro builder de unidades deberá delegar al motor
de cantidades; esta interfaz no decide su representación numérica. Unidad y
variable, dimensión y magnitud, valor canónico y salida, prefijo y potencia,
y temperatura con origen desplazado siguen siendo contratos diferentes.

## Recursos y verificaciones de la primera entrega (archivo)

**Las cifras y la matriz de esta sección pertenecen a la entrega inicial de
TOOLBOX-01, anterior al popup. No acreditan una repetición de toda la matriz
sobre la revisión actual.** Las comprobaciones y tamaños actuales están en
la sección de revisión popup al principio de este documento.


Los registros detallados se conservan en `out/toolbox-01/`. El pool de
producción sigue siendo 65.536 B. La sesión es transitoria: 23 widgets, cuatro
canvases, cuatro AST de preview retenidos y uno temporal durante sustitución.
Una preview de producción contiene como máximo diez nodos contando su fila.
Las mayores cajas medidas con STIX real son 117 × 37 px.

`Session` ocupa 1.648 B en host de 64 bits y 1.100 B en Xtensa; `Store`, 176/164 B;
Identity, 4 B; Prepared Xtensa, 12 B. Las búsquedas usan buffers locales
acotados; no se introducen reservas de IRAM ni asignaciones en layout/draw/cursor.
La memoria del AST conserva la política del motor existente. El overlay de
asignaciones mide C++ y AST, excluyendo malloc de Giac y el pool C de LVGL.

[WROOM local final](../out/toolbox-01/firmware/numos-esp32-s3-wroom-1u-n16r8/identity.json):

| Medida | Baseline | Candidato | Delta |
|---|---:|---:|---:|
| Imagen | 5.707.440 B | 5.728.864 B | +21.424 B |
| Flash enlazada | 5.707.077 B | 5.728.493 B | +21.416 B |
| RAM estática | 119.200 B | 119.408 B | +208 B |
| Texto IRAM | 60.407 B | 60.407 B | 0 B |

SHA-256 de esa imagen local:
`75fdd8342e31940d773530e79f50865ddf2a6435fe163184e62ea0695e4fe207`.
Flash enlazada y RAM usan las secciones del size-check instalado de PlatformIO,
no el tamaño del ELF con información de depuración. El script
`scripts/profile-toolbox-firmware.py` registra secciones, hashes, tipos DWARF y
prólogos `entry a1` del Xtensa; conserva las imágenes locales con su identidad.

Matriz final compilada, junto con native `emulator_pc`. Todos los tamaños son
bytes; los [registros de firmware](../out/toolbox-01/firmware/resources-firmware.json)
contienen los nombres completos de entorno, secciones y hashes:

| Entorno | Imagen | Flash enlazada | RAM estática | Texto IRAM |
|---|---:|---:|---:|---:|
| WROOM ordinario | 5.728.864 | 5.728.493 | 119.408 | 60.407 |
| WROOM bring-up | 5.740.528 | 5.740.165 | 119.592 | 60.407 |
| WROOM bring-up display-perf | 5.742.176 | 5.741.809 | 119.664 | 60.407 |
| WROOM demo | 5.668.416 | 5.668.049 | 119.552 | 60.407 |
| CAM ordinario (`esp32s3_n16r8`) | 5.641.008 | 5.640.641 | 118.000 | 59.039 |
| CAM validate | 5.684.672 | 5.684.305 | 118.008 | 59.039 |
| CAM hwux | 5.643.568 | 5.643.201 | 118.000 | 59.039 |

Marcos propios de pila: `searchRank` 304 B, `rankText` 240 B, `refresh` 272 B,
`Store::save` 304 B, `open` 128 B e `insertPrepared` 48 B. La pareja de búsqueda
requiere 544 B antes de contar sus llamadores. Estas cifras **no son** el máximo
de pila de una tarea: faltan llamadas LVGL/filesystem, recorridos del AST y
marcos simultáneos de refresco. El alto de pila disponible en placa queda en el
checklist físico; no se infiere de la ausencia de desbordamientos en host.

[Muestra host final](../out/toolbox-01/resources-host-final.json) de 54 aperturas
mixtas: media 802 µs, máximo 2.098 µs; 54 búsquedas, 423/4.182 µs;
783 refrescos, 230/12.067 µs; 53 cierres, 263/7.381 µs. Hubo compilaciones
concurrentes en el host; se conservan los máximos sin filtrar. Son ámbitos
inclusivos, **no latencias acreditadas del ESP32**.
El ámbito activate mezcla navegación, escritura e inserción y se conserva sin
presentarlo como una medida aislada de inserción. El pico adicional observado
del overlay fue 1.812 B al abrir; una sustitución de preview o inserción requirió
464 B temporales; guardado, 164 B. Son muestras, no límites universales de heap.

En el pool fijo se muestrearon mínimos de 8.704 B libres / 8.120 B para el mayor
bloque libre en los recorridos examinados. Se ejecutaron 50 ciclos con 60 filas
de historial y una expresión de 150 términos, además de 50 ciclos HOME después
de cuatro calentamientos registrados. En HOME se mantienen 67 objetos, tres
timers y cero handles retenidos. Las series crudas conservan un escalón inicial
de 8 B y diferencias de capacidad de bloques entre insertar/cancelar; las
comparaciones posteriores son exactas por recorrido, sin tolerancia ni reset
de Giac. No se considera el historial legítimo una fuga.

Grapher agota el pool fijo antes de abrir Toolbox tanto en el baseline B como
en el candidato (mismo aborto LVGL). Esta excepción histórica continúa abierta;
los controles de Grapher certifican el perfil CLIB, no el pool fijo ni hardware.

Controles realizados:

- 80 recorridos de Toolbox por eventos, incluidas 43 fixtures completas en
  Calculation; 101 controles adicionales
  de capacidades (43 Equations, 43 Calculus, 15 Grapher). Equations compara la
  serialización; Calculus deriva; Grapher compila y muestrea donde procede.
- 3.752 comprobaciones de constructor, padres, orden de argumentos, ranuras,
  multiplicación y rollback;
  302 puntos de fallo persistente de asignación. 17 escenarios privados de UI
  incluyen apertura, preview, búsqueda, inserción, guardado y época obsoleta.
  No se inyectó fallo en cada malloc interno de LVGL: ese allocator conserva
  sus asserts; la apertura tiene preflight de espacio libre y bloque contiguo.
- Detectores de IDs duplicados, ES ausente, codificación dañada, aliases,
  variantes, esquema futuro/obsoleto, CRC, truncamiento, IDs desconocidos,
  duplicados persistidos, filesystem ausente, límite y reordenación.
- 11.894 comprobaciones de fuentes/previews, 151.539 de espaciado y 911 de
  presentación FORMAT, sin promover goldens ni máscaras.
- 113 casos de la puerta rápida Calculation; corchetes, viewport, FORMAT/ENG,
  mapa de 50 teclas y modificadores; Steps, Templates y Calculate conservados.
- 900 trazas matemáticas idénticas al baseline B, excluyendo exclusivamente
  tiempos y el directorio del ejecutable (su nombre se valida).
- Toolbox en shell y componente en Chromium, Firefox y WebKit; búsqueda SDL,
  puntero, no evaluación durante navegación, publicación y recarga IDBFS.
  Regresiones web de corchetes, viewport, FORMAT/ENG y frontera de publicación.
- WASM-MATH Release compila y pasa su suite en los tres navegadores. Su target
  continúa incluyendo solo el cierre matemático, sin LVGL ni Toolbox; esta tarea
  no cambió `GiacEngine` ni la frontera pública headless.

Los intentos fallidos de los harnesses se conservan. Se corrigieron selección
de objetos del renderer, rutas ASCII requeridas por MinGW/Emscripten en este
host y las expectativas de modificadores; no se declararon éxitos esos intentos.
La copia ASCII usada para web tiene manifiesto de hashes de fuentes y utiliza
las dependencias ya instaladas. Los binarios privados de observación/fallos no
se enlazan al firmware.

La matriz inicial encontró `SCons.Tool.FortranCommon` ausente al llegar a HWUX.
El host tenía PlatformIO 6.1.19 en PATH y 6.2.0 en la instalación de VS Code;
los artefactos del directorio compartido dejaron de estar disponibles durante
esa ejecución. Se reconstruyó con **6.2.0 ya instalado**, en directorios privados
separados para native y firmware. Los INI privados solo cambian `build_dir`;
el perfil privado de pool fijo cambia además el allocator native para medir
los mismos 64 KiB del firmware. No se editó la configuración del usuario.

La prueba web separa la tecla de lanzamiento y la escritura en dos barreras
observables de transición/publicación. Con carga de compilación, agruparlas
permitía escribir antes de estar listo el editor; se corrigió el recorrido,
sin ampliar timeouts ni saltar el control de eventos.

## Referencia externa

Se consultaron [Toolbox](https://www.numworks.com/manual/toolbox/) y
[Calculation](https://www.numworks.com/manual/calculation/), y se verificó
[Epsilon 72c8306f4fe3adf3bfc9c79802a39b80afb8e988](https://github.com/numworks/epsilon/tree/72c8306f4fe3adf3bfc9c79802a39b80afb8e988),
archivos `epsilon/apps/shared/math_toolbox_controller.cpp` y
`epsilon/apps/shared/math_toolbox_content.cpp`. Copias y hashes están en el
baseline. Se estudiaron organización, entrada a categorías, previews y acción
principal. La implementación, recetas y textos EN/ES son propios; no se copió
su tabla ni se incorporaron Poincaré/Escher.

## Entrega visual y evidencias de la primera entrega (archivo)

[ZIP visual autocontenido](../out/toolbox-01/TOOLBOX_01_visual.zip) y
[galería sin conexión](../out/toolbox-01/visual/index.html): 32 capturas originales
de 320 × 240 y sus ampliaciones 4× por vecino más próximo. Incluyen raíz,
favoritos vacíos/poblados/orden/límite, recientes, las siete categorías,
búsqueda EN/ES, consulta vacía, ayuda, variantes, integral, contexto rechazado,
fallos recuperables, edición, historial, continuación y Steps.

Se inspeccionaron todas las capturas. La revisión corrigió rótulos que invadían
la fila siguiente, el aviso de error que transparentaba la lista y bordes de
selección heredados al abrir Opciones. Los originales utilizan las fuentes del
producto; el ZIP no contiene archivos de fuentes ni recursos de red. Esta es
revisión visual del emulador, no aprobación humana de un LCD físico.

Las identidades y las comprobaciones aceptadas se reúnen en el
[manifiesto de cierre](../out/toolbox-01/delivery-manifest.json). Los intentos
fallidos conservados no forman parte de sus resultados aprobados. La
[comparación del corpus](../out/toolbox-01/corpus-comparison.json) explicita sus
únicas exclusiones; el [inventario visual](../out/toolbox-01/visual/manifest.json)
identifica cada captura y su hash.

## Archivos modificados

Lista completa de archivos de trabajo de esta entrega, sin staging:

| Responsabilidad | Archivos |
|---|---|
| Catálogo y llamadas admitidas | `src/math/toolbox-catalog.json`, `src/math/ToolboxEntries.inc`, `src/math/ToolboxCatalog.h`, `src/math/ToolboxCatalog.cpp`, `src/math/MathInputCalls.h`, `src/math/CalculationEngine.cpp` |
| Persistencia | `src/math/ToolboxStore.h`, `src/math/ToolboxStore.cpp`, `src/hal/FileSystem.h` |
| Editor transaccional | `src/math/CursorController.h`, `src/math/CursorController.cpp` |
| Navegador y aviso recuperable | `src/ui/Toolbox.h`, `src/ui/Toolbox.cpp`, `src/ui/StatusBar.h`, `src/ui/StatusBar.cpp` |
| Aplicaciones | `src/apps/CalculationApp.cpp`, `src/apps/EquationsApp.cpp`, `src/apps/CalculusApp.cpp`, `src/apps/GrapherApp.cpp` |
| Eventos y plataforma | `src/SystemApp.cpp`, `src/hal/NativeHal.cpp`, `src/input/KeyboardManager.h`, `wasm/numos-keypad.js`, `platformio.ini` |
| Generación, perfil y galería | `scripts/generate-toolbox-catalog.py`, `scripts/profile-toolbox-firmware.py`, `scripts/package-toolbox-visuals.py` |
| Pruebas y overlay privado | `scripts/test-tutor-composition-host.py`, `scripts/test-toolbox.py`, `scripts/test-toolbox-contexts.py`, `scripts/test-toolbox-faults.py`, `scripts/test-toolbox-lifecycle.py`, `scripts/test-toolbox-source.py`, `scripts/build-toolbox-allocation-probe.py`, `tests/host/toolbox_checks.cpp`, `tests/host/toolbox_visual_checks.cpp`, `tests/wasm/toolbox.mjs` |
| Documentación | `docs/TOOLBOX_01.md`, `docs/TOOLBOX_01_CATALOG.md` |

Los artefactos, logs, INI privados, fuentes de instrumentación, parches y hashes
se guardan bajo `out/toolbox-01/` o la caché privada indicada en sus registros;
no son nuevas dependencias ni archivos de producto.

## Reproducción y revisión física posterior

Generar/comprobar inventario: `python scripts/generate-toolbox-catalog.py --check`.
Fixtures de producto: `scripts/test-toolbox.py --bin <native> --out <evidencia>`;
capacidades: `scripts/test-toolbox-contexts.py`; ciclos: `scripts/test-toolbox-lifecycle.py`
con el build privado de pool fijo; faults: `scripts/build-toolbox-allocation-probe.py`
y `scripts/test-toolbox-faults.py`. Cada runner conserva scripts, logs y resultados.
Para web, definir `NUMOS_WEB_ROOT` al paquete local y ejecutar
`node tests/wasm/toolbox.mjs`. No son órdenes de despliegue.

Checklist pendiente de hardware, sin atribuir revisión humana del LCD:

1. Verificar identidad de la imagen instalada antes de cualquier flash futuro.
2. Revisar nitidez y foco EN/ES a 320 × 240, integral, fracción y grado pendiente.
3. Recorrer ALPHA físico, cursor UTF-8, EXE mantenido, BACK y HOME; revisar rebotes.
4. Completar raíz cúbica de 8, log₂(8), gcd(12,8), inserción en exponente y división.
5. Probar favoritos, límite, orden, reinicio y recuperación de un guardado fallido.
6. Comprobar Steps, Templates, VAR/STO, FORMAT, TABLE, ENG y continuidad con Ans.
7. Medir heap interno/PSRAM, stack libre y latencias reales con historial lleno.

Asunto sugerido para un commit futuro, no creado:
`feat(toolbox): add searchable contextual catalog and favorites`.
