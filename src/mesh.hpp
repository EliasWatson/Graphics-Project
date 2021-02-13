#ifndef H_MESH
#define H_MESH

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stack>

#include "shader.hpp"

struct mesh {
    glm::vec3 position, rotationAxis, scale;
    float rotation;

    GLuint vbo;
    GLsizei vertexCount;

    shader shaderProgram;
    bool invertBackface;

    mesh(shader shaderProgram);
    int render(std::stack<glm::mat4>* matrixStack, glm::mat4 perspectiveMatrix, float currentTime);
};

#endif