#ifndef H_CAMERA
#define H_CAMERA

#include "entity.hpp"

#include <glm/glm.hpp>

struct camera {
    entity* parentEntity;
    glm::mat4 pMat, vMat;
    int width, height;
    float fov, aspect;
    float clipNear, clipFar;

    camera() { }

    void updateViewMat();
    void resize(int width, int height);
};

#endif