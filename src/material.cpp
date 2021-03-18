#include "material.hpp"

#include <glm/gtc/type_ptr.hpp>

void material::render(model_data md, params p, light_data l, environment env) {
    // Load shader
    this->shaderProgram.use(md.vbo);

    // Load textures
    texture::clearTextureContrib(this);
    for(texture tex : this->textures) tex.loadTexture(this);
    env.irradiance.loadTexture(this);
    env.reflection.loadTexture(this);
    glActiveTexture(GL_TEXTURE0);

    // Copy to uniforms
    this->shaderProgram.setMat4("proj_matrix", p.perspective);
    this->shaderProgram.setMat4("mv_matrix",   p.modelView);
    this->shaderProgram.setMat4("norm_matrix", p.invModelView);
    this->shaderProgram.setFloat("tf", p.time);

    this->shaderProgram.setVec4("light_pos",      MAX_LIGHTS, l.pos[0]);
    this->shaderProgram.setVec4("light_ambient",  MAX_LIGHTS, l.ambient[0]);
    this->shaderProgram.setVec4("light_diffuse",  MAX_LIGHTS, l.diffuse[0]);
    this->shaderProgram.setVec4("light_specular", MAX_LIGHTS, l.specular[0]);

    this->shaderProgram.setVec4("ambient_color",  this->ambient);
    this->shaderProgram.setVec4("diffuse_color",  this->diffuse);
    this->shaderProgram.setVec4("specular_color", this->specular);
    this->shaderProgram.setFloat("shininess_factor", this->shininess);

    this->shaderProgram.setFloat("environment_intensity", env.intensity);

    // Setup options
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(md.invertBackface ? GL_CW : GL_CCW);

    // Draw model
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, md.indices);
    glDrawElements(GL_TRIANGLES, md.vertexCount, GL_UNSIGNED_INT, 0);
}