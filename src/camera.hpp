#ifndef H_CAMERA
#define H_CAMERA

#include <glm/glm.hpp>

struct camera {
    glm::mat4 pMat, vMat;
    int width, height;
    float fov, aspect;
    float clipNear, clipFar;

    camera() { }

    void resize(int width, int height);
};

#endif