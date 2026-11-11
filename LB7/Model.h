// Model.h: вся модель целиком.
// Загружает файл через Assimp и превращает его в набор объектов Mesh.
#pragma once
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <iostream>
#include <functional>
#include "Mesh.h"
#include "Shader.h"

class Model {
public:
    std::vector<Mesh> meshes;
    std::string directory;

    Model(std::string const& path) {
        loadModel(path);
    }

    // Обычная отрисовка всей модели одной матрицей (как раньше)
    void Draw(Shader& shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

    // Отрисовка по частям: перед каждой сеткой вызывается beforeDraw(имя),
    // где в основной программе задаётся матрица этой детали и уходит в шейдер
    void Draw(Shader& shader, const std::function<void(const std::string&)>& beforeDraw) {
        for (unsigned int i = 0; i < meshes.size(); i++) {
            beforeDraw(meshes[i].name);
            meshes[i].Draw(shader);
        }
    }

private:
    void loadModel(std::string const& path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path,
            aiProcess_Triangulate | aiProcess_GenSmoothNormals);

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }

        size_t slash = path.find_last_of("/\\");
        directory = (slash == std::string::npos) ? "" : path.substr(0, slash);

        processNode(scene->mRootNode, scene);

        // Отладка: какие меши увидел Assimp и их габариты (min / max по осям)
        std::cout << "Meshes loaded: " << meshes.size() << std::endl;
        for (unsigned int i = 0; i < meshes.size(); i++) {
            glm::vec3 mn(1e9f), mx(-1e9f);
            for (const Vertex& v : meshes[i].vertices) {
                mn = glm::min(mn, v.Position);
                mx = glm::max(mx, v.Position);
            }
            std::cout << "  [" << i << "] " << meshes[i].name
                << "  min(" << mn.x << ", " << mn.y << ", " << mn.z << ")"
                << "  max(" << mx.x << ", " << mx.y << ", " << mx.z << ")" << std::endl;
        }
    }
    void processNode(aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene);
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
        (void)scene;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

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

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        return Mesh(vertices, indices, mesh->mName.C_Str());
    }
};