
# Simulación de Pelotas Rebotando en un Cubo 3D

Este proyecto implementa una simulación gráfica interactiva utilizando **C**, **OpenGL**, y **SDL2**, donde múltiples pelotas rebotan dentro de un cubo tridimensional. El propósito es demostrar conocimientos avanzados en programación en C, computación gráfica, física básica y renderizado 3D.

![Simulación de pelotas rebotando](https://github.com/BayronDavid/simulacion-colisiones-opengl/blob/main/assets/simulation.gif?raw=true)

---

## 🎯 **Características del Proyecto**

- **Renderizado 3D**: Utiliza OpenGL para dibujar el cubo y las pelotas con iluminación básica.
- **Física**:
  - Simula colisiones elásticas entre pelotas y con las paredes del cubo.
  - Opción para habilitar/deshabilitar la gravedad.
- **Interactividad**:
  - Rotación de la cámara usando las teclas de flecha.
  - Activa/desactiva la gravedad con la tecla `G`.
  - Salida rápida con `ESC` o al cerrar la ventana.
- **Optimización de FPS**: Control del tiempo para mantener una tasa de refresco estable (60 FPS).

---

## 🛠 **Requisitos**

### Herramientas necesarias:
1. **SDL2**: Para la creación de ventanas, manejo de eventos y contexto OpenGL.
2. **GLEW**: Para manejar extensiones de OpenGL.
3. **GLU**: Para utilidades como proyección y vista.
4. **gcc**: Para compilar el programa.
5. **Servidor X**: En caso de usar un contenedor Docker, para renderizar gráficos (p. ej., XLaunch).

### Configuración del Servidor X (XLaunch en Windows):
1. Instalar XLaunch desde [Xming](https://sourceforge.net/projects/xming/).
2. Iniciar XLaunch con las siguientes configuraciones:
   - Seleccionar "Multiple Windows".
   - Habilitar "No Access Control".
3. Identificar la dirección IP de la máquina anfitriona. Esta IP se usará para la variable `DISPLAY`.

### Configuración de variables de entorno:
1. Establecer las siguientes variables de entorno en el archivo `devcontainer.json` para facilitar la ejecución dentro del contenedor Docker:
   ```json
   {
       "containerEnv": {
           "LIBGL_ALWAYS_INDIRECT": "1",
           "DISPLAY": "172.23.48.1:1"
       }
   }
   ```
   Reemplazar `172.23.48.1:1` con la IP de la máquina anfitriona y el puerto correspondiente si es diferente.

---

## 🚀 **Ejecución**

1. Compilar el programa:
   ```bash
   make
   ```

2. Ejecutar el binario generado:
   ```bash
   ./main
   ```

3. Observar la simulación en la ventana gráfica. Usar los controles interactivos para explorar.

---

## 🎮 **Controles**

| Tecla          | Acción                                           |
|----------------|--------------------------------------------------|
| `↑ / ↓`       | Rotar la cámara hacia arriba o abajo             |
| `← / →`       | Rotar la cámara hacia izquierda o derecha        |
| `G`            | Activar/desactivar la gravedad                  |
| `ESC`          | Salir del programa                              |

---

## 🖥 **Descripción Técnica**

### **Código principal (`main.c`)**

#### **Inicialización**
- **SDL2 y OpenGL**: Configura SDL2 para usar un contexto OpenGL con un buffer doble y profundidad habilitada.
- **GLEW**: Permite usar extensiones modernas de OpenGL.
- **Iluminación básica**: Configura una luz puntual para resaltar las pelotas y el cubo.

#### **Física**
1. **Gravedad**:
   - Se aplica al eje Y si está habilitada.
   - La constante `GRAVITY` controla su intensidad.
2. **Colisiones**:
   - **Con las paredes**:
     - Si una pelota toca una pared, rebota invirtiendo su velocidad en ese eje.
   - **Entre pelotas**:
     - Detecta colisiones usando la distancia entre centros.
     - Calcula la dirección normal del impacto y ajusta velocidades de acuerdo con las leyes de conservación de momento lineal.

#### **Renderizado**
- **Cubo**:
  - Dibuja las aristas internas del cubo con líneas.
  - Configura una perspectiva utilizando `gluPerspective` y una vista de cámara con `gluLookAt`.
- **Pelotas**:
  - Renderiza cada pelota como una esfera coloreada.
  - Usa `drawSphere` para aproximar esferas con coordenadas esféricas.

---
