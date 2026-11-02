
// Лабораторная работа 2.Создание рендера с использованием VBO, VAO и EBO.
// Вариант 14 Шестиугольник Цвет фигуры (0.5,0.5,1.0) Цвет фона (1.0,1.0,0.3)
// GLEW подключается первым: он загружает функции OpenGL из драйвера.
// Макрос GLEW_STATIC (стоит в настройках проекта), GLEW собран прямо в программу.
#include "GL/glew.h"
// GLFW: создаёт окно и контекст OpenGL, обрабатывает события окна.
#include "GLFW/glfw3.h"
#include <cstdio>   // вывод сообщений printf, fprintf
#include <cmath>    // cosf, sinf для расчёта вершин

// Вершинный шейдер: просто передаёт позицию дальше
const char* vertex_shader = R"(#version 410 core
layout(location = 0) in vec3 vp;
void main() {
    gl_Position = vec4(vp, 1.0);
})";

// Фрагментный шейдер: цвет приходит снаружи через uniform
const char* fragment_shader = R"(#version 410 core
uniform vec4 ourColor;
out vec4 frag_colour;
void main() {
    frag_colour = ourColor;
})";

int main() {
    // ИНИЦИАЛИЗАЦИЯ GLFW 
    // glfwInit() запускает библиотеку. Если не удалось, выводим ошибку и выходим.
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    // ВЕРСИЯ OpenGL
    // Задаётся ДО создания окна. Запрашиваем OpenGL 4.6 профиль Core.
    // строки раскомментированы, поэтому glBegin/glEnd больше не работают
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // FORWARD_COMPAT убирает устаревшие функции, CORE_PROFILE оставляет только современные.
    // С ними glBegin/glEnd перестали бы работать.
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // СОЗДАНИЕ ОКНА И КОНТЕКСТА
    // Окно 512x512 пикселей с заголовком "LB2 Variant 14".
    // Два последних NULL: не полноэкранный режим и без общего контекста.
    GLFWwindow* window = glfwCreateWindow(512, 512, "LB2 Variant 14", NULL, NULL);
    if (!window) {              // окно не создалось: закрываем GLFW и выходим
        glfwTerminate();
        return -1;
    }
    // Делаем контекст окна текущим: все команды gl... теперь относятся к нему.
    glfwMakeContextCurrent(window);

    // ИНИЦИАЛИЗАЦИЯ GLEW 
    // Флаг нужен, чтобы GLEW корректно загружал функции в современных контекстах.
    glewExperimental = GL_TRUE;
    // glewInit() должен вызываться ТОЛЬКО после glfwMakeContextCurrent.
    GLenum ret = glewInit();
    if (GLEW_OK != ret) {       // при ошибке выводим её текст
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        return 1;
    }

    // ИНФОРМАЦИЯ О ВЕРСИИ И ВИДЕОКАРТЕ
    const GLubyte* version_str = glGetString(GL_VERSION);   // версия OpenGL
    const GLubyte* device_str = glGetString(GL_RENDERER);   // название видеокарты
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("This version OpenGL running is %s\n", version_str);
    printf("This device OpenGL running is %s\n", device_str);

    // ДАННЫЕ ШЕСТИУГОЛЬНИКА: 7 вершин (центр и 6 по окружности радиуса 0.5)
    float points[] = {
         0.0f,   0.0f,   0.0f,   // 0: центр
         0.5f,   0.0f,   0.0f,   // 1
         0.25f,  0.433f, 0.0f,   // 2
        -0.25f,  0.433f, 0.0f,   // 3
        -0.5f,   0.0f,   0.0f,   // 4
        -0.25f, -0.433f, 0.0f,   // 5
         0.25f, -0.433f, 0.0f    // 6
    };
    // Индексы: 6 треугольников, у каждого общая вершина 0 (центр). Счёт с нуля!
    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1
    };

    // VAO, VBO, EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);                                   // всё дальнейшее запоминает VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO);                       // вершины в видеопамять
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);               // индексы в видеопамять
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // атрибут 0: 3 float на вершину, шаг 3 float, смещение 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);                         // отвязываем VBO
    glBindVertexArray(0);                                     // отвязываем VAO (EBO до этого не отвязываем!)

    // КОМПИЛЯЦИЯ ШЕЙДЕРОВ И СБОРКА ПРОГРАММЫ
    GLint success;
    GLchar infoLog[1024];

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 1024, NULL, infoLog);
        fprintf(stderr, "Ошибка вершинного шейдера:\n%s\n", infoLog);
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs, 1024, NULL, infoLog);
        fprintf(stderr, "Ошибка фрагментного шейдера:\n%s\n", infoLog);
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs);
    glAttachShader(shader_program, fs);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 1024, NULL, infoLog);
        fprintf(stderr, "Ошибка линковки программы:\n%s\n", infoLog);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);

    // место uniform-переменной в шейдере (ищем один раз, до цикла)
    GLint colorLocation = glGetUniformLocation(shader_program, "ourColor");

    // ОСНОВНОЙ ЦИКЛ
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 0.3f, 1.0f);   // фон варианта 14
        glClear(GL_COLOR_BUFFER_BIT);

        // Цвет зависит от времени: у каждого канала свой сдвиг фазы, значения 0..1
        // Градиен 
        float t = (float)glfwGetTime();
        float r = sinf(t) * 0.5f + 0.5f;
        float g = sinf(t + 2.0f) * 0.5f + 0.5f;
        float b = sinf(t + 4.0f) * 0.5f + 0.5f;

        glUseProgram(shader_program);                      // сначала включаем программу,
        glUniform4f(colorLocation, r, g, b, 1.0f);         // и только потом передаём uniform
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);   // 6 треугольников x 3 индекса

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ОЧИСТКА
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);
    glfwTerminate();
    return 0;
}
