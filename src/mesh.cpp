#include "mesh.hpp"

#include <glm/gtc/matrix_transform.hpp>

mesh::mesh() {
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    this->rotation = 0.0f;

    this->vbo = std::vector<GLuint>();
    this->vboIndices = 0;
    this->vertexCount = 0;

    this->mat = material();
    this->invertBackface = false;
}

int mesh::render(std::stack<glm::mat4>* matrixStack, glm::mat4 perspectiveMatrix, float currentTime, material::light_data lightData) {
    // Build matrices
    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::translate(glm::mat4(1.0f), this->position);

    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::scale(glm::mat4(1.0f), this->scale);

    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::rotate(glm::mat4(1.0f), this->rotation, this->rotationAxis);

    this->invMvMat = glm::transpose(glm::inverse(matrixStack->top()));

    // Render mesh
    this->mat.render({
        this->vbo,
        this->vboIndices,
        this->vertexCount,
        this->invertBackface
    }, {
        perspectiveMatrix,
        matrixStack->top(),
        this->invMvMat,
        currentTime
    }, lightData);

    // Return number of matrices added to the stack
    return 3;
}