# build-readme.ps1: собирает общий README.md из README всех лабораторных работ.
# Запускать из корня репозитория.
$utf8 = New-Object System.Text.UTF8Encoding $false

$header = @'
<!-- Файл создан скриптом build-readme.ps1. Правь README в папках лабораторных и запускай скрипт заново. -->
# Компьютерная графика, 3 курс: лабораторные работы

Репозиторий с семью лабораторными работами по курсу «Компьютерная графика»: от окна и шестиугольника на OpenGL 1.0 до управляемой трёхмерной модели робота с освещением.

**Автор:** Горбенко А. В., группа АСУбз-24-1
**Вариант:** 14 (шестиугольник, цвет фигуры (0.5, 0.5, 1.0), цвет фона (1.0, 1.0, 0.3))

**Технологии:** Visual Studio (x64), OpenGL 4.6 Core Profile, GLFW, GLEW, GLM, Assimp, Blender.

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