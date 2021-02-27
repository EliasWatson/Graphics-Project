#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void camera::resize(int width, int height) {
    this->width = width;
    this->height = height;

    this->aspect = (float) width / (float) height;
    this->pMat = glm::perspective(glm::radians(this->fov), this->aspect, this->clipNear, this->clipFar);
}

void camera::calculateViewMatrix() {
    this->vMat = glm::translate(this->dir, this->pos * -1.0f);
}