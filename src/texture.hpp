#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

struct shader;

const GLenum texture_gl_types[6] = {
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_2D
};

struct texture {
    enum type {
        ALBEDO          = 0,
        ROUGHNESS_METAL = 1,
        NORMAL          = 2,
        IRRADIANCE      = 3,
        REFLECTION      = 4,
        SHADOW          = 5,
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