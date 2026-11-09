// Лабораторная работа 4. Движение камеры, матрицы, библиотека glm.
// цвет фигуры (0.5, 0.5, 1.0), цвет фона (1.0, 1.0, 0.3)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include "Shader.h"

// Параметры окна
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Камера 
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);    // позиция (точка)
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);   // направление взгляда (вектор)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);    // вектор "вверх"

float yaw = -90.0f;   // рыскание; -90, чтобы камера сначала смотрела вдоль -Z
float pitch = 0.0f;   // тангаж
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Время 
float deltaTime = 0.0f;   // время между кадрами
float lastFrame = 0.0f;

bool wireframe = false;   // режим каркаса (клавиша F)

// Реакция на движение мыши: меняем yaw и pitch, пересчитываем cameraFront
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {         // без этого при первом движении камера резко дёрнется
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;   // y в окне растёт вниз, поэтому разность наоборот
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;    // чтобы камера не переворачивалась
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Клавиши, которые срабатывают один раз при нажатии: Esc и F
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        wireframe = !wireframe;
}

// Движение камеры: WASD. Скорость умножается на deltaTime, чтобы не зависеть от FPS
void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

int main() {
    if (!glfwInit()) { fprintf(stderr, "ERROR: could not start GLFW3.\n"); return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LB4 Variant 14 - Cube", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum ret = glewInit();
    if (GLEW_OK != ret) { fprintf(stderr, "Error: %s\n", glewGetErrorString(ret)); return 1; }

    printf("OpenGL: %s\nDevice: %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    // Мышь и клавиши
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);   // курсор скрыт и закреплён в окне
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);   // тест глубины: ближние грани закрывают дальние

    // Геометрия куба: 6 граней по 4 вершины, у каждой грани своя яркость 
    const float corners[6][4][3] = {
        { { -0.5f,-0.5f, 0.5f },{ 0.5f,-0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f } },   // передняя (+Z)
        { { 0.5f,-0.5f,-0.5f },{ -0.5f,-0.5f,-0.5f },{ -0.5f, 0.5f,-0.5f },{ 0.5f, 0.5f,-0.5f } },   // задняя (-Z)
        { { -0.5f,-0.5f,-0.5f },{ -0.5f,-0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ -0.5f, 0.5f,-0.5f } },   // левая (-X)
        { { 0.5f,-0.5f, 0.5f },{ 0.5f,-0.5f,-0.5f },{ 0.5f, 0.5f,-0.5f },{ 0.5f, 0.5f, 0.5f } },   // правая (+X)
        { { -0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f,-0.5f },{ -0.5f, 0.5f,-0.5f } },   // верхняя (+Y)
        { { -0.5f,-0.5f,-0.5f },{ 0.5f,-0.5f,-0.5f },{ 0.5f,-0.5f, 0.5f },{ -0.5f,-0.5f, 0.5f } }    // нижняя (-Y)
    };
    const float shade[6] = { 0.85f, 0.5f, 0.65f, 0.75f, 1.0f, 0.4f };   // яркость граней
    const glm::vec3 baseColor(0.5f, 0.5f, 1.0f);                       // цвет фигуры варианта 14

    float vertices[24 * 6];            // x, y, z, r, g, b
    unsigned int indices[36];
    for (int f = 0; f < 6; f++) {
        for (int v = 0; v < 4; v++) {
            int i = (f * 4 + v) * 6;
            vertices[i + 0] = corners[f][v][0];
            vertices[i + 1] = corners[f][v][1];
            vertices[i + 2] = corners[f][v][2];
            vertices[i + 3] = baseColor.r * shade[f];
            vertices[i + 4] = baseColor.g * shade[f];
            vertices[i + 5] = baseColor.b * shade[f];
        }
        unsigned int b = f * 4;
        unsigned int idx[6] = { b, b + 1, b + 2, b + 2, b + 3, b };
        for (int k = 0; k < 6; k++) indices[f * 6 + k] = idx[k];
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);                    // позиция
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));  // цвет
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader shader("shaders/shader.vert", "shaders/shader.frag");

    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w == 0 || h == 0) { glfwPollEvents(); continue; }   // окно свёрнуто
        glViewport(0, 0, w, h);

        glClearColor(1.0f, 1.0f, 0.3f, 1.0f);                    // фон варианта 14
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      // очищаем цвет и глубину
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        // Матрицы
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);   // положение объекта в мире (в начале координат)
        glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));   // фиксированный поворот куба
        //glm::mat4 transform = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));  // Вращение куба
        shader.use();
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("transform", glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);   // 6 граней x 2 треугольника x 3 индекса

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shader.remove();
    glfwTerminate();
    return 0;
}