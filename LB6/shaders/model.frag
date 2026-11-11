#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

// Свойства материала поверхности
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;   // "резкость" блика
};

// Свойства источника света
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;      // положение камеры

void main()
{
    // 1. Окружающее освещение
    vec3 ambient = light.ambient * material.ambient;

    // 2. Диффузное освещение: зависит от угла между нормалью и направлением на свет
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // 3. Зеркальное освещение: блик зависит от угла между отражённым лучом и взглядом
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // Итоговый цвет — сумма трёх компонентов
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}