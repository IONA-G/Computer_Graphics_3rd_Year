// Мини-библиотека для работы с шейдерными программами.
// Загружает шейдеры из файлов, компилирует, проверяет ошибки,
// и умеет отправлять uniform-переменные одной строкой.
#pragma once
#include "GL/glew.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    GLuint ID = 0;   // номер шейдерной программы

    // Читает два файла, компилирует шейдеры и собирает программу
    Shader(const char* vertexPath, const char* fragmentPath) {
        std::string vCode = readFile(vertexPath);
        std::string fCode = readFile(fragmentPath);

        GLuint vs = compile(GL_VERTEX_SHADER, vCode.c_str(), "VERTEX");
        GLuint fs = compile(GL_FRAGMENT_SHADER, fCode.c_str(), "FRAGMENT");

        ID = glCreateProgram();
        glAttachShader(ID, vs);
        glAttachShader(ID, fs);
        glLinkProgram(ID);

        GLint success;
        GLchar infoLog[1024];
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << "\n";
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    void use() const { glUseProgram(ID); }       // включить программу
    void remove() const { glDeleteProgram(ID); } // удалить (вызывать ДО glfwTerminate)

    // ---- uniform одной строкой. Программа должна быть включена через use() ----
    void setBool(const std::string& n, bool v) const { glUniform1i(loc(n), (int)v); }
    void setInt(const std::string& n, int v) const { glUniform1i(loc(n), v); }
    void setFloat(const std::string& n, float v) const { glUniform1f(loc(n), v); }
    void setVec2(const std::string& n, float x, float y) const { glUniform2f(loc(n), x, y); }
    void setVec3(const std::string& n, float x, float y, float z) const { glUniform3f(loc(n), x, y, z); }
    void setVec4(const std::string& n, float x, float y, float z, float w) const { glUniform4f(loc(n), x, y, z, w); }
    void setMat4(const std::string& n, const float* m) const { glUniformMatrix4fv(loc(n), 1, GL_FALSE, m); }

private:
    GLint loc(const std::string& n) const { return glGetUniformLocation(ID, n.c_str()); }

    static std::string readFile(const char* path) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "ERROR::SHADER::FILE_NOT_FOUND: " << path << "\n";
            return "";
        }
        std::stringstream ss;
        ss << file.rdbuf();
        std::string s = ss.str();
        // убираем BOM (метку кодировки UTF-8), если редактор её добавил
        if (s.size() >= 3 && (unsigned char)s[0] == 0xEF && (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF)
            s.erase(0, 3);
        return s;
    }

    static GLuint compile(GLenum type, const char* src, const char* name) {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, NULL);
        glCompileShader(s);
        GLint success;
        GLchar infoLog[1024];
        glGetShaderiv(s, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(s, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << name << "\n" << infoLog << "\n";
        }
        return s;
    }
};
