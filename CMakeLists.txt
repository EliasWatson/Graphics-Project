cmake_minimum_required(VERSION 3.1)
project(Graphics_Project)

include_directories(
    lib/imgui
    lib/imgui/backends
    lib/glm
)

add_executable(Graphics_Project
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp

    src/main.cpp
    src/util.hpp
    src/util.cpp
    src/shader.hpp
    src/shader.cpp
    src/material.hpp
    src/material.cpp
    src/light.hpp
    src/light.cpp
    src/mesh.hpp
    src/mesh.cpp
    src/entity.hpp
    src/entity.cpp
    src/texture.hpp
    src/texture.cpp
    src/camera.hpp
    src/camera.cpp
    src/scene.hpp
    src/scene.cpp
    src/importers/scene_importer.hpp
    src/importers/scene_importer.cpp
    src/importers/texture_importer.hpp
    src/importers/texture_importer.cpp
)
#set_property(TARGET Graphics_Project PROPERTY CXX_STANDARD 14)
#target_compile_options(Graphics_Project PRIVATE -Wall)

# glfw
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# glew
add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)

# glm
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

# soil2
add_subdirectory(lib/soil2 EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE soil2)

# assimp
#set(ZLIB_LIBRARIES zlibstatic)
# set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_ASSIMP_TOOLS ON)
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

include_directories("lib/assimp/build/include" "lib/assimp/include")
add_subdirectory(lib/assimp EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp)