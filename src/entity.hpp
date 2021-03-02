#ifndef H_ENTITY
#define H_ENTITY

#include "camera.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <stack>

struct scene;

struct entity {
    glm::mat4 modelMatrix;
    std::vector<int> meshIndices;
    std::vector<entity> children;

    entity() { }
    int updateMatrixStack(std::stack<glm::mat4>* matrixStack);
    void render(scene* s, camera* cam);
};

#endif