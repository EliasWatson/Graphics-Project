#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void camera::updateViewMat() {
    this->direction.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->direction.y = sin(glm::radians(this->pitch));
    this->direction.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->direction = glm::normalize(this->direction);

    this->vMat = glm::lookAt(
        glm::vec3(this->parentEntity->worldPos),
        glm::vec3(this->parentEntity->worldPos) + this->direction,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

void camera::resize(int width, int height) {
    this->width = width;
    this->height = height;

    this->aspect = (float) width / (float) height;
    this->pMat = glm::perspective(glm::radians(this->fov), this->aspect, this->clipNear, this->clipFar);
}