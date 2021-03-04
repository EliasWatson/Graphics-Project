#ifndef H_CAMERA
#define H_CAMERA

#include "entity.hpp"

#include <glm/glm.hpp>

struct camera {
    entity* parentEntity = nullptr;

    float pitch = 0.0f, yaw = -90.0f;
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::mat4 pMat, vMat;

    int width, height;
    float fov, aspect;
    float clipNear, clipFar;

    camera() { }

    void updateViewMat();
    void resize(int width, int height);
};

#endif