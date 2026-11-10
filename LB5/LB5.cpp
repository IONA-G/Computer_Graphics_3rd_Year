// Лабораторная работа 5. Подключение и работа с библиотекой ASSIMP. Импорт модели.
// цвет фигуры (0.5, 0.5, 1.0), цвет фона (1.0, 1.0, 0.3)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"

// Параметры окна
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Камера (отодвинута назад, чтобы модель попала в кадр)
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);     // позиция
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);  // куда смотрит
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);      // где "верх"

float yaw = -90.0f;     // поворот влево-вправо (-90, чтобы смотреть вдоль -Z)
float pitch = 0.0f;     // поворот вверх-вниз
float lastX = SCR_WIDTH / 2.0f;   // прошлая позиция мыши
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;           // первое движение мыши

// Время
float deltaTime = 0.0f;   // время между кадрами
float lastFrame = 0.0f;

bool wireframe = false;   // режим каркаса (клавиша F)

// Движение мыши: меняем yaw и pitch и пересчитываем направление взгляда
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {   // чтобы камера не дёрнулась при первом движении
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;   // y в окне растёт вниз, поэтому наоборот
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;       // чувствительность мыши
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;    // ограничение, чтобы камера не переворачивалась
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Клавиши с одним срабатыванием: Esc (выход) и F (каркас)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        wireframe = !wireframe;
}

// Движение камеры клавишами WASD (скорость не зависит от FPS)
void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;   // вперёд
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;   // назад
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;   // влево
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;   // вправо
}

int main() {
    // Запуск GLFW и настройка контекста OpenGL 4.6 Core
    if (!glfwInit()) { fprintf(stderr, "ERROR: could not start GLFW3.\n"); return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LB5 Variant 14 - Assimp", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);   // контекст делаем текущим до glewInit

    // Загрузка функций OpenGL через GLEW
    glewExperimental = GL_TRUE;
    GLenum ret = glewInit();
    if (GLEW_OK != ret) { fprintf(stderr, "Error: %s\n", glewGetErrorString(ret)); return 1; }

    printf("OpenGL: %s\nDevice: %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    // Мышь и клавиши
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // курсор скрыт
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);   // тест глубины: ближние части закрывают дальние

    // Шейдеры и модель
    Shader shader("shaders/model.vert", "shaders/model.frag");
    Model robot("../Assets/Blender_model_Cyclone-5/cyclone-5.obj");   // путь от папки LB5

    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        // Время между кадрами
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Размер окна (на случай изменения)
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w == 0 || h == 0) { glfwPollEvents(); continue; }   // окно свёрнуто
        glViewport(0, 0, w, h);

        glClearColor(1.0f, 1.0f, 0.3f, 1.0f);                   // цвет фона варианта 14
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // очищаем цвет и глубину
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);   // каркас или заливка

        // Матрицы: перспектива, камера, положение модели
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);   // модель стоит в начале координат

        // Передаём всё в шейдер
        shader.use();
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("model", glm::value_ptr(model));
        shader.setVec3("lightColor", 0.5f, 0.5f, 1.0f);   // цвет фигуры варианта 14

        robot.Draw(shader);   // рисуем модель

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.remove();   // удаляем шейдер до glfwTerminate
    glfwTerminate();
    return 0;
}