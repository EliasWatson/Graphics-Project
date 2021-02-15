#ifndef H_MATERIAL
#define H_MATERIAL

#include "shader.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct material {
    struct model_data {
        std::vector<GLuint> vbo;
        GLuint indices;
        int vertexCount;
        bool invertBackface;
    };

    struct params {
        glm::mat4 perspective, modelView, rotation;
        float time;
    };

    shader shaderProgram;
    glm::vec3 ambient, diffuse, specular, shininess;
    std::vector<shader_texture> textures;

    material() { }
    material(shader shaderProgram) : shaderProgram(shaderProgram) { }

    void render(model_data md, params p);
};

#endif