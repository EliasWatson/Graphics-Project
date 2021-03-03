#ifndef H_LIGHT
#define H_LIGHT

#include "entity.hpp"

#include <glm/glm.hpp>

struct light {
    entity* parentEntity;
    glm::vec4 ambient, diffuse, specular;
};

#endif