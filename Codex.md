# Codex

## Project overview
- **SeeBot** — Win32 DLL-проект (`SeeBot`) + отдельный инжектор (`Injector`) для загрузки в клиент WoW.
- **Текущая фаза**: каркас/интеграционный bootstrap (основные подсистемы созданы как заглушки, логика поэтапно наполняется).
- **Состав таргетов**:
  - `SeeBot` (`SHARED`) — основная DLL с ядром, хуками, игровыми модулями, IPC и объектной моделью.
  - `Injector` (`EXECUTABLE`) — утилита инжекта `LoadLibraryA` в `Wow.exe`.

## Hard constraints
- [x] Только **Win32/x86 (32-bit)**.
- [x] **64-bit сборки запрещены** (CMake завершает конфигурацию с ошибкой).
- [x] Стандарт: **C++17**.
- [x] Система сборки: **CMake**.
- [x] Кодировка исходников/компиляции: **UTF-8**.

См. детали: [`docs/build.md`](docs/build.md).

## Repository map
- `Core/` — DLL (`SeeBot`):
  - `hooks/` — хуки рендера/игры (например, EndScene).
  - `objects/` — объектная модель WoW + `ObjectManager`.
  - `game/` — игровые подсистемы (movement, spells, inventory, targets, lua).
  - `ipc/` — межпроцессное взаимодействие.
  - `memory/` — offsets/адреса.
  - `extensions/`, `products/` — интерфейсы расширений/продуктов и реализации.
- `Injector/` — консольный инжектор DLL в процесс игры.
- `docs/` — проектная документация и инструкции по сборке.
- `profiles/` — заготовки под ротации/маршруты (`rotations`, `routes`).
- `GUI/` — заготовка под UI-компоненты.

## Build & run quickstart
Источник: [`docs/build.md`](docs/build.md).

### MSVC (Visual Studio)
- [ ] `cmake -S . -B build-vs -G "Visual Studio 17 2022" -A Win32`
- [ ] `cmake --build build-vs --config Release`

### MinGW (x86)
- [ ] `cmake -S . -B build-mingw -G "MinGW Makefiles"`
- [ ] `cmake --build build-mingw`

> Важно: toolchain должен быть именно **x86**, иначе конфигурация будет остановлена.

## Development workflow
- Добавление нового модуля:
  - [ ] Создать `*.h` + `*.cpp` в соответствующем подкаталоге `Core/` (`game/`, `objects/`, `ipc/`, `hooks/` и т.д.).
  - [ ] Подключить `*.cpp` в `Core/CMakeLists.txt`.
  - [ ] Экспортировать минимальный интерфейс через namespace `seebot::<domain>`.
- Naming:
  - [ ] Классы: `PascalCase` (`ObjectManager`, `LuaEngine`).
  - [ ] Константы: `kCamelCase` (`kPlayerBase`).
  - [ ] Пространства имен: `seebot::<subsystem>`.
- Данные игры:
  - [ ] Offsets хранить централизованно в `Core/memory/offsets.h`.
  - [ ] Enum’ы хранить рядом с доменом использования (предпочтительно в `Core/game/` или `Core/objects/` отдельными `*_enums.h`).

## Safety notes
- [x] Lua-вызовы выполнять **только из игрового потока** (через рендер/игровой hook).
- [x] Не выполнять Lua из `DllMain` или потока инициализации.
- [x] В `DllMain` держать только минимальный bootstrap (инициализирующий поток, без тяжёлой логики).

## Current status
Текущие заготовки/модули в репозитории:
- [x] Core lifecycle: `Core/core.h`, `Core/core.cpp`, `Core/dllmain.cpp`.
- [x] Hooks: `Core/hooks/endscene_hook.h`, `Core/hooks/endscene_hook.cpp` (stub).
- [x] Object system: `Core/objects/object_manager.*`, `wow_object.h`, `wow_unit.h`, `wow_player.h`, `wow_local_player.h`, `wow_gameobject.h`.
- [x] Game systems: `Core/game/lua_engine.*`, `movement.*`, `spell_manager.*`, `inventory.*`, `target_manager.*`.
- [x] IPC: `Core/ipc/pipe_server.*` (stub).
- [x] Product/extension contracts: `Core/products/iproduct.h`, `Core/products/gatherer.*`, `Core/extensions/iplugin.h`, `Core/extensions/irotation.h`.
- [x] Injector: `Injector/main.cpp`.
- [x] Build/docs baseline: `CMakeLists.txt`, `Core/CMakeLists.txt`, `Injector/CMakeLists.txt`, `docs/build.md`.

## Next milestones
- [ ] EndScene hook: реальная установка/снятие хука + безопасный тик игрового цикла.
- [ ] Object Manager: чтение и кеширование object list, типизация сущностей.
- [ ] Lua Engine integration: безопасная очередь/диспетчер команд в игровом потоке.
- [ ] IPC: рабочий pipe-протокол для команд из внешнего процесса/GUI.

## Related docs
- [`docs/build.md`](docs/build.md)
