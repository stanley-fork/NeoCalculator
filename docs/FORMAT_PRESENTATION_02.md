# Calculation: entrada vacía y tildes de FORMAT

Corrección posterior a `FORMAT_BOARD_01.md`: una entrada completamente vacía en
Calculation queda sin casilla. Las ranuras de las plantillas siguen visibles,
incluido el historial y el estado después de EXE. El menú español recupera los
glifos de «Científica», «Ingeniería», «periódico» y «Fracción».

## Implementación

- `MathCanvas::setEmptyRootPlaceholderVisible(false)` se aplica exclusivamente al
  canvas de entrada de Calculation. Solo omite la tinta de un `Empty` que sea el
  único hijo directo de la raíz. No elimina el nodo, no cambia medidas, cursor,
  validación ni serialización. Otros canvases conservan el valor por defecto
  `true`; las bases, exponentes, fracciones y raíces conservan sus casillas.
- Las filas del menú usan `ui::tutorFont14()`: el Montserrat existente con su
  complemento español, ya incluido en el producto. Se conservan ASCII, kerning,
  tamaño, color y textos. No se generan fuentes ni se añade un tamaño nuevo.
- La observación privada del menú consulta la fuente real de cada etiqueta y
  verifica `is_placeholder` para sus caracteres. No añade código de diagnóstico
  al firmware.

## Detección y comprobaciones

`scripts/test-calculation-format-presentation.py` reproduce mediante teclas las
dos regresiones contra el ejecutable conservado. Antes falla la ausencia de
caja tanto al entrar como al borrar toda la entrada, y falla la presencia de
glifos en español. Después pasan EN y ES, con capturas de 320×240 y ampliaciones
enteras ×4. También cubre plantillas pendientes, EXE e historial. Los tres estados
del menú en inglés conservan exactamente los píxeles del área de contenido.

`tests/host/math_placeholder_checks.cpp` pasa 217 comprobaciones: las dos políticas
de raíz, con/sin cursor, tres estilos y cuatro familias de ranuras. Comprueba los
bordes en el framebuffer, el blanco de la raíz que lo solicita, la validación de
entradas incompletas y cero asignaciones propias al medir/dibujar.

Pasan además las 113 pruebas de entrada (incluida la reproducción SDL completa)
y las 23 sesiones de FORMAT. Compilan native, WROOM, CAM y Wasm. El paquete web
local pasa empaquetado, validación y smoke. No se publica.

Chromium y Firefox pasan FORMAT en shell y componente. El harness rápido de
WebKit capturó el menú aún abierto tras un clic (y en otra ejecución una pantalla
pendiente de refresco tras FORMAT). La repetición privada con 200 ms entre grupos
de teclas y 480 ms después del clic pasa ambas superficies, manteniendo idénticas
comparaciones de píxeles y resultados. Se conservan los fallos y esa reproducción;
no se ha cambiado el producto, ampliado máscaras ni declarado que la ejecución
rápida pasó. Queda por robustecer la sincronización de ese harness.

## Placa y recursos

La misma WROOM `44:B1:76:A7:B7:2C` de COM9 tenía la imagen verificada `78f95453…`.
Se conserva esa aplicación y se verifica contra la flash antes de sustituirla.
La imagen ordinaria nueva, SHA-256
`251ded7b3264b89b674c5a0495f09a155fa07816610d607873c9296bf39d95c1`, tiene 5707408 bytes.
Se instala solo en OTA0 (`0x10000`), se verifica por digest y arranca con
`[BOOT] OK`. El prefijo de 64 KiB es idéntico antes/después de escribir: se conserva
la configuración actual del usuario, incluido el idioma. No se modifica el
bootloader, las particiones, el almacenamiento ni la seguridad.

WROOM: +32 bytes en el binario; RAM estática e IRAM sin aumento. `MathCanvas`
continúa en 184 bytes y `CalculationApp` en 840 en Xtensa: la opción ocupa padding
ya disponible. El frame propio de `drawEmptyBaseline` es de 208 bytes; no se
introduce recursión ni asignaciones de heap. Son medidas del binario/objetos, no
una afirmación de pico total de heap o revisión humana del LCD.

## Evidencia y preservación

`out/format-polish-02/` conserva snapshot y diff iniciales, ambos ejecutables,
resultados, capturas y registros de compilación/instalación. Galería:
`visual/index.html`; ZIP: `format-polish-02-visual.zip`, con recursos relativos
comprobados y sin archivos de fuentes. Las imágenes son del renderer C++/LVGL
native; no son fotografías de la placa.

Se conserva el trabajo anterior sin commit sobre
`ffb7082ba3b7db7741a8a603a5ad83f40c6c2682`, sin staging ni cambios en
`.vscode/settings.json`. El ejecutable del launcher de Windows también se actualiza.

Archivos de esta corrección:

- `src/apps/CalculationApp.cpp`
- `src/ui/MathRenderer.cpp`
- `src/ui/MathRenderer.h`
- `scripts/build-calculation-observer.py`
- `scripts/test-calculation-format-presentation.py`
- `tests/host/math_placeholder_checks.cpp`
- `docs/FORMAT_PRESENTATION_02.md`
