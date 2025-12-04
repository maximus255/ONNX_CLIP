# ONNX CLIP Inference in C++

C++ реализация инференса CLIP модели через ONNX Runtime. Проект делает то же самое, что и Python скрипт test_clip.py, но без PyTorch - только ONNX модели.

## Что делает

Загружает изображения через OpenCV, токенизирует текст через BPE токенизатор, запускает инференс через ONNX Runtime для обоих энкодеров (изображение и текст), считает косинусное сходство между эмбеддингами и находит топ-K наиболее подходящих текстовых описаний для каждого изображения.

## Зависимости

ONNX Runtime 1.20.0 - скачивается автоматически через download_onnxruntime.ps1. Библиотеки лежат в onnxruntime/onnxruntime-win-x64-1.20.0/.

OpenCV 3.4.4 - нужна установленная версия по пути D:\WORK\OpenCV\opencv-3.4.4\. Используются модули core, imgproc, imgcodecs.

nlohmann/json и cxxopts - header-only библиотеки, уже лежат в include/.

## Настройка MSVC проекта

Проект уже настроен для ONNX Runtime. Для OpenCV нужно:

1. Открыть свойства проекта (Properties)
2. В Configuration Properties -> VC++ Directories -> Include Directories добавить: D:\WORK\OpenCV\opencv-3.4.4\build\install\include
3. В Configuration Properties -> VC++ Directories -> Library Directories добавить: D:\WORK\OpenCV\opencv-3.4.4\build\install\x64\vc14\staticlib\
4. Библиотеки линкуются автоматически через pragma comment в ImageProcessor.cpp

Если OpenCV установлена в другом месте, измени пути в ONNX_CPP.vcxproj в секциях AdditionalIncludeDirectories и AdditionalLibraryDirectories.

## Модели и файлы

В директории с моделями должны быть:
- image_encoder.onnx
- text_encoder.onnx
- tokenizer_encoder.json
- bpe_simple_vocab_16e6.txt

Также нужен файл classes.txt в родительской директории относительно models-dir или рядом с exe. В нем по одной строке на описание класса.

## Запуск

Собрать проект в Visual Studio, получить ONNX_CPP.exe. Запустить через командную строку:

```
ONNX_CPP.exe --images путь_к_изображениям --models-dir путь_к_моделям
```

Опциональные параметры:
- --topk N - сколько топ результатов показывать (по умолчанию 3)
- --max-images N - максимум изображений для обработки (по умолчанию 10)

Пример:
```
ONNX_CPP.exe --images D:\test_images --models-dir D:\WORK\AI\ONNX_CLIP\models --topk 5 --max-images 20
```

Или короткие флаги:
```
ONNX_CPP.exe -i D:\test_images -d D:\WORK\AI\ONNX_CLIP\models -k 5 -m 20
```

Программа найдет все jpg, jpeg, png файлы в указанной директории (рекурсивно), загрузит модели, закодирует тексты из classes.txt, обработает изображения и выведет топ-K совпадений для каждого.

## Структура проекта

include/ - заголовочные файлы, разбиты по модулям (config, utils, image, text, onnx, math, clip)
src/ - реализации .cpp файлов
Main.cpp - точка входа, парсинг аргументов командной строки

## Технические детали

Изображения обрабатываются по одной (batch size 1), как в Python версии. Текстовые эмбеддинги кешируются после первого вычисления. Используется C++17, стандартная библиотека, RAII для управления ресурсами ONNX, исключения для обработки ошибок.

Изображения нормализуются по ImageNet стандарту: mean [0.485, 0.456, 0.406], std [0.229, 0.224, 0.225]. Размер 224x224, формат CHW float32.

Токенизация через BPE, контекст 77 токенов, эмбеддинги размером 512.
