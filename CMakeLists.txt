cmake_minimum_required(VERSION 3.1)
project(Graphics_Project)

add_executable(Graphics_Project
    src/main.cpp
    src/util.hpp
    src/shader.hpp
    src/shader.cpp
    src/material.hpp
    src/material.cpp
    src/light.hpp
    src/light.cpp
    src/mesh.hpp
    src/mesh.cpp
    src/texture.hpp
    src/texture.cpp
    src/model_importer.hpp
    src/model_importer.cpp
)
set_property(TARGET Graphics_Project PROPERTY CXX_STANDARD 14)
#target_compile_options(Graphics_Project PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(Graphics_Project PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(Graphics_Project PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(Graphics_Project PRIVATE glm)

# soil2
add_subdirectory(lib/soil2 EXCLUDE_FROM_ALL)
target_link_libraries(Graphics_Project PRIVATE soil2)

# tinyobjloader
add_subdirectory(lib/tinyobjloader EXCLUDE_FROM_ALL)
target_link_libraries(Graphics_Project PRIVATE tinyobjloader)