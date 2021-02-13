#include "texture.hpp"

#include <SOIL2.h>

texture::texture(const char* path) {
    this->id = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
}