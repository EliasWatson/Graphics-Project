#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

struct material;

const std::string texture_type_names[5] = {
    "albedo",
    "roughness",
    "normal",
    "irradiance",
    "reflection"
};

const GLenum texture_gl_types[5] = {
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D
};

struct texture {
    enum type {
        ALBEDO      = 0,
        ROUGHNESS   = 1,
        NORMAL      = 2,
        IRRADIANCE  = 3,
        REFLECTION  = 4,
        NUM_TYPES
    };

    GLuint id;
    type tex_type;

    texture() { }
    texture(GLuint id, type tex_type);

    void loadTexture(material* mat);
    static void clearTextureContrib(material* mat);
};

#endif