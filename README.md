# Computer Graphics Labs OpenGL

Репозиторий хранит в себе лабораторные работы по дисциплине «Компьютерная графика»

## Технологии

- Visual Studio, платформа x64
- OpenGL 4.6 Core Profile
- GLFW, GLEW, GLM, Assimp
- Blender (модель робота «Циклон-5»)

## Лабораторные работы

| № | Тема | Описание и результат |
|---|---|---|
| 1 | Настройка Visual Studio, окно и контекст, шестиугольник на OpenGL 1.0 (`glBegin` / `glEnd`) | [LB1/README.md](LB1/README.md) |
| 2 | VAO, VBO, EBO, шейдеры, `uniform`, класс `Shader` | [LB2/README.md](LB2/README.md) |
| 3 | 3D-модель робота «Циклон-5» в Blender | [Assets/Blender_model_Cyclone-5/README.md](Assets/Blender_model_Cyclone-5/README.md) |
| 4 | Камера и матрицы, библиотека GLM | [LB4/README.md](LB4/README.md) |
| 5 | Библиотека Assimp, импорт модели `.obj` | [LB5/README.md](LB5/README.md) |
| 6 | Освещение по модели Фонга (ambient, diffuse, specular) | [LB6/README.md](LB6/README.md) |
| 7 | Движение через аффинные преобразования: управление звеньями робота с клавиатуры | [LB7/README.md](LB7/README.md) |

## Структура репозитория

```
Computer_Graphics_3rd_Year/
├── Gorbenko_A.V_ASUbz_24-1.slnx   решение Visual Studio (все проекты в одном решении)
├── glew-2.3.1/                    GLEW
├── glfw-3.5.1.bin.WIN64/          GLFW (бинарники для Windows)
├── glm-X.Y.Z/                     GLM (только заголовочные файлы)
├── Assimp/                        Assimp (include, lib/x64, bin/x64)
├── LB1/ … LB7/                    проекты лабораторных работ
└── Assets/
    └── Blender_model_Cyclone-5/   модель робота (.blend, .obj, .mtl, изображения)
```

Библиотеки лежат в корне репозитория один раз и подключаются ко всем проектам через `$(SolutionDir)`.

## Как собрать и запустить

1. Склонировать репозиторий:
   ```
   git clone https://github.com/IONA-G/Computer_Graphics_3rd_Year.git
   ```
2. Открыть решение `Gorbenko_A.V_ASUbz_24-1.slnx` в Visual Studio.
3. Выбрать платформу **x64**, нужный проект сделать запускаемым (ПКМ → **Set as Startup Project**).
4. Собрать и запустить (**Ctrl+F5**).

Проекты ЛБ5–ЛБ7 используют Assimp: после сборки `assimp-vc145-mt.dll` копируется рядом с `.exe` автоматически (Post-Build Event). Модель загружается из `Assets/Blender_model_Cyclone-5/cyclone-5.obj`, поэтому рабочая папка проекта должна быть `$(ProjectDir)`.

## Результаты

- **ЛБ4–ЛБ7:** управление камерой — **W / A / S / D** и мышь, **F** переключает режим каркаса, **Esc** — выход.
- **ЛБ7:** управление моделью — **J / L** (поворот станины), **I / K** (подъём), **T / G** и **Y / H** (выдвижение рук), **Z / X** и **C / V** (клешни). Подробности в [LB7/README.md](LB7/README.md).
