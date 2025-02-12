# **RoadMap SlowGL**

## **Общая архитектура проекта**

```plaintext
SlowGL/
│── CMakeLists.txt
│── main.cpp
│── modules/
│   ├── Core/
│   │   ├── Core.h
│   │   ├── Core.cpp
│   ├── Image/
│   │   ├── Image.h
│   │   ├── Image.cpp
│   ├── Renderer/
│   │   ├── Renderer.h
│   │   ├── Renderer.cpp
│   ├── OBJParser/
│   │   ├── OBJParser.h
│   │   ├── OBJParser.cpp
│── resources/
│   ├── models/
│   │   ├── sample.obj
│── examples/
│   ├── Example1/
│   │   ├── main.cpp
│   ├── Example2/
│   │   ├── main.cpp
│── labs/
│   ├── lab1/
│   │   ├── main.cpp
│   ├── lab2/
│   │   ├── main.cpp
```

## **Разделение на модули**

- **Core/** – Вспомогательные функции, математические операции
- **Image/** – Генерация и обработка изображений
- **Renderer/** – Реализация алгоритмов растеризации, отрисовка моделей
- **OBJParser/** – Чтение и разбор `.obj` файлов
- **examples/** – Примеры использования, не включаемые в основную сборку
- **labs/** – Отдельные лабораторные работы, каждая со своим `main.cpp`

---

## **1. Работа с изображениями (Модуль `Image/`)**

### **Функциональность**

1. Генерация полностью чёрного (`H × W`, 8-битное, монохромное) изображения.
2. Генерация полностью белого (`H × W`, 8-битное, монохромное) изображения.
3. Создание красного (`H × W`, 8-битное, цветное) изображения.
4. Генерация градиентного изображения (`H × W`, цветное).

### **Техническая реализация**

- Используем `sf::Image` из SFML для работы с пиксельными данными.
- Оптимизация хранения данных через `std::vector<uint8_t>`.
- Сохранение изображений в формате PNG.

---

## **2. Алгоритмы отрисовки линий (Модуль `Renderer/`)**

### **Функциональность**

1. Реализация алгоритма DDA (Digital Differential Analyzer).
2. Реализация алгоритма Брезенхема.
3. Отрисовка 12-лучевой звезды в окне 200×200.

### **Техническая реализация**

- Использование `sf::VertexArray` для отрисовки линий.
- Оптимизация хранения буфера изображения.
- Выбор оптимального алгоритма в зависимости от требований к точности и скорости.

---

## **3. Разбор формата `.obj` (Модуль `OBJParser/`)**

### **Функциональность**

1. Чтение вершин модели (`v X Y Z`).
2. Чтение граней (`f v1 v2 v3`).
3. Игнорирование координаты `Z` для 2D-отрисовки.

### **Техническая реализация**

- Использование `std::ifstream` для построчного чтения файла.
- Парсинг координат вершин с использованием `std::istringstream`.
- Хранение данных в `std::vector<Vertex>`.

---

## **Настройка CMake**

- Основной `CMakeLists.txt` собирает только основную часть проекта (`SlowGL`).
- Примеры из `examples/` и лабораторные работы из `labs/` собираются только в среде разработки (например, CLion, VS Code), но исключаются из финальной сборки.

**Пример кода для CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(SlowGL LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(SlowGL STATIC
    modules/Core/Core.cpp
    modules/Image/Image.cpp
    modules/Renderer/Renderer.cpp
    modules/OBJParser/OBJParser.cpp
)

add_executable(SlowGL_main main.cpp)
target_link_libraries(SlowGL_main PRIVATE SlowGL sfml-graphics)

if(CMAKE_PROJECT_NAME STREQUAL "SlowGL" AND CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_subdirectory(examples)
    add_subdirectory(labs)
endif()
```

Этот подход позволяет использовать примеры и лабораторные работы отдельно от основного проекта, а при релизной сборке они не учитываются.

