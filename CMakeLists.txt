cmake_minimum_required(VERSION 3.30)
project(SlowGL LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

file(GLOB_RECURSE sources_h src/*.h)
file(GLOB_RECURSE sources src/*.cpp src/*.h)

get_filename_component(full_path_main_cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ABSOLUTE)

list(REMOVE_ITEM sources "${full_path_main_cpp}")

add_library(SlowGL ${sources})

# Fetch SFML
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

target_link_libraries(SlowGL PRIVATE sfml-graphics)
target_compile_features(SlowGL PRIVATE cxx_std_17)

# Add examples
option(SLOWGL_BUILD_EXAMPLES "Build SlowGL examples" ON)
if (SLOWGL_BUILD_EXAMPLES)
        add_subdirectory(examples)
endif ()

# Копируем шрифты в выходную директорию
file(COPY ${CMAKE_SOURCE_DIR}/fonts DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/fonts/Inter)

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