#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Constantes de la simulación
#define NUM_BALLS 5
#define CUBE_SIZE 2.0f // Cubo de [-1,1] en x,y,z
#define GRAVITY -0.8f  // Gravedad opcional en Y
#define ELASTICITY 1.0f

// Control de tiempo (FPS)
#define FPS 60
#define FRAME_DELAY (1000 / FPS)

// Estructura para una pelota
typedef struct
{
    float x, y, z;    // Posición
    float vx, vy, vz; // Velocidad
    float r;          // Radio de la pelota
    float color[3];   // Color (RGB)
} Ball;

// Variables globales
static Ball balls[NUM_BALLS];
static int running = 1;

// Variables de cámara
static float cameraRotX = 20.0f;
static float cameraRotY = -30.0f;

// Gravedad habilitada o no
static int useGravity = 1;

// Función para inicializar las pelotas con valores aleatorios
void initBalls()
{
    srand((unsigned int)time(NULL));
    for (int i = 0; i < NUM_BALLS; i++)
    {
        balls[i].r = 0.1f + (float)rand() / RAND_MAX * 0.1f; // radio entre 0.1 y 0.2
        balls[i].x = ((float)rand() / RAND_MAX - 0.5f) * (CUBE_SIZE - 2.0f * balls[i].r);
        balls[i].y = ((float)rand() / RAND_MAX - 0.5f) * (CUBE_SIZE - 2.0f * balls[i].r);
        balls[i].z = ((float)rand() / RAND_MAX - 0.5f) * (CUBE_SIZE - 2.0f * balls[i].r);

        balls[i].vx = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;
        balls[i].vy = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;
        balls[i].vz = ((float)rand() / RAND_MAX - 0.5f) * 0.5f;

        balls[i].color[0] = (float)rand() / RAND_MAX;
        balls[i].color[1] = (float)rand() / RAND_MAX;
        balls[i].color[2] = (float)rand() / RAND_MAX;
    }
}

// Función para dibujar una esfera (utilizando glut-like, pero lo haremos a mano)
void drawSphere(float r, int lats, int longs)
{
    // Aproximación de esfera usando coordenadas esféricas
    // Para simplificar, usar immediate mode
    for (int i = 0; i <= lats; i++)
    {
        float lat0 = M_PI * (-0.5f + (float)(i - 1) / lats);
        float z0 = sin(lat0);
        float zr0 = cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)i / lats);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= longs; j++)
        {
            float lng = 2 * M_PI * (float)(j - 1) / longs;
            float x = cos(lng);
            float y = sin(lng);

            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(r * x * zr0, r * y * zr0, r * z0);

            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(r * x * zr1, r * y * zr1, r * z1);
        }
        glEnd();
    }
}

// Función para dibujar el cubo (solo las aristas internas)
void drawCube(float size)
{
    float half = size / 2.0f;
    glBegin(GL_LINES);
    // 12 aristas
    glVertex3f(-half, -half, -half);
    glVertex3f(-half, -half, half);
    glVertex3f(-half, -half, -half);
    glVertex3f(-half, half, -half);
    glVertex3f(-half, -half, -half);
    glVertex3f(half, -half, -half);

    glVertex3f(half, half, half);
    glVertex3f(half, half, -half);
    glVertex3f(half, half, half);
    glVertex3f(half, -half, half);
    glVertex3f(half, half, half);
    glVertex3f(-half, half, half);

    glVertex3f(-half, half, -half);
    glVertex3f(half, half, -half);
    glVertex3f(-half, half, -half);
    glVertex3f(-half, half, half);

    glVertex3f(-half, -half, half);
    glVertex3f(half, -half, half);
    glVertex3f(-half, -half, half);
    glVertex3f(-half, half, half);

    glVertex3f(half, -half, -half);
    glVertex3f(half, -half, half);
    glVertex3f(half, -half, -half);
    glVertex3f(half, half, -half);
    glEnd();
}

// Detecta colisión entre pelotas y ajusta velocidades
void handleBallCollisions()
{
    for (int i = 0; i < NUM_BALLS; i++)
    {
        for (int j = i + 1; j < NUM_BALLS; j++)
        {
            float dx = balls[j].x - balls[i].x;
            float dy = balls[j].y - balls[i].y;
            float dz = balls[j].z - balls[i].z;
            float dist = sqrtf(dx * dx + dy * dy + dz * dz);
            float sumR = balls[i].r + balls[j].r;

            if (dist < sumR && dist > 0.0f)
            {
                // Colisión
                // Vector normal
                float nx = dx / dist;
                float ny = dy / dist;
                float nz = dz / dist;

                // Velocidad relativa
                float dvx = balls[j].vx - balls[i].vx;
                float dvy = balls[j].vy - balls[i].vy;
                float dvz = balls[j].vz - balls[i].vz;

                // Impulso a lo largo de la normal
                float dot = dvx * nx + dvy * ny + dvz * nz;

                // Si se están separando no hacer nada
                if (dot > 0)
                    continue;

                float mass = 1.0f; // Asumimos masas iguales
                float jImpulse = -(1.0f + ELASTICITY) * dot / (1 / mass + 1 / mass);
                float jx = jImpulse * nx;
                float jy = jImpulse * ny;
                float jz = jImpulse * nz;

                balls[i].vx -= jx / mass;
                balls[i].vy -= jy / mass;
                balls[i].vz -= jz / mass;

                balls[j].vx += jx / mass;
                balls[j].vy += jy / mass;
                balls[j].vz += jz / mass;

                // Ajustar posiciones para evitar superposición
                float overlap = (sumR - dist) * 0.5f;
                balls[i].x -= nx * overlap;
                balls[i].y -= ny * overlap;
                balls[i].z -= nz * overlap;

                balls[j].x += nx * overlap;
                balls[j].y += ny * overlap;
                balls[j].z += nz * overlap;
            }
        }
    }
}

