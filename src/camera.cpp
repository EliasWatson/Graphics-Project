#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void camera::updateViewMat() {
    this->vMat = glm::lookAt(this->pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void camera::resize(int width, int height) {
    this->width = width;
    this->height = height;

    this->aspect = (float) width / (float) height;
    this->pMat = glm::perspective(glm::radians(this->fov), this->aspect, this->clipNear, this->clipFar);
}