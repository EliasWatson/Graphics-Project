#ifndef H_LIGHT
#define H_LIGHT

#include <glm/glm.hpp>

struct light {
    glm::vec4 pos;
    glm::vec4 ambient, diffuse, specular;
};

#endif