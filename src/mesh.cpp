#include "mesh.hpp"

#include "scene.hpp"

#include <glm/gtc/matrix_transform.hpp>

mesh::mesh() {
    this->boundMin = glm::vec3(0.0, 0.0, 0.0);
    this->boundMax = glm::vec3(0.0, 0.0, 0.0);

    this->vbo = std::vector<GLuint>();
    this->ebo = 0;
    this->vertexCount = 0;

    this->materialIndex = 0;
    this->invertBackface = false;
}