
add_executable(slowgl_lab1 main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/fonts/Inter)

# Link libraries
target_link_libraries(slowgl_lab1 PRIVATE SlowGL sfml-graphics glm::glm ImGui-SFML::ImGui-SFML nfd)
target_compile_features(slowgl_lab1 PRIVATE cxx_std_17)
