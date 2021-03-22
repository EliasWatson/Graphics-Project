#ifndef H_MATERIAL
#define H_MATERIAL

#include "shader.hpp"
#include "texture.hpp"
#include "environment.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

#define MAX_LIGHTS 2

struct material {
    struct model_data {
        GLuint vao;
        std::vector<GLuint> vbo;
        GLuint indices;
        int vertexCount;
        bool invertBackface;
    };

    struct params {
        glm::vec4 camPos;
        glm::mat4 perspective, view, model, invModel;
        float time;
    };

    struct light_data {
        float pos[MAX_LIGHTS][4];
        float ambient[MAX_LIGHTS][4];
        float diffuse[MAX_LIGHTS][4];
        float specular[MAX_LIGHTS][4];
    };

    shader shaderProgram;
    glm::vec4 ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    glm::vec4 diffuse = glm::vec4(1.0f, 1.0, 1.0f, 1.0f);
    glm::vec4 specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float shininess = 1.0f;
    std::vector<texture> textures;

    material() { }
    material(shader shaderProgram) : shaderProgram(shaderProgram) { }

    void render(model_data md, params p, light_data l, environment env);
};

#endif