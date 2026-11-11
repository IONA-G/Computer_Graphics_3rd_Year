#version 460 core
layout (location = 0) in vec3 aPos;      // позиция вершины
layout (location = 1) in vec3 aNormal;   // нормаль вершины

out vec3 FragPos;   // положение точки в мировых координатах (для освещения)
out vec3 Normal;    // нормаль в мировых координатах

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;   // transpose(inverse(mat3(model)))

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}