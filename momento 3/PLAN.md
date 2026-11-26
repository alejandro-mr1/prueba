# Plan de programación · Momento III

Batman: Operación Bronx es un juego 2D de dos niveles. El punto de partida son la extensión del contexto y el diagrama de clases de las entregas anteriores. La fecha indicada en la guía es el 27 de noviembre de 2026.

## Decisiones de diseño

| Área | Decisión | Cómo se comprueba |
| --- | --- | --- |
| Interfaz | Qt 6 Widgets, `QPainter` y controles de teclado. Escena lógica de 960 × 560 que conserva su proporción. | Iniciar, pausar, reanudar y reiniciar desde la interfaz. |
| Simulación | C++17, coordenadas en píxeles y tiempo en segundos. La interfaz alimenta el modelo con un paso fijo de 1/120 s. | Los tiempos de habilidad, persecución y rescate son independientes de la frecuencia de dibujo. |
| POO y memoria | `Personaje` es la base de `Batman`, `Guardia` y `Civil`. `Partida` posee el nivel activo y cada nivel posee sus personajes mediante `std::unique_ptr`. | Al reiniciar se recrea el nivel y se libera el anterior. Las clases de simulación compilan sin Qt. |
| Nivel 1 | Mapa cenital, tres evidencias y una salida. Tres detecciones terminan el intento. La habilidad de detective dura ocho segundos, revela pistas y baja la velocidad. | La salida queda bloqueada hasta recoger tres evidencias. Una alerta sostenida registra una sola detección y devuelve al punto inicial. |
| Agente | Percepción por cono y audición, razonamiento por estados, acción con rutas y aprendizaje mediante conteo de encuentros en tres zonas. | Al oír investiga, al ver persigue, al perder contacto revisa la última posición y luego prioriza zonas con encuentros. Las paredes bloquean la visión. |
| Dificultad | Dos perfiles para nivel 1. Cambian alcance y apertura de visión, audición, patrulla, búsqueda, barrido y tiempo de exposición a los reflectores. | La misma maniobra exige elegir mejor el camino, bajar el ruido y aprovechar ventanas más cortas. El número de evidencias y detecciones es idéntico. |
| Nivel 2 | Techos con desplazamiento lateral, dos civiles que siguen el recorrido del jugador y un punto seguro. El rescate dispone de 180 segundos. | Los dos civiles deben llegar a la zona segura. Una caída devuelve al último techo alcanzado. La cuenta regresiva sigue durante el intento. |
| Físicas | Reflector sinusoidal, salto parabólico y péndulo con salida tangencial. Gravedad, impulso, rapidez angular de entrada y alcance de enganche quedan definidos en la lógica. | Cambia la orientación del haz, el salto tiene subida y caída, el gancho mantiene longitud constante y conserva la velocidad de salida. |
| Excepciones y audio | Validar los parámetros de geometría y física. Informar recursos gráficos que no se puedan cargar en la GUI. Usar audio original para el ambiente y los eventos. | Comprobar desde el juego que el ambiente se activa en el nivel 1 y que hay efectos distintos para evidencia, alerta, rescate y gancho. |

## Secuencia de trabajo

1. Preparar la rama `momento-3`, la compilación y el modelo base.
2. Completar el nivel de infiltración y comprobar las rutas, la dificultad y la detección.
3. Completar el rescate y ajustar ambos huecos para que requieran el gancho.
4. Integrar las pantallas, el arte de referencia del Momento II y el audio. Jugar ambos niveles de principio a fin.
5. Generar los diagramas de clases detallado y resumido desde la arquitectura final y compilar el ejecutable para la plataforma de prueba. Mantener historial de cambios por hitos. La guía pide commits al menos cada dos días mientras dure el desarrollo.
