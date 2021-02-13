#ifndef H_MESH
#define H_MESH

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stack>

struct mesh {
    glm::vec3 position, rotationAxis, scale;
    float rotation;

    GLuint vbo;
    GLsizei vertexCount;
    GLuint shader;

    bool invertBackface;

    mesh();
    int render(std::vector<GLuint>* uniformLocs, std::stack<glm::mat4>* matrixStack, glm::mat4 perspectiveMatrix, float currentTime);
};

#endif