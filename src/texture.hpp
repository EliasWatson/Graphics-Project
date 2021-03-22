#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

struct shader;

const std::string texture_type_names[6] = {
    "albedo",
    "roughness",
    "metal",
    "normal",
    "irradiance",
    "reflection"
};

const GLenum texture_gl_types[6] = {
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP
};

struct texture {
    enum type {
        ALBEDO      = 0,
        ROUGHNESS   = 1,
        METAL       = 2,
        NORMAL      = 3,
        IRRADIANCE  = 4,
        REFLECTION  = 5,
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