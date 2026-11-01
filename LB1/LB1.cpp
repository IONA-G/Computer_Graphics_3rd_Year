// ПОДКЛЮЧЕНИЕ БИБЛИОТЕК
// Лабораторная работа 1. Настройка Visual Studio для работы с OpenGL. Создание контекста и рендера.
// Вариант 14 Шестиугольник Цвет фигуры (0.5,0.5,1.0) Цвет фона (1.0,1.0,0.3)
// GLEW подключается первым: он загружает функции OpenGL из драйвера.
// Макрос GLEW_STATIC (стоит в настройках проекта), GLEW собран прямо в программу.
#include "GL/glew.h"
// GLFW: создаёт окно и контекст OpenGL, обрабатывает события окна.
#include "GLFW/glfw3.h"
#include <cstdio>   // вывод сообщений printf, fprintf
#include <cmath>    // cosf, sinf для расчёта вершин

int main() {
    // ИНИЦИАЛИЗАЦИЯ GLFW 
    // glfwInit() запускает библиотеку. Если не удалось, выводим ошибку и выходим.
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }

    // ВЕРСИЯ OpenGL
    // Задаётся ДО создания окна. Запрашиваем OpenGL 1.0 (мажорная версия 1, минорная 0).
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // Для версии 1.0 эти две строки временно закомментированы:
    // FORWARD_COMPAT убирает устаревшие функции, CORE_PROFILE оставляет только современные.
    // С ними glBegin/glEnd перестали бы работать.
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // СОЗДАНИЕ ОКНА И КОНТЕКСТА
    // Окно 512x512 пикселей с заголовком "LB1 Variant 14".
    // Два последних NULL: не полноэкранный режим и без общего контекста.
    GLFWwindow* window = glfwCreateWindow(512, 512, "LB1 Variant 14", NULL, NULL);
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

    // ПАРАМЕТРЫ ШЕСТИУГОЛЬНИКА 
    const float PI = 3.14159265f;
    const float R = 0.5f;       // радиус: расстояние от центра до вершины (координаты от -1 до 1)

    // ОСНОВНОЙ ЦИКЛ ПРОГРАММЫ 
    // Выполняется, пока пользователь не закроет окно.
    while (!glfwWindowShouldClose(window)) {

        // a) Очистка старого кадра
        glClearColor(1.0f, 1.0f, 0.3f, 1.0f);   // цвет фона (r, g, b, прозрачность)
        glClear(GL_COLOR_BUFFER_BIT);           // залить экран этим цветом

        // b) Отрисовка нового кадра
        glBegin(GL_TRIANGLES);                  // начало списка вершин: каждые 3 вершины = один треугольник
        glColor3f(0.5f, 0.5f, 1.0f);            // цвет фигуры задаётся ДО вершин
        for (int i = 0; i < 6; i++) {           // шестиугольник = 6 треугольников
            float a1 = i * PI / 3.0f;           // угол первой вершины шаг 60°
            float a2 = (i + 1) * PI / 3.0f;     // угол следующей вершины
            glVertex2f(0.0f, 0.0f);                         // центр фигуры
            glVertex2f(R * cosf(a1), R * sinf(a1));         // вершина на окружности
            glVertex2f(R * cosf(a2), R * sinf(a2));         // соседняя вершина на окружности
        }
        glEnd();                                // конец списка вершин

        // c) Смена кадра: показываем нарисованное, прячем прошлый кадр (двойная буферизация)
        glfwSwapBuffers(window);

        // d) Обработка событий окна и устройств: мышь, клавиатура, закрытие, перемещение
        glfwPollEvents();
    }

    // ЗАВЕРШЕНИЕ РАБОТЫ
    // Закрываем окно, освобождаем ресурсы GLFW.
    glfwTerminate();
    return 0;
}