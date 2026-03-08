# SeeBot

SeeBot — учебный стенд для разработки модульного Win32-бота под **World of Warcraft 3.3.5a**.
Проект состоит из DLL (`SeeBot`), которая загружается в процесс игры, и отдельного инжектора (`Injector`) для доставки DLL в `Wow.exe`.

## Требования

- **ОС:** Windows
- **Архитектура:** только **x86 (32-bit / Win32)**
- **Система сборки:** CMake
- **Компилятор:**
  - MSVC (Visual Studio, платформа Win32), или
  - MinGW toolchain в 32-bit конфигурации

## Быстрый старт

Пошаговые команды для MSVC и MinGW вынесены в отдельное руководство:

- [Сборка проекта (`docs/build.md`)](docs/build.md)

## Структура репозитория

```text
seemoon/
├─ Core/        # DLL SeeBot: lifecycle, hooks, game-модули, objects, ipc, products/extensions
├─ Injector/    # Win32-инжектор DLL в Wow.exe
├─ GUI/         # заготовка UI-слоя
├─ profiles/    # заготовки профилей (маршруты/ротации)
├─ docs/        # проектная документация
├─ Codex.md     # инженерные заметки по ограничениям, процессу и статусу
└─ CMakeLists.txt
```

## Текущий прогресс

- **Готово/рабочий минимум:**
  - инжектор DLL (`Injector`) с базовым сценарием `LoadLibraryA`;
  - базовый bootstrap DLL и каркас модульной архитектуры;
  - CMake-ограничение на Win32/x86 сборку.
- **In progress:**
  - hook-инфраструктура (включая заготовку EndScene);
  - объектная модель и интеграция Lua в игровом контексте;
  - продуктовый/extension-слой (включая начальный `gatherer`).
- **Stub/заглушки:**
  - часть игровых менеджеров (`movement`, `spell_manager`, `inventory`, `target_manager`);
  - IPC server;
  - GUI и профили.

Подробный статус модулей: [docs/status.md](docs/status.md).

## Important constraints

1. **32-bit only:** проект поддерживает только Win32/x86. Любая попытка конфигурировать 64-bit сборку считается некорректной.
2. **Потокобезопасность Lua-вызовов:** обращения к Lua и игровым объектам должны выполняться строго в игровом потоке (через игровой/рендер hook), а не из `DllMain` и не из произвольных фоновых потоков.

## Полная документация

- [Сборка (`docs/build.md`)](docs/build.md)
- [Архитектура (`docs/architecture.md`)](docs/architecture.md)
- [Статус модулей (`docs/status.md`)](docs/status.md)
- [Roadmap (`docs/roadmap.md`)](docs/roadmap.md)
- [Codex notes (`Codex.md`)](Codex.md)
