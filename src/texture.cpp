#include "texture.hpp"

#include "material.hpp"

const char* texture_type_names_sampler[6] = {
    "albedo_sampler",
    "roughness_metal_sampler",
    "normal_sampler",
    "irradiance_sampler",
    "reflection_sampler",
    "shadow_sampler"
};

const char* texture_type_names_sampler_contrib[6] = {
    "albedo_sampler_contrib",
    "roughness_metal_sampler_contrib",
    "normal_sampler_contrib",
    "irradiance_sampler_contrib",
    "reflection_sampler_contrib",
    "shadow_sampler_contrib"
};

texture::texture(GLuint id, type tex_type) {
    this->id = id;
    this->tex_type = tex_type;
}

void texture::loadTexture(shader* shdr) {
    glActiveTexture(GL_TEXTURE0 + this->tex_type);
    glBindTexture(texture_gl_types[this->tex_type], this->id);
    shdr->setFloat(texture_type_names_sampler[this->tex_type], float(this->tex_type));
    shdr->setFloat(texture_type_names_sampler_contrib[this->tex_type], 1.0f);
}

void texture::clearTextureContrib(shader* shdr) {
    for(int i = 0; i < NUM_TYPES; ++i) {
        shdr->setFloat(texture_type_names_sampler_contrib[i], 0.0f);
    }
}