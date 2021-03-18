#ifndef H_ENVIRONMENT
#define H_ENVIRONMENT

#include "texture.hpp"

struct environment {
    texture irradiance, reflection;
    float intensity = 1.0f;
};

#endif