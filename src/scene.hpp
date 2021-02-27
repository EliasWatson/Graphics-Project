#ifndef H_SCENE
#define H_SCENE

#include <vector>
#include <gl/glew.h>
#include <string>

#include "shader.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "entity.hpp"
#include "light.hpp"
#include "texture.hpp"
#include "camera.hpp"

struct scene {
    enum shader_type {
        DEFAULT
    };

    shader shaderProgram;
    material::light_data lightData;
    std::string baseDirectory;

    std::vector<texture> textures;
    std::vector<material> materials;
    std::vector<mesh> meshes;
    std::vector<light> lights;
    std::vector<camera> cameras;

    int mainCamera = 0;
    entity rootNode;

    // Allocated here to prevent allocation per-frame
    std::stack<glm::mat4> mvStack;

    scene(shader_type type);
    void render();
    void resize(int width, int height);

private:
    void loadDefaultShaders();
};

#endif