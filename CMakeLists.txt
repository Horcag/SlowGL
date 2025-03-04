cmake_minimum_required(VERSION 3.30)
project(SlowGL LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

file(GLOB_RECURSE sources_h src/*.h)
file(GLOB_RECURSE sources src/*.cpp src/*.h)

get_filename_component(full_path_main_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ABSOLUTE)

list(REMOVE_ITEM sources "${full_path_main_cpp}")

# Fetch SFML
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

# Fetch nfd для диалогов выбора файлов
FetchContent_Declare(nfd
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG v1.2.1
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(nfd)

# Fetch ImGui
FetchContent_Declare(ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.91.8
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)

# Fetch ImGui-SFML
set(IMGUI_SFML_FIND_SFML OFF)
FetchContent_Declare(ImGui-SFML
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG v3.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(ImGui-SFML)


add_library(SlowGL ${sources}
        src/Render/triangles.cpp
        src/Render/triangles.h
        src/Core/IModule.h
        src/Core/ModuleManager.h
        src/Core/ModuleManager.cpp
        src/Modules/Lab1/Lab1Module.h
        src/Modules/Lab1/Lab1Module.cpp
        src/Utility/FileUtils.h
        src/Utility/FileUtils.cpp
        src/Modules/Lab2/Lab2Module.h
        src/Modules/Lab2/Lab2Module.cpp)

# Копируем шрифты в выходную директорию
file(COPY ${CMAKE_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR}/bin)


# Добавляем пути для include
include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/fonts/Inter
        ${CMAKE_BINARY_DIR}/_deps/imgui-src
        ${CMAKE_BINARY_DIR}/_deps/imgui-sfml-src
        ${CMAKE_BINARY_DIR}/_deps/sfml-src/include
        ${CMAKE_BINARY_DIR}/_deps/nfd-src/src/include
)

# Привязываем библиотеки к SlowGL
target_link_libraries(SlowGL PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML)
target_compile_features(SlowGL PRIVATE cxx_std_17)

# Добавляем исполняемый файл
add_executable(SlowGLApp src/main.cpp)
target_link_libraries(SlowGLApp PRIVATE SlowGL sfml-graphics ImGui-SFML::ImGui-SFML nfd)
target_compile_features(SlowGLApp PRIVATE cxx_std_17)

# Add examples
option(SLOWGL_BUILD_EXAMPLES "Build SlowGL examples" ON)
if (SLOWGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()


# Игорь не фикси тесты
# Add tests
#if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
#    enable_testing()
#
#    # Add test executable
#    add_executable(OBJParserTests
#            tests/OBJParser_test.cpp
#            ${sources}
#    )
#
#    target_include_directories(OBJParserTests PUBLIC ${CMAKE_SOURCE_DIR}/src)
#
#    # Link Google Test
#    target_link_libraries(OBJParserTests PRIVATE gtest_main sfml-graphics ImGui-SFML::ImGui-SFML)
#    target_compile_features(OBJParserTests PRIVATE cxx_std_17)
#
#    # Add tests to CMake testing
#    include(GoogleTest)
#    gtest_discover_tests(OBJParserTests)
#endif ()

