# Сборка SeeBot (только 32-bit)

Проект поддерживает **только 32-bit (Win32/x86)** сборку. На уровне CMake есть проверка, и конфигурация завершится ошибкой при попытке собрать 64-bit.

## Visual Studio (MSVC)

Используйте генератор Visual Studio с указанием платформы Win32:

```bash
cmake -S . -B build-vs -G "Visual Studio 17 2022" -A Win32
cmake --build build-vs --config Release
```

## MinGW (GCC/Clang)

Используйте 32-bit toolchain MinGW. Пример:

```bash
cmake -S . -B build-mingw -G "MinGW Makefiles"
cmake --build build-mingw
```

> Важно: компилятор и окружение MinGW должны быть именно x86 (32-bit), иначе CMake остановит конфигурацию.
