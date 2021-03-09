#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

struct material;

const std::string texture_type_names[3] = {
    "albedo",
    "roughness",
    "normal"
};

const GLenum texture_gl_types[3] = {
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D
};

struct texture {
    enum type {
        ALBEDO      = 0,
        ROUGHNESS   = 1,
        NORMAL      = 2,
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