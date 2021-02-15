#include "material.hpp"

#include <glm/gtc/type_ptr.hpp>

void material::render(model_data md, params p) {
    // Load shader
    std::vector<GLuint>* uniformLocs = this->shaderProgram.use(md.vbo, this->textures);

    // Copy to uniforms
    glUniformMatrix4fv(uniformLocs->at(0), 1, GL_FALSE, glm::value_ptr(p.rotation));
    glUniformMatrix4fv(uniformLocs->at(1), 1, GL_FALSE, glm::value_ptr(p.modelView));
    glUniformMatrix4fv(uniformLocs->at(2), 1, GL_FALSE, glm::value_ptr(p.perspective));
    glUniform1f(uniformLocs->at(3), p.time);

    // Setup options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(md.invertBackface ? GL_CW : GL_CCW);

    // Draw model
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.indices);
    glDrawElements(GL_TRIANGLES, md.vertexCount, GL_UNSIGNED_INT, 0);
}