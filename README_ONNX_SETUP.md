# Настройка ONNX Runtime для C++

## Вариант 1: Скачать предкомпилированные библиотеки (рекомендуется для начала)

### Шаги:

1. **Скачайте ONNX Runtime для Windows x64:**
   - Перейдите на: https://github.com/microsoft/onnxruntime/releases
   - Скачайте архив `onnxruntime-win-x64-<версия>.zip` (например, `onnxruntime-win-x64-1.16.3.zip`)
   - Распакуйте в папку проекта, например: `ONNX_CPP/onnxruntime/`

2. **Структура должна быть:**
   ```
   ONNX_CPP/
   ├── onnxruntime/
   │   ├── include/
   │   │   └── onnxruntime/
   │   │       └── core/
   │   │           └── session/
   │   │               └── onnxruntime_cxx_api.h
   │   └── lib/
   │       └── onnxruntime.lib
   └── ONNX_CPP.cpp
   ```

3. **Настройте пути в Visual Studio:**
   - Правой кнопкой на проект → Properties
   - Configuration Properties → VC++ Directories:
     - **Include Directories**: добавьте `$(ProjectDir)onnxruntime\include`
     - **Library Directories**: добавьте `$(ProjectDir)onnxruntime\lib`
   - Configuration Properties → Linker → Input:
     - **Additional Dependencies**: добавьте `onnxruntime.lib`

4. **Скопируйте DLL:**
   - Скопируйте `onnxruntime.dll` из `onnxruntime/lib/` в папку с исполняемым файлом (обычно `x64/Debug/` или `x64/Release/`)

---

## Вариант 2: Использовать vcpkg (для долгосрочных проектов)

### Шаги:

1. **Установите vcpkg** (если еще не установлен):
   ```powershell
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

2. **Интегрируйте vcpkg с Visual Studio:**
   ```powershell
   .\vcpkg integrate install
   ```

3. **Установите ONNX Runtime:**
   ```powershell
   .\vcpkg install onnxruntime:x64-windows
   ```

4. **В Visual Studio:**
   - Tools → Options → vcpkg → Use vcpkg
   - Укажите путь к vcpkg

---

## Проверка подключения

После настройки попробуйте скомпилировать код с:
```cpp
#include <onnxruntime_cxx_api.h>
```

Если компиляция проходит без ошибок - библиотека подключена правильно!

