#ifndef H_TEXTURE
#define H_TEXTURE

#include "gl/glew.h"

struct texture {
    GLuint id;

    texture() { }
    texture(const char* path);
};

#endif