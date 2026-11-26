# Batman: Operación Bronx

Juego 2D para Informática II. Empieza con una infiltración en los callejones y termina con un rescate en las azoteas.

## Abrir en Qt Creator

1. Instala un kit de escritorio de Qt 6.2 o superior con los módulos Widgets y Multimedia, compilador C++17 y CMake.
2. Abre `momento 3/CMakeLists.txt` como proyecto.
3. Selecciona el kit de escritorio, configura el proyecto y pulsa **Ejecutar**.

Desde una terminal configurada para Qt y CMake, en la carpeta `momento 3`:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

El binario incluido en `ejecutable/linux-x86_64/OperacionBronx` se compiló para Debian 13 de 64 bits. En ese sistema necesita las bibliotecas de ejecución de Qt 6 Widgets y Multimedia y sus complementos gráficos (`libqt6widgets6`, `libqt6multimedia6` y `qt6-qpa-plugins`). En Windows, puedes compilar con el kit instalado en Qt Creator y usar el `windeployqt` del mismo kit para copiar las DLL y complementos al lado del `.exe`.

## Controles

| Tecla | Infiltración | Rescate |
| --- | --- | --- |
| WASD o flechas | Moverse | A/D o flechas laterales para correr |
| Mayús | Mantener para caminar en sigilo | Pulsar para hacer un salto alto |
| E | Recoger evidencia cercana | Pedir a un civil cercano que te siga |
| Q | Revelar evidencias durante ocho segundos | |
| Espacio | | Saltar |
| F | | Engancharse o soltarse al balancear hacia el otro techo |
| Esc | Pausar o reanudar | Pausar o reanudar |

En la infiltración, reúne tres evidencias y entra en la salida. Tres detecciones terminan el intento. En el rescate, espera en la zona segura hasta que lleguen ambos civiles. Tienes tres minutos.

## Archivos del proyecto

- `PLAN.md` contiene las decisiones y los criterios de verificación del Momento III.
- `src/personajes.*`, `src/niveles.*` y `src/partida.*` contienen la lógica de simulación, sin dependencias de Qt.
- `src/ventana.*`, `src/vista.*` y `src/audio.*` integran controles, gráficos y sonido con Qt.
- `diagramas/` contiene los diagramas de clases detallado y resumido en PDF y su fuente en Graphviz.
- `recursos/` contiene recortes de los sprites y fondos de `momento 2/MOMENTO 2.pdf` y los sonidos sintetizados para este proyecto.
