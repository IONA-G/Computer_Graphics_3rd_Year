#version 460 core
layout (location = 0) in vec3 aPos;      // позиция вершины (атрибут 0)
layout (location = 1) in vec3 aNormal;   // нормаль (атрибут 1), пока не используется

uniform mat4 model;        // положение модели в мире
uniform mat4 view;         // камера
uniform mat4 projection;   // перспектива

void main()
{
    // Цепочка преобразований: модель -> мир -> камера -> экран
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}