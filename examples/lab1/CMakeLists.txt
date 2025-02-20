add_executable(slowgl_lab1 main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/fonts/Inter)
target_link_libraries(slowgl_lab1 PRIVATE sfml-graphics ImGui-SFML::ImGui-SFML)
target_compile_features(slowgl_lab1 PRIVATE cxx_std_17)
