#ifndef H_TEXTURE
#define H_TEXTURE

#include <GL/glew.h>
#include <string>

struct texture {
    GLuint id;
    std::string type;

    texture() { }
    texture(GLuint id, std::string type);
};

#endif