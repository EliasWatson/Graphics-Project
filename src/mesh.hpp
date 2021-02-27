#ifndef H_MESH
#define H_MESH

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <stack>

struct mesh {
    glm::vec3 boundMin, boundMax;

    GLuint vao;
    std::vector<GLuint> vbo;
    GLuint ebo;
    GLsizei vertexCount;

    int materialIndex = 0;
    bool invertBackface;

    mesh();
};

#endif