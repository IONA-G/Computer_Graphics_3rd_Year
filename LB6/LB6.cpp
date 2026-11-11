// Лабораторная работа 6. Настройка освещения с использованием шейдерных программ.
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

// Камера
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Время
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool wireframe = false;   // режим каркаса (клавиша F)

// Положение источника света в мировых координатах
glm::vec3 lightPos = glm::vec3(3.0f, 4.0f, 3.0f);

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LB6 Variant 14 - Phong", NULL, NULL);
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

        processInput(window);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        if (w == 0 || h == 0) { glfwPollEvents(); continue; }
        glViewport(0, 0, w, h);

        glClearColor(1.0f, 1.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

        // Матрицы
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);
        // Нормальная матрица: корректно преобразует нормали при любых трансформациях модели
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

        shader.use();
        shader.setMat4("projection", glm::value_ptr(projection));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat3("normalMatrix", normalMatrix);

        // Камера и свет
        shader.setVec3("viewPos", cameraPos);
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", glm::vec3(0.2f));     // слабый фоновый свет
        shader.setVec3("light.diffuse", glm::vec3(0.8f));     // основной белый свет
        shader.setVec3("light.specular", glm::vec3(1.0f));    // блики с полной силой

        // Материал: цвет фигуры варианта 14 (0.5, 0.5, 1.0)
        shader.setVec3("material.ambient", 0.5f, 0.5f, 1.0f);
        shader.setVec3("material.diffuse", 0.5f, 0.5f, 1.0f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);

        robot.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.remove();
    glfwTerminate();
    return 0;
}