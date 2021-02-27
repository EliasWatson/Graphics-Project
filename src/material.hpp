#ifndef H_MATERIAL
#define H_MATERIAL

#include "shader.hpp"
#include "texture.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#define MAX_LIGHTS 2

struct material {
    struct model_data {
        std::vector<GLuint> vbo;
        GLuint indices;
        int vertexCount;
        bool invertBackface;
    };

    struct params {
        glm::mat4 perspective, modelView, invModelView;
        float time;
    };

    struct light_data {
        float pos[MAX_LIGHTS][4];
        float ambient[MAX_LIGHTS][4];
        float diffuse[MAX_LIGHTS][4];
        float specular[MAX_LIGHTS][4];
    };

    shader shaderProgram;
    glm::vec4 ambient, diffuse, specular;
    float shininess;
    std::vector<texture> textures;

    material() { }
    material(shader shaderProgram) : shaderProgram(shaderProgram) { }

    void render(model_data md, params p, light_data l);
};

#endif