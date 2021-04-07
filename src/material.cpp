#include "material.hpp"

#include <glm/gtc/type_ptr.hpp>

void material::render(model_data md, params p, light_data l, environment env) {
    // Load shader
    this->shaderProgram.use(md.vbo);
    glBindVertexArray(md.vao);

    // Load textures
    texture::clearTextureContrib(&this->shaderProgram);
    for(texture tex : this->textures) tex.loadTexture(&this->shaderProgram);
    env.irradiance.loadTexture(&this->shaderProgram);
    env.reflection.loadTexture(&this->shaderProgram);
    env.shadowTex.loadTexture(&this->shaderProgram);
    glActiveTexture(GL_TEXTURE0);

    // Copy to uniforms
    this->shaderProgram.setVec4("cam_pos", p.camPos);
    this->shaderProgram.setFloat("tf", p.time);
    this->shaderProgram.setVec2("screen_resolution", glm::vec2(p.screenWidth, p.screenHeight));
    this->shaderProgram.setVec2("shadow_resolution", glm::vec2(env.shadowWidth, env.shadowHeight));

    this->shaderProgram.setMat4("proj_matrix",      p.perspective);
    this->shaderProgram.setMat4("view_matrix",      p.view);
    this->shaderProgram.setMat4("model_matrix",     p.model);
    this->shaderProgram.setMat4("inv_model_matrix", p.invModel);
    this->shaderProgram.setMat4("shadow_matrix",    env.getShadowMatrix(p.model));

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