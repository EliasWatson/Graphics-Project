#include "material.hpp"

#include <glm/gtc/type_ptr.hpp>

void material::render(model_data md, params p, light_data l) {
    // Load shader
    std::vector<GLuint>* uniformLocs = this->shaderProgram.use(md.vbo, this->textures);

    // Copy to uniforms
    glUniformMatrix4fv(uniformLocs->at(0), 1, GL_FALSE, glm::value_ptr(p.perspective));
    glUniformMatrix4fv(uniformLocs->at(1), 1, GL_FALSE, glm::value_ptr(p.modelView));
    glUniformMatrix4fv(uniformLocs->at(2), 1, GL_FALSE, glm::value_ptr(p.invModelView));
    glUniform1f(uniformLocs->at(3), p.time);

    glUniform4fv(uniformLocs->at(4), MAX_LIGHTS, l.pos[0]);
    glUniform4fv(uniformLocs->at(5), MAX_LIGHTS, l.ambient[0]);
    glUniform4fv(uniformLocs->at(6), MAX_LIGHTS, l.diffuse[0]);
    glUniform4fv(uniformLocs->at(7), MAX_LIGHTS, l.specular[0]);

    glUniform4fv(uniformLocs->at(8),  1, glm::value_ptr(this->ambient));
    glUniform4fv(uniformLocs->at(9),  1, glm::value_ptr(this->diffuse));
    glUniform4fv(uniformLocs->at(10), 1, glm::value_ptr(this->specular));
    glUniform1f (uniformLocs->at(11), this->shininess);

    // Setup options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(md.invertBackface ? GL_CW : GL_CCW);

    // Draw model
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.indices);
    glDrawElements(GL_TRIANGLES, md.vertexCount, GL_UNSIGNED_INT, 0);
}