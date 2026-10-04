# UNIT-CATALOG-02 — datos, fuentes y alcance científico

Fecha de revisión: **3 de octubre de 2026**. Este documento describe el registro ampliado y su trazabilidad. La inserción tipada no activa todavía operaciones dimensionales ni conversiones de usuario.

## Cobertura real

El adjunto contiene **301 filas** en la sección de unidades determinadas y **9 filas** regionales o históricas. Las 310 están relacionadas con una identidad existente, una nueva unidad, una composición o una referencia física. Gravedad estándar y carga elemental se dirigen al espacio separado de referencias, sin convertirse en variables `g` o `e`.

El registro integrado contiene **202 definiciones de unidad**, **312 accesos de catálogo** y **58 referencias físicas o contextuales**. Los accesos incluyen potencias y productos de componentes; no son 312 unidades elementales nuevas. Las variantes prefijadas tampoco se cuentan como definiciones independientes. La [matriz de cobertura](UNIT_CATALOG_02_COVERAGE.md) relaciona las **353 filas de las cuatro tablas** del usuario con las identidades finales.

| Naturaleza de las 202 definiciones | Cantidad | Tratamiento |
| --- | ---: | --- |
| Exactas | 193 | Racionales, potencias decimales y potencias explícitas de π |
| Medida | 1 | Dalton, con incertidumbre típica CODATA |
| Correspondencias históricas | 5 | Castilla/Burgos de 1852, conservadas como aproximadas |
| Convenciones publicadas aproximadamente | 3 | Debye, horsepower de caldera y tsubo |

La ampliación cubre astronomía, topografía, sistemas anglosajones, cocina, mecánica, propiedades térmicas, fluidos, electricidad y magnetismo, química, dosimetría, fotometría, información digital y consumo en transporte. Mantiene las siete familias base y las 22 unidades derivadas SI con nombre especial de la entrega anterior.

## Fuente única y archivos de evidencia

La fuente de producción es [registry.json](../src/math/units/registry.json). Cada definición conserva fuente, localización, variante, dimensión, magnitud, relación canónica y exactitud. [generate-unit-catalog.py](../scripts/generate-unit-catalog.py) genera las tablas inmutables, las entradas del proveedor y el inventario. No se han copiado tablas ni traducciones de NumWorks.

En la ruta ignorada `out/unit-catalog-02/data/` se conservan:

- `requested-rows.json`: las 310 filas de las secciones 1 y 3 y su contexto original.
- `build-extension.py` y `extension.json`: propuesta reproducible previa a la integración, con identidades estables y cambios explícitos sobre las existentes.
- `audit-extension.py` y `audit.json`: **130 controles independientes** sobre factores, estructura, exactitud, variantes, dominios y cobertura; el informe incluye el hash del JSON comprobado.
- `source-access.json` y `sources/`: contenido recuperado, hashes y limitaciones reales de acceso.
- `AUDIT_NOTES.txt`: decisiones de revisión y discrepancias encontradas en las fuentes.

La propuesta intermedia no sustituye al registro integrado ni debe sobreescribirlo: las revisiones finales de nombres, ayudas y metadatos están en producción. Los controles integrados están en [test-expanded-unit-data.py](../scripts/test-expanded-unit-data.py); la ejecución final registra **69.665 controles aprobados** en `out/unit-catalog-02/expanded-host-final.log`.

## Fuentes y decisiones de revisión

