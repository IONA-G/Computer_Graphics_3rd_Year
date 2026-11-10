// Model.h: вся модель целиком.
// Загружает файл через Assimp и превращает его в набор объектов Mesh.
#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>     // класс Importer: читает файл
#include <assimp/scene.h>          // структуры aiScene, aiNode, aiMesh
#include <assimp/postprocess.h>    // флаги обработки (Triangulate и др.)
#include <string>
#include <vector>
#include <iostream>
#include "Mesh.h"
#include "Shader.h"

class Model {
public:
    std::vector<Mesh> meshes;      // все сетки модели
    std::string directory;         // папка, в которой лежит файл модели

    // Конструктор: сразу загружает модель по пути
    Model(std::string const& path) {
        loadModel(path);
    }

    // Рисуем все сетки по очереди
    void Draw(Shader& shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // Шаг 1 загрузить файл в объект сцены Assimp
    void loadModel(std::string const& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate |        // всё разбить на треугольники
            aiProcess_GenSmoothNormals);   // создать нормали, если их нет в файле

        // Проверка: файл открылся, сцена целая, есть корневой узел
        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        // Запоминаем папку модели
        size_t slash = path.find_last_of("/\\");
        directory = (slash == std::string::npos) ? "" : path.substr(0, slash);

        // Шаг 2: рекурсивно обходим узлы, начиная с корневого
        processNode(scene->mRootNode, scene);
    }

    // Узел содержит индексы мешей из сцены и ссылки на дочерние узлы
    void processNode(aiNode* node, const aiScene* scene) {
        // Все меши этого узла
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        // Затем то же самое для каждого дочернего узла
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    // Шаг 3 перенести данные из aiMesh в наш Mesh
    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        (void)scene;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        // Вершины: позиция и  нормаль
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z);
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[i].x,
                    mesh->mNormals[i].y,
                    mesh->mNormals[i].z);
            else
                vertex.Normal = glm::vec3(0.0f);
            vertices.push_back(vertex);
        }

        // Индексы каждая грань (face) состоит из индексов своих вершин
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return Mesh(vertices, indices);
    }
};