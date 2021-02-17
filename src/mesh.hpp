#ifndef H_MESH
#define H_MESH

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stack>

#include "material.hpp"

struct mesh {
    glm::vec3 position, rotationAxis, scale;
    float rotation;

    std::vector<GLuint> vbo;
    GLuint vboIndices;
    GLsizei vertexCount;

    material mat;
    bool invertBackface;

    // These are declared to help prevent stack allocations during render time
    glm::mat4 invMvMat;

    mesh();
    int render(std::stack<glm::mat4>* matrixStack, glm::mat4 perspectiveMatrix, float currentTime, material::light_data lightData);
};

#endif