// Mesh.h: одна сетка.
// Хранит вершины, индексы и имя детали, отвечает за отправку данных в видеопамять и отрисовку.
#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <cstddef>
#include "Shader.h"

// Одна вершина: позиция и нормаль
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string name;            // имя объекта из файла модели (например, Piston_pump)
    unsigned int VAO = 0;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::string name) {
        this->vertices = vertices;
        this->indices = indices;
        this->name = name;
        setupMesh();
    }

    void Draw(Shader& shader) {
        (void)shader;
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO = 0, EBO = 0;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // location 0: позиция
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // location 1: нормаль
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glBindVertexArray(0);
    }
};