# Статус модулей

## Таблица модулей

| Модуль (файл/папка) | Назначение | Статус |
|---|---|---|
| `Injector/main.cpp` | Консольный инжектор DLL в `Wow.exe` через WinAPI | ready |
| `Core/dllmain.cpp` | DLL entrypoint и запуск bootstrap-потока | in-progress |
| `Core/core.h`, `Core/core.cpp` | Базовый lifecycle (`Init/MainLoop/Shutdown`) | stub |
| `Core/hooks/` (`endscene_hook.*`, `hook_utils.h`) | Каркас hook-инфраструктуры, будущий EndScene тик | in-progress |
| `Core/memory/offsets.h` | Централизованный контейнер offsets/адресов | stub |
| `Core/objects/` | Объектная модель WoW и `ObjectManager` | in-progress |
| `Core/game/lua_engine.*` | Точка интеграции Lua-вызовов в игровом потоке | in-progress |
| `Core/game/movement.*` | Менеджер передвижения | stub |
| `Core/game/spell_manager.*` | Менеджер спеллов/кастов | stub |
| `Core/game/inventory.*` | Менеджер инвентаря | stub |
| `Core/game/target_manager.*` | Менеджер выбора/сопровождения цели | stub |
| `Core/ipc/pipe_server.*` | IPC сервер (named pipe) для внешних команд | stub |
| `Core/products/` (`iproduct.h`, `gatherer.*`) | Продуктовый слой и пример gatherer | in-progress |
| `Core/extensions/` (`iplugin.h`, `irotation.h`) | Контракты расширений/ротаций | in-progress |
| `GUI/` | UI-слой (каркас, без реализации) | stub |
| `profiles/` | Шаблоны маршрутов/ротаций | stub |

## Что уже реализовано минимально

- Рабочий инжектор с полным базовым сценарием `FindProcess -> OpenProcess -> VirtualAllocEx -> WriteProcessMemory -> CreateRemoteThread(LoadLibraryA)`.
- DLL bootstrap с выносом инициализации из `DllMain` в отдельный поток.
- Каркас модульной структуры: hooks, objects, game managers, ipc, products, extensions.
- Базовые интерфейсы расширяемости (`IProduct`, `IPlugin`, `IRotation`) и начальный `gatherer`.
- CMake-ограничение на Win32/x86 сборку и базовая документация по сборке.

## Технические риски и зависимости

1. **Потокобезопасность и контекст выполнения**
   - Lua и доступ к игровым объектам должны выполняться строго в игровом потоке (через будущий hook).
   - Ошибки контекста вызова могут приводить к крашам клиента.

2. **Хуки и совместимость клиента**
   - Реализация EndScene hook зависит от конкретной версии клиента/рендера.
   - Anti-cheat/защиты и обновления клиента могут ломать сигнатуры и цепочки вызовов.

3. **Offsets и object manager**
   - Требуется стабильная схема обновления offsets и валидация адресов.
   - Неверные offsets ведут к UB, чтению мусора и нестабильности.

4. **IPC протокол и границы доверия**
   - Внешние команды через pipe должны проходить валидацию.
   - Нужны ограничения на частоту/размер сообщений и отказоустойчивость при разрыве соединения.

5. **Ограничение платформы**
   - Проект завязан на Win32/x86; перенос на x64 потребует пересборки адресной модели, хуков и ABI-совместимости.
