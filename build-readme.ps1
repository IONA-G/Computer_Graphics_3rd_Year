# build-readme.ps1: собирает общий README.md из README всех лабораторных работ.
$utf8 = New-Object System.Text.UTF8Encoding $false

$header = @'
# Computer Graphics Labs OpenGL

Репозиторий с семью лабораторными работами по курсу «Компьютерная графика»: от окна и шестиугольника на OpenGL 1.0 до управляемой трёхмерной модели робота с освещением.

**Вариант:** 14 (шестиугольник, цвет фигуры (0.5, 0.5, 1.0), цвет фона (1.0, 1.0, 0.3))

**Технологии:** Visual Studio (x64), OpenGL 4.6 Core Profile, GLFW, GLEW, GLM, Assimp, Blender.

## Структура репозитория

```
Computer_Graphics_3rd_Year/
├── Gorbenko_A.V_ASUbz_24-1.slnx   решение Visual Studio (все проекты в одном решении)
├── README.md                      общий README (собирается скриптом build-readme.ps1)
├── build-readme.ps1               скрипт сборки общего README из README лабораторных
├── .gitignore
├── glew-2.3.1/                    GLEW (исходники, нужен src/glew.c)
├── glfw-3.5.1.bin.WIN64/          GLFW (готовые бинарники для Windows)
├── glm-X.Y.Z/                     GLM (только заголовочные файлы)
├── Assimp/                        Assimp (include, lib/x64, bin/x64)
├── LB1/ … LB7/                    проекты лабораторных работ
└── Assets/
    └── Blender_model_Cyclone-5/   модель робота: .blend, .obj, .mtl, png/, README.md
```

Библиотеки лежат в корне репозитория один раз и подключаются ко всем проектам через `$(SolutionDir)`, поэтому проекты собираются без установки чего-либо в систему.

## Состав проектов

| Проект | Основные файлы | Что делает |
|---|---|---|
| **LB1** | `LB1.cpp` | окно и контекст, шестиугольник на OpenGL 1.0 (`glBegin` / `glEnd`) |
| **LB2** | `LB2.cpp`, `Shader.h`, `shaders/` | VAO, VBO, EBO, шейдеры, `uniform`, класс `Shader` |
| **Assets** (ЛБ3) | `Cyclone-5.blend`, `cyclone-5.obj`, `Cyclone-5.mtl`, `png/` | 3D-модель робота «Циклон-5» в Blender, экспорт в `.obj` |
| **LB4** | `LB4.cpp`, `Shader.h`, `shaders/` | камера (WASD, мышь), матрицы, библиотека GLM |
| **LB5** | `LB5.cpp`, `Mesh.h`, `Model.h`, `Shader.h`, `shaders/` | импорт модели через Assimp, классы `Mesh` и `Model`, плоская заливка |
| **LB6** | `LB6.cpp`, `Mesh.h`, `Model.h`, `Shader.h`, `shaders/` | освещение по модели Фонга: ambient, diffuse, specular |
| **LB7** | `LB7.cpp`, `Mesh.h`, `Model.h`, `Shader.h`, `shaders/` | управление звеньями робота с клавиатуры: матрицы для каждой детали |

## Как собрать и запустить

1. `git clone https://github.com/IONA-G/Computer_Graphics_3rd_Year.git`
2. Открыть решение `Gorbenko_A.V_ASUbz_24-1.slnx` в Visual Studio.
3. Выбрать платформу **x64**, нужный проект сделать запускаемым (ПКМ, Set as Startup Project).
4. Собрать и запустить (Ctrl+F5).

'@

$labs = @(
    @{ Id = 'lb1'; Dir = 'LB1';                            Title = 'Лабораторная работа №1' },
    @{ Id = 'lb2'; Dir = 'LB2';                            Title = 'Лабораторная работа №2' },
    @{ Id = 'lb3'; Dir = 'Assets/Blender_model_Cyclone-5'; Title = 'Лабораторная работа №3' },
    @{ Id = 'lb4'; Dir = 'LB4';                            Title = 'Лабораторная работа №4' },
    @{ Id = 'lb5'; Dir = 'LB5';                            Title = 'Лабораторная работа №5' },
    @{ Id = 'lb6'; Dir = 'LB6';                            Title = 'Лабораторная работа №6' },
    @{ Id = 'lb7'; Dir = 'LB7';                            Title = 'Лабораторная работа №7' }
)

$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine($header)

# Оглавление
[void]$sb.AppendLine('## Содержание')
[void]$sb.AppendLine('')
foreach ($lab in $labs) {
    [void]$sb.AppendLine("- [$($lab.Title)](#$($lab.Id))")
}
[void]$sb.AppendLine('')

# Тексты лабораторных
foreach ($lab in $labs) {
    $path = Join-Path $lab.Dir 'README.md'
    if (-not (Test-Path $path)) {
        Write-Host "ВНИМАНИЕ: не найден $path" -ForegroundColor Yellow
        continue
    }
    $text = [System.IO.File]::ReadAllText($path, [System.Text.Encoding]::UTF8)
    $prefix = $lab.Dir + '/'

    # Пути к картинкам и ссылкам в markdown: ](файл) -> ](папка/файл)
    $text = [regex]::Replace($text, '\]\((?!https?://|#|/|mailto:)([^)\s]+)',
        [System.Text.RegularExpressions.MatchEvaluator]{ param($m) '](' + $prefix + $m.Groups[1].Value })
    # Пути в HTML: src="файл" -> src="папка/файл"
    $text = [regex]::Replace($text, 'src="(?!https?://|/)([^"]+)"',
        [System.Text.RegularExpressions.MatchEvaluator]{ param($m) 'src="' + $prefix + $m.Groups[1].Value + '"' })

    [void]$sb.AppendLine("<a id=`"$($lab.Id)`"></a>")
    [void]$sb.AppendLine('')
    [void]$sb.AppendLine($text.TrimEnd())
    [void]$sb.AppendLine('')
    [void]$sb.AppendLine('---')
    [void]$sb.AppendLine('')
    Write-Host "Добавлено: $path"
}

[System.IO.File]::WriteAllText((Join-Path (Get-Location) 'README.md'), $sb.ToString(), $utf8)
Write-Host 'Готово: README.md создан.' -ForegroundColor Green