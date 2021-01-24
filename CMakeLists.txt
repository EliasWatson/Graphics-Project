cmake_minimum_required(VERSION 3.1)
project(Graphics_Project)

add_executable(Graphics_Project
    src/main.cpp
)
set_property(TARGET Graphics_Project PROPERTY CXX_STANDARD 11)
target_compile_options(Graphics_Project PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(Graphics_Project PRIVATE glfw)