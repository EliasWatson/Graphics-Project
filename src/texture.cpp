#include "texture.hpp"

#include "material.hpp"

texture::texture(GLuint id, type tex_type) {
    this->id = id;
    this->tex_type = tex_type;
}

void texture::loadTexture(material* mat) {
    glActiveTexture(GL_TEXTURE0 + this->tex_type);
    glBindTexture(texture_gl_types[this->tex_type], this->id);
    std::string name = texture_type_names[this->tex_type];
    mat->shaderProgram.setFloat((name + "_sampler").c_str(), float(this->tex_type));
    mat->shaderProgram.setFloat((name + "_sampler_contrib").c_str(), 1.0f);
}

void texture::clearTextureContrib(material* mat) {
    for(int i = 0; i < NUM_TYPES; ++i) {
        mat->shaderProgram.setFloat((texture_type_names[i] + "_sampler_contrib").c_str(), 0.0f);
    }
}