# Архитектура SeeBot

## 4-слойная схема

1. **Injector**
2. **Core**
3. **Managers / Products / Extensions**
4. **GUI / IPC**

```text
[Injector]
    ↓ inject (LoadLibraryA)
[Core: DllMain + bootstrap + hooks]
    ↓ lifecycle / tick
[Managers | Products | Extensions]
    ↓ commands / state / events
[GUI | IPC]
```

## Ответственность слоёв

### 1) Injector
- Отдельный Win32-процесс, который ищет `Wow.exe` и выполняет DLL injection через `CreateRemoteThread + LoadLibraryA`.
- Не содержит игровой логики и не управляет игровым циклом.
- Отвечает только за доставку `SeeBot.dll` в целевой процесс.

### 2) Core
- Точка входа DLL (`DllMain`) и безопасный bootstrap.
- Инициализирует базовый жизненный цикл (`Init`, `MainLoop`, `Shutdown`).
- Содержит infra-уровень для hook-механизмов (например, будущий `EndScene` hook), memory offsets и базовые runtime-инициализации.
- Гарантирует, что тяжёлая логика не выполняется напрямую в `DllMain`.

### 3) Managers / Products / Extensions
- **Managers**: доменные сервисы (объекты мира, цели, передвижение, спеллы, инвентарь, Lua).
- **Products**: высокоуровневые «режимы/продукты» (например, gatherer), которые координируют менеджеры.
- **Extensions**: расширяемые контракты (плагины/ротации), позволяющие подключать новую логику без изменения ядра.
- Слой реализует прикладное поведение бота поверх Core-тактов.

### 4) GUI / IPC
- Внешний интерфейс управления (GUI) и transport-канал (IPC).
- Передаёт команды в Core/Managers и получает статус/телеметрию.
- Должен быть изолирован от прямых вызовов в критические участки игрового потока.

## Поток выполнения (текущий и целевой)

1. **Injection**
   - Injector находит `Wow.exe`, открывает процесс, пишет путь к DLL, запускает удалённый поток с `LoadLibraryA`.
2. **`DllMain` (DLL_PROCESS_ATTACH)**
   - В `DllMain` отключаются thread notifications (`DisableThreadLibraryCalls`).
   - Создаётся отдельный bootstrap-поток и `DllMain` быстро возвращает управление.
3. **Thread bootstrap**
   - Bootstrap-поток выполняет минимальную инициализацию (`seebot::Init`) и подготавливает runtime.
   - Логика, зависящая от игрового потока, остаётся отложенной.
4. **Future EndScene loop**
   - После установки хука `EndScene` основной тик переносится в игровой/рендер-поток.
   - Там безопасно выполняются: object iteration, Lua dispatch, products/extensions tick, IPC command apply.
