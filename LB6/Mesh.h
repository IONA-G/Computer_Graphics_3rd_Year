// Mesh.h: одна сетка
// Хранит вершины и индексы и отвечает за их отправку в видеопамять и отрисовку.
#pragma once
#include "GL/glew.h"        // GLEW подключается раньше остального OpenGL
#include <glm/glm.hpp>
#include <vector>
#include <cstddef>          // для offsetof
#include "Shader.h"

// Одна вершина что именно мы храним для каждой точки модели
struct Vertex {
    glm::vec3 Position;     // координаты (x, y, z)
    glm::vec3 Normal;       // вектор нормали (понадобится для освещения)
};

class Mesh {
public:
    std::vector<Vertex> vertices;        // все вершины сетки
    std::vector<unsigned int> indices;   // индексы: по каким вершинам строятся треугольники
    unsigned int VAO = 0;                // объект, который помнит настройку атрибутов

    // Конструктор: запоминает данные и сразу настраивает буферы
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        this->vertices = vertices;
        this->indices = indices;
        setupMesh();
    }

    // Отрисовка сетки. Шейдер включается и настраивается снаружи (use и uniform-ы),
    // параметр оставлен, чтобы совпадать с методичкой.
    void Draw(Shader& shader) {
        (void)shader;
        glBindVertexArray(VAO);                                  // выбираем сетку
        glDrawElements(GL_TRIANGLES,                             // рисуем треугольниками
            static_cast<unsigned int>(indices.size()),           // сколько индексов
            GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);                                    // сбрасываем выбор
    }

private:
    unsigned int VBO = 0, EBO = 0;   // буфер вершин и буфер индексов

    void setupMesh() {
        // Создаём VAO, VBO и EBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // Отправляем вершины в видеопамять (целиком структуру Vertex)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // Отправляем индексы
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Атрибут 0 (location = 0 в шейдере) позиция, 3 float, начало структуры
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        // Атрибут 1 (location = 1 в шейдере) нормаль, смещение до поля Normal
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }
};