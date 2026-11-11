// Лабораторная работа 7. Создание движения через аффинные преобразования.
// Вариант 14: цвет фигуры (0.5, 0.5, 1.0), цвет фона (1.0, 1.0, 0.3)
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <string>
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"

// Параметры окна
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Камера (отодвинута и чуть наклонена вниз)
glm::vec3 cameraPos = glm::vec3(4.0f, 3.7f, -4.5f);
glm::vec3 cameraFront = glm::vec3(-0.621f, -0.392f, 0.678f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -227.5f;
float pitch = -23.1f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool wireframe = false;   // режим каркаса (клавиша F)

// Источник света
glm::vec3 lightPos = glm::vec3(4.0f, 6.0f, 5.0f);

// ---------------- Переменные движения ----------------
float baseAngle = 0.0f;   // 1) поворот модуля, градусы (J / L)
float liftY = 0.0f;       // 2) подъём модуля по вертикали, метры (I / K)
float ext1 = 0.0f;        // 3) выдвижение левой руки вперёд, метры (T / G)
float ext2 = 0.0f;        //    выдвижение правой руки вперёд, метры (Y / H)
float clawL = 0.0f;       // смыкание левой клешни, градусы (Z / X)
float clawR = 0.0f;       // смыкание правой клешни, градусы (C / V)

// Скорости
const float ANGLE_SPEED = 60.0f;   // град/с
const float LIFT_SPEED = 0.5f;     // м/с
const float EXT_SPEED = 0.6f;      // м/с
const float CLAW_SPEED = 30.0f;    // град/с

// Крайние положения (подбираются по модели)
const float LIFT_MIN = 0.0f, LIFT_MAX = 0.45f;   // подъём: вниз не ниже исходного положения
const float EXT_MIN = 0.0f, EXT_MAX = 0.8f;      // руки: назад не уходят, только вперёд
const float CLAW_MAX = 25.0f;                    // ход клешни в градусах

// Точки вращения (в координатах модели, ось Y вверх)
const glm::vec3 BASE_PIVOT = glm::vec3(0.0f, 0.0f, 0.0f);          // вертикальная ось поворота модуля
const glm::vec3 PIVOT_CLAW_1 = glm::vec3(-0.7525f, 2.553f, -1.19f);  // Claw_1 (левая, верхняя)
const glm::vec3 PIVOT_CLAW_2 = glm::vec3(-0.7527f, 2.425f, -1.19f);  // Claw_2 (левая, нижняя)
const glm::vec3 PIVOT_CLAW_11 = glm::vec3(0.682f, 2.485f, -1.1875f); // Claw_1_1 (правая, ближняя к центру)
const glm::vec3 PIVOT_CLAW_22 = glm::vec3(0.810f, 2.4866f, -1.1873f); // Claw_2_2 (правая, дальняя от центра)

// Поворот вокруг произвольной точки: сместить в начало, повернуть, вернуть обратно
glm::mat4 rotateAround(const glm::vec3& p, float angleDeg, const glm::vec3& axis) {
    glm::mat4 m = glm::mat4(1.0f);
    m = glm::translate(m, p);
    m = glm::rotate(m, glm::radians(angleDeg), axis);
    m = glm::translate(m, -p);
    return m;
}

// Матрица преобразования для детали по её имени (с учётом зависимостей звеньев)
glm::mat4 partMatrix(const std::string& name) {
    const glm::vec3 AX_X = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 AX_Y = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 rot = rotateAround(BASE_PIVOT, baseAngle, AX_Y);                       // 1-я степень
    glm::mat4 lift = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, liftY, 0.0f));  // 2-я степень
    // 3-я степень: рука выезжает вперёд (в сторону -Z), зависит от поворота и подъёма
    glm::mat4 hand1 = rot * lift * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -ext1));
    glm::mat4 hand2 = rot * lift * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -ext2));

    if (name == "Stationary_platform") return glm::mat4(1.0f);   // база неподвижна
    if (name == "Machine_bed")         return rot;
    if (name == "Piston_pump")         return rot * lift;
    if (name == "Hand_1")              return hand1;
    if (name == "Claw_1")              return hand1 * rotateAround(PIVOT_CLAW_1, clawL, AX_X);
    if (name == "Claw_2")              return hand1 * rotateAround(PIVOT_CLAW_2, -clawL, AX_X);
    if (name == "Hand_2")              return hand2;
    if (name == "Claw_1_1")            return hand2 * rotateAround(PIVOT_CLAW_11, clawR, AX_Y);
    if (name == "Claw_2_2")            return hand2 * rotateAround(PIVOT_CLAW_22, -clawR, AX_Y);
    return glm::mat4(1.0f);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)  pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        wireframe = !wireframe;
}

