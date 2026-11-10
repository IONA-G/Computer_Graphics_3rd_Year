#version 460 core
out vec4 FragColor;        // итоговый цвет пикселя

uniform vec3 lightColor;   // цвет плоской закраски, приходит из программы

void main()
{
    FragColor = vec4(lightColor, 1.0);   // закрашиваем в один цвет, без освещения
}