// Actualiza posiciones y maneja colisiones con paredes
void updateBalls(float dt)
{
    for (int i = 0; i < NUM_BALLS; i++)
    {
        // Aplicar gravedad
        if (useGravity)
        {
            balls[i].vy += GRAVITY * dt;
        }

        // Mover
        balls[i].x += balls[i].vx * dt;
        balls[i].y += balls[i].vy * dt;
        balls[i].z += balls[i].vz * dt;

        float half = CUBE_SIZE / 2.0f;

        // Colisión con paredes
        // Eje X
        if (balls[i].x - balls[i].r < -half)
        {
            balls[i].x = -half + balls[i].r;
            balls[i].vx = -balls[i].vx;
        }
        if (balls[i].x + balls[i].r > half)
        {
            balls[i].x = half - balls[i].r;
            balls[i].vx = -balls[i].vx;
        }

        // Eje Y
        if (balls[i].y - balls[i].r < -half)
        {
            balls[i].y = -half + balls[i].r;
            balls[i].vy = -balls[i].vy;
        }
        if (balls[i].y + balls[i].r > half)
        {
            balls[i].y = half - balls[i].r;
            balls[i].vy = -balls[i].vy;
        }

        // Eje Z
        if (balls[i].z - balls[i].r < -half)
        {
            balls[i].z = -half + balls[i].r;
            balls[i].vz = -balls[i].vz;
        }
        if (balls[i].z + balls[i].r > half)
        {
            balls[i].z = half - balls[i].r;
            balls[i].vz = -balls[i].vz;
        }
    }

    // Colisiones entre pelotas
    handleBallCollisions();
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window *window = SDL_CreateWindow("Simulacion Pelotas en Cubo 3D",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window)
    {
        fprintf(stderr, "No se pudo crear la ventana: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext)
    {
        fprintf(stderr, "No se pudo crear el contexto OpenGL: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Error al inicializar GLEW\n");
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Ajustes de luz
    GLfloat lightPos[] = {1.0f, 1.0f, 2.0f, 0.0f};
    GLfloat lightAmb[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat lightDiff[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);

    initBalls();

    Uint32 lastTime = SDL_GetTicks();

    while (running)
    {
        Uint32 frameStart = SDL_GetTicks();
        float dt = (frameStart - lastTime) / 1000.0f;
        lastTime = frameStart;

        // Manejo de eventos
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = 0;
                }
                if (event.key.keysym.sym == SDLK_UP)
                {
                    cameraRotX -= 5.0f;
                }
                if (event.key.keysym.sym == SDLK_DOWN)
                {
                    cameraRotX += 5.0f;
                }
                if (event.key.keysym.sym == SDLK_LEFT)
                {
                    cameraRotY -= 5.0f;
                }
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    cameraRotY += 5.0f;
                }
                if (event.key.keysym.sym == SDLK_g)
                {
                    useGravity = !useGravity;
                }
            }
        }

        // Actualizar lógica
        updateBalls(dt);

        // Renderizar
        glViewport(0, 0, 800, 600);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = 800.0f / 600.0f;
        gluPerspective(60.0, aspect, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gluLookAt(0.0, 0.0, 5.0,
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);

        glRotatef(cameraRotX, 1.0f, 0.0f, 0.0f);
        glRotatef(cameraRotY, 0.0f, 1.0f, 0.0f);

        // Dibujar cubo contenedor
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawCube(CUBE_SIZE);
        glEnable(GL_LIGHTING);

        // Dibujar pelotas
        for (int i = 0; i < NUM_BALLS; i++)
        {
            glPushMatrix();
            glTranslatef(balls[i].x, balls[i].y, balls[i].z);
            glColor3f(balls[i].color[0], balls[i].color[1], balls[i].color[2]);
            drawSphere(balls[i].r, 16, 16);
            glPopMatrix();
        }

        SDL_GL_SwapWindow(window);

        // Control de FPS
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (FRAME_DELAY > frameTime)
        {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
