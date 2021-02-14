#include "mesh.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

mesh::mesh() {
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    this->rotation = 0.0f;

    this->vbo = std::vector<GLuint>();
    this->vboIndices = 0;
    this->vertexCount = 0;

    this->shaderProgram = shader();
    this->textures = std::vector<shader_texture>();
    this->invertBackface = false;
}

int mesh::render(std::stack<glm::mat4>* matrixStack, glm::mat4 perspectiveMatrix, float currentTime) {
    // Load shader
    std::vector<GLuint>* uniformLocs = this->shaderProgram.use(this->vbo, this->textures);

    // Build matrices
    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::translate(glm::mat4(1.0f), this->position);

    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::scale(glm::mat4(1.0f), this->scale);

    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::rotate(glm::mat4(1.0f), this->rotation, this->rotationAxis);

    // This doesn't account for parented rotation, but good enough for now
    glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), this->rotation, this->rotationAxis);

    // Copy to uniforms
    glUniformMatrix4fv(uniformLocs->at(0), 1, GL_FALSE, glm::value_ptr(rotMat));
    glUniformMatrix4fv(uniformLocs->at(1), 1, GL_FALSE, glm::value_ptr(matrixStack->top()));
    glUniformMatrix4fv(uniformLocs->at(2), 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
    glUniform1f(uniformLocs->at(3), float(currentTime));

    // Setup options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(this->invertBackface ? GL_CW : GL_CCW);

    // Draw model
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glDrawElements(GL_TRIANGLES, this->vertexCount, GL_UNSIGNED_INT, 0);

    // Return number of matrices added to the stack
    return 3;
}