**SI actual.** Se conserva la autoridad del BIPM, novena edición, versión **4.01 de junio de 2026**, consultada en la entrega anterior. Los 24 prefijos son los vigentes; la selección neutral no se cuenta como otro prefijo oficial. CEM aporta terminología española, sin utilizar traducciones antiguas para excluir prefijos posteriores. [BIPM](https://www.bipm.org/en/si-brochure-9), [prefijos SI](https://www.bipm.org/en/measurement-units/si-prefixes).

**Convenciones exactas.** UCUM **2.2, 17 de junio de 2024**, se utiliza para relaciones convencionales revisadas, especialmente cadenas de volumen y masa. No se ha importado numéricamente todo su catálogo: contiene valores astronómicos antiguos y una fila de `phot` con un factor que contradice NIST. Se conserva `phot = 10⁴ lx`, comprobado en NIST. [UCUM](https://ucum.org/ucum), [NIST SP 811, B.8](https://www.nist.gov/pml/special-publication-811/nist-guide-si-appendix-b-conversion-factors/nist-guide-si-appendix-b8).

**Pie y geometría.** Pulgada, pie, yarda, cadena, eslabón, rod, furlong, acre y sus productos usan el **pie internacional**. El pie topográfico estadounidense histórico tiene su propia identidad `1200/3937 m`; no redefine el pie habitual. [NIST, factores revisados](https://www.nist.gov/pml/us-surveyfoot/revised-unit-conversion-factors).

**Astronomía y tipografía.** El año luz utiliza el año juliano de 365,25 días; no un año civil. El pársec conserva simbólicamente `(648000/π) au`. Punto y pica son exclusivamente DTP/PostScript: `1/72 in` y `1/6 in`; no se mezclan con variantes tipográficas históricas. [IAU 2015 B2, nota explicativa 4](https://arxiv.org/abs/1510.06262), [W3C, longitudes absolutas CSS](https://www.w3.org/TR/css-values-4/#absolute-lengths).

**Calorías y potencia.** Se distinguen caloría termoquímica, caloría de Tabla Internacional, sus Btu, therm US/EC y los tipos de horsepower. Los factores redondeados de una tabla no se convierten en definiciones exactas: el Btu termoquímico se deriva de `4,184 J`, `453,59237 g` y una diferencia Fahrenheit de `5/9 K`; el horsepower de caldera permanece aproximado. La tonelada equivalente de petróleo es una convención estadística de energía, no una propiedad universal del petróleo. [Notas NIST 9 y 24](https://www.nist.gov/pml/special-publication-811/nist-guide-si-footnotes), [anexos energéticos del Gobierno británico](https://assets.publishing.service.gov.uk/media/5a81d77fe5274a2e87dbfc46/Annex_A-D.pdf).

**Presión y permeabilidad.** Las columnas convencionales de Hg y agua identifican densidad, gravedad y altura; no representan cualquier columna física real. Para Hg se conserva la relación convencional de densidad `13595,1 kg/m³`, presente en CLDR, junto con la distinción de convenciones de NIST. CLDR tampoco se importa íntegramente. El darcy deriva de su definición de flujo original, reproducida por USGS, y conserva `10⁻⁷/101325 m²` como racional exacto. [NIST, nota 12](https://www.nist.gov/pml/special-publication-811/nist-guide-si-footnotes), [CLDR, relaciones de unidades](https://github.com/unicode-org/cldr/blob/main/common/supplemental/units.xml), [USGS Water-Supply Paper 887, p. 9](https://pubs.usgs.gov/wsp/0887/report.pdf).

**Constantes y valores medidos.** El dalton conserva `1,66053906892 × 10⁻²⁷ kg` con incertidumbre típica `5,2 × 10⁻³⁷ kg`, según CODATA 2022. La masa `eV/c²` sí deriva de constantes exactas y se almacena sin redondeo binario. El debye se marca como correspondencia aproximada: la fuente IUPAC consultada no justifica imponer silenciosamente una conversión CGS exacta después de la revisión del SI. [CODATA 2022](https://physics.nist.gov/cuu/Constants/Table/allascii.txt), [IUPAC Green Book, cuarta edición abreviada, pp. 18 y 40](https://iupac.org/wp-content/uploads/2025/03/IUPAC-GB4Abridged.pdf).

**Cocina y medidas técnicas.** Se diferencian taza estadounidense habitual, taza FDA de 240 mL y taza métrica de 250 mL, además de cucharadas de 15 y 20 mL. La convención culinaria se declara en la etiqueta; no se generaliza a cualquier receta. La unidad de rack es una altura nominal. La unidad enzimática exige condiciones de ensayo y se diferencia de la unidad internacional biológica contextual. [FDA, §101.9(b)(5)(viii)](https://www.ecfr.gov/current/title-21/chapter-I/subchapter-B/part-101/subpart-A/section-101.9), [convenciones australianas del editor](https://www.nigella.com/ask/weights-and-measures-for-australia), [Dell, definición de U](https://www.dell.com/support/kbdoc/en-rs/000144807/me4-locating-the-service-tag), [IUPAC I03114](https://goldbook.iupac.org/terms/view/I03114).

**Regionales e históricas.** CEM publica correspondencias de 1852 y explica su redondeo. La fanega superficial consultada aparece con `6439,5617 m²`, más cifras que el adjunto; se conserva como histórica y aproximada. La fuente oficial de Hong Kong identifica el catty común y el tael de `1/16` de ese catty, sin extrapolarlos al comercio de oro. JETRO publica el tsubo redondeado; la referencia del rai se sustituyó por una fuente de la Oficina Nacional de Estadística de Tailandia porque la página BOI aportada ya no mostraba la conversión. [CEM, pp. 643 y 656](https://www.cem.es/sites/default/files/2019-11/00000458recurso.pdf), [Chinese Medicine Council of Hong Kong, PDF p. 27](https://www.cmchk.org.hk/pcm/pdf/guide_whole_chm_bfver_c.pdf), [JETRO, PDF p. 104](https://www.jetro.go.jp/ext_images/en/reports/survey/pdf/2013_05_01_biz.pdf), [NSO, censo de pesca de 1985, p. 26 §1.7](https://www.nso.go.th/nsoweb/storage/ebook/2023/20230509185951_10500.pdf).

### Límites de recuperación de fuentes

La descarga directa de FDA devolvió una página de solicitud de acceso pese a responder HTTP 200; el texto sustantivo se verificó mediante la herramienta web. Dell e IUPAC rechazaron la descarga directa, pero sus páginas oficiales fueron legibles por esa vía. El PDF de NSO rechazó la descarga directa; el índice del buscador mostró el párrafo oficial con `1 rai = 1600 m²`. Estas diferencias están anotadas en el manifiesto y no se presentan como archivos descargados satisfactoriamente.

La página de Heat Holders aportada ya no mostraba el factor numérico del tog. Se utilizó una especificación técnica de IOM que sí establece la relación con la resistencia térmica superficial. [IOM, especificación de mantas, PDF p. 1](https://www.iom.int/sites/g/files/tmzbdl486/files/annex_1bis_-_iom_synthetic_blanket_h_technical_specifications_and_aql.pdf).

## Identidad y estructura

- `Cal` selecciona exactamente **kilo + caloría termoquímica**. `cal_th` y `cal_IT` identifican las versiones neutrales; las etiquetas distinguen la convención. No hay una segunda identidad nutricional duplicada en favoritos.
- `Wh`, `kWh`, `Ah` y `mAh` son alias de productos de componentes identificados. El prefijo cambia W o A, no la hora ni ambos componentes simultáneamente.
- La familia de kilogramo-fuerza se ancla en `gf`; el acceso habitual `kgf` utiliza kilo. La masa sigue anclada en el gramo de la entrega anterior.
- `L/100 km` contiene un coeficiente racional explícito y componentes L/km; el editor puede mostrar `L/(100·km)` sin ocultar un factor en el nombre.
- Las tasas de muestras, fotogramas, eventos, instrucciones, operaciones de coma flotante y E/S contienen un átomo de recuento y un denominador temporal. La precisión, el tipo de evento y el protocolo siguen siendo contexto necesario.
- Bit y byte pertenecen al dominio de información. Sus siete exponentes SI no autorizan identificarlos con cualquier número sin unidad.
- Las propiedades térmicas compuestas emplean diferencias de temperatura. Los orígenes de Celsius, Fahrenheit y Réaumur no se aplican a esas diferencias.

Los símbolos compartidos —`pc`, `pt`, `st`, `D`, `U`, `gal`— no actúan como identificadores internos. El nombre visible, la magnitud y el ID distinguen sus variantes. Las diferencias de magnitud permanecen explícitas aunque coincidan las dimensiones: Hz/Bq, J/par, Gy/Sv, W/VA/var y viscosidad/difusividad.

## Localización y comprobaciones

El registro conserva nombres EN/ES y variantes auténticas de escritura estadounidense, como `meter`, `liter` y `diopter`. No se inventan diferencias científicas entre España y Latinoamérica cuando ambos usan la misma denominación. Los símbolos permanecen invariantes entre regiones.

[test-expanded-unit-catalog.py](../scripts/test-expanded-unit-catalog.py) usa eventos reales de Toolbox y Calculation. La ejecución final pasó **42 casos**, incluidos `Cal`, el selector de Wh, favoritos recargados, tres galones, el coeficiente de consumo, componentes de recuento, referencias y las cuatro regiones del idioma. El informe está en `out/unit-catalog-02/expanded-events-final/results.json`, reejecutado tras corregir las etiquetas regionales de volumen de gas. Las capturas originales son 320×240; la revisión visual detectó un nombre estadounidense de caloría antiguo, que se corrigió antes de la comprobación final.

Los casos incluyen ayuda EN/ES para pH, Mach, dB, volumen normal de gas, masa electrónica, tiempo atómico, referencias de Planck y Dalton. `pH` también puede significar picohenrio y `dB` decibyte: se comprueban ambas identidades y la selección explícita de la referencia contextual. Una prueba enfocada adicional conserva `L/(100·km)` al mostrar el aviso provisional. La revisión visual de estas capturas no encontró recorte ni glifos ausentes en las ayudas examinadas. La ayuda EN de Dalton aún podría mejorar editorialmente: explica que es una referencia aproximada, mientras la ayuda ES y los datos sí muestran el valor nominal.

[package-expanded-unit-gallery.py](../scripts/package-expanded-unit-gallery.py) empaqueta **89 imágenes únicas originales 320×240** en `out/unit-catalog-02/gallery/index.html` y `out/unit-catalog-02/gallery.zip`, con enlaces relativos, hashes y sin archivos de fuentes. Representan 95 capturas: 46 de la ampliación y Ajustes regionales, 43 del catálogo base conservado y seis canvas Chromium con `Meter`/`Metre` y los nombres regionales de `Normal`/`Standard cubic meter`/`metre`. Seis pares idénticos píxel a píxel se muestran una sola vez y conservan ambas procedencias en el manifiesto. Las escenas base proceden del candidato actual y de **36 casos de eventos aprobados**: incluyen Unidades, Longitud, Metro centrado y extremos, Electricidad, Resistencia, µF, kΩ, cm², cm³, favoritos mm/km/h, búsquedas sensibles a mayúsculas y ±∞. No se han reescalado las imágenes ni reutilizado capturas de un binario antiguo. Los oráculos de datos y estas muestras host **no son operaciones dimensionales disponibles en Calculation**, ni mediciones de latencia o memoria máxima en ESP32.