void processInput(GLFWwindow* window) {
    // Камера: WASD
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // 1) Поворот модуля: J влево, L вправо
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) baseAngle += ANGLE_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) baseAngle -= ANGLE_SPEED * deltaTime;
    baseAngle = glm::clamp(baseAngle, -140.0f, 140.0f);

    // 2) Подъём модуля: I вверх, K вниз (с ограничением)
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) liftY += LIFT_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) liftY -= LIFT_SPEED * deltaTime;
    liftY = glm::clamp(liftY, LIFT_MIN, LIFT_MAX);

    // 3) Выдвижение рук: левая T вперёд / G назад, правая Y вперёд / H назад
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) ext1 += EXT_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) ext1 -= EXT_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) ext2 += EXT_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) ext2 -= EXT_SPEED * deltaTime;
    ext1 = glm::clamp(ext1, EXT_MIN, EXT_MAX);
    ext2 = glm::clamp(ext2, EXT_MIN, EXT_MAX);

    // Клешни: левая Z смыкание / X размыкание, правая C смыкание / V размыкание
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) clawL += CLAW_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) clawL -= CLAW_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) clawR += CLAW_SPEED * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) clawR -= CLAW_SPEED * deltaTime;
    clawL = glm::clamp(clawL, 0.0f, CLAW_MAX);
    clawR = glm::clamp(clawR, 0.0f, CLAW_MAX);
}

int main() {
    if (!glfwInit()) { fprintf(stderr, "ERROR: could not start GLFW3.\n"); return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LB7 Variant 14 - Affine", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum ret = glewInit();
    if (GLEW_OK != ret) { fprintf(stderr, "Error: %s\n", glewGetErrorString(ret)); return 1; }

    printf("OpenGL: %s\nDevice: %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    Shader shader("shaders/model.vert", "shaders/model.frag");
    Model robot("../Assets/Blender_model_Cyclone-5/cyclone-5.obj");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Временно: позиция и углы камеры в заголовке окна
        //char title[300];
        //snprintf(title, sizeof(title),
        //    "cam (%.1f, %.1f, %.1f) yaw %.1f pitch %.1f",
        //    cameraPos.x, cameraPos.y, cameraPos.z, yaw, pitch);
        //glfwSetWindowTitle(window, title);

        processInput(window);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w == 0 || h == 0) { glfwPollEvents(); continue; }
        glViewport(0, 0, w, h);

        glClearColor(1.0f, 1.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        shader.use();
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setMat4("view", glm::value_ptr(view));

        // Камера и свет
        shader.setVec3("viewPos", cameraPos);
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", glm::vec3(0.2f));
        shader.setVec3("light.diffuse", glm::vec3(0.8f));
        shader.setVec3("light.specular", glm::vec3(1.0f));

        // Материал: цвет фигуры варианта 14
        shader.setVec3("material.ambient", 0.5f, 0.5f, 1.0f);
        shader.setVec3("material.diffuse", 0.5f, 0.5f, 1.0f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);

        // Отрисовка по частям: перед каждой деталью считаем её матрицу и отправляем в шейдер
        robot.Draw(shader, [&](const std::string& name) {
            glm::mat4 model = partMatrix(name);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
            shader.setMat4("model", glm::value_ptr(model));
            shader.setMat3("normalMatrix", normalMatrix);
            });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.remove();
    glfwTerminate();
    return 0;
}