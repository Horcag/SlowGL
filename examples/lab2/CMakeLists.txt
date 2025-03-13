
add_executable(slowgl_lab2 main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/fonts/Inter)

# Link libraries
target_link_libraries(slowgl_lab2 PRIVATE SlowGL sfml-graphics ImGui-SFML::ImGui-SFML nfd glm::glm libglew_static)
target_compile_features(slowgl_lab2 PRIVATE cxx_std_17)
