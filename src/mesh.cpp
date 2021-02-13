#include "mesh.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

mesh::mesh(shader shaderProgram) {
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    this->rotation = 0.0f;

    this->vbo = 0;
    this->vertexCount = 0;

    this->shaderProgram = shaderProgram;
    this->invertBackface = false;
}

int mesh::render(std::stack<glm::mat4>* matrixStack, glm::mat4 perspectiveMatrix, float currentTime) {
    // Load shader
    std::vector<GLuint>* uniformLocs = this->shaderProgram.use();

    // Build matrices
    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::translate(glm::mat4(1.0f), this->position);

    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::scale(glm::mat4(1.0f), this->scale);

    matrixStack->push(matrixStack->top());
    matrixStack->top() *= glm::rotate(glm::mat4(1.0f), this->rotation, this->rotationAxis);

    // Copy to uniforms
    glUniformMatrix4fv(uniformLocs->at(0), 1, GL_FALSE, glm::value_ptr(matrixStack->top()));
    glUniformMatrix4fv(uniformLocs->at(1), 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
    glUniform1f(uniformLocs->at(2), float(currentTime));

    // Bind VBO to shader vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Draw model
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(this->invertBackface ? GL_CW : GL_CCW);
    glDrawArrays(GL_TRIANGLES, 0, this->vertexCount);

    // Return number of matrices added to the stack
    return 3;
}