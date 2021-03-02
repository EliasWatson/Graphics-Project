#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

const std::string texture_type_names[3] = {
    "albedo",
    "roughness",
    "normal"
};

struct texture {
    enum type {
        ALBEDO,
        ROUGHNESS,
        NORMAL
    };

    GLuint id;
    type tex_type;

    texture() { }
    texture(GLuint id, type tex_type);
};

#endif