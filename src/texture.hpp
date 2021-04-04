#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

struct shader;

const std::string texture_type_names[5] = {
    "albedo",
    "roughness_metal",
    "normal",
    "irradiance",
    "reflection"
};

const GLenum texture_gl_types[5] = {
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP
};

struct texture {
    enum type {
        ALBEDO          = 0,
        ROUGHNESS_METAL = 1,
        NORMAL          = 2,
        IRRADIANCE      = 3,
        REFLECTION      = 4,
        NUM_TYPES
    };

    GLuint id;
    type tex_type;

    texture() { }
    texture(GLuint id, type tex_type);

    void loadTexture(shader* shdr);
    static void clearTextureContrib(shader* shdr);
};

#endif