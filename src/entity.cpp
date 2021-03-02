#include "entity.hpp"

#include "mesh.hpp"
#include "scene.hpp"

int entity::updateMatrixStack(std::stack<glm::mat4>* matrixStack) {
    matrixStack->push(matrixStack->top());
    matrixStack->top() *= this->modelMatrix;

    // Return number of matrices added to the stack
    return 1;
}

void entity::render(scene* s, camera* cam) {
    int matAdded = this->updateMatrixStack(&s->mvStack);

    for(int meshIndex : this->meshIndices) {
        mesh* m = &s->meshes[meshIndex];
        s->materials[m->materialIndex].render({
            m->vbo,
            m->ebo,
            m->vertexCount,
            m->invertBackface
        }, {
            cam->pMat,
            s->mvStack.top(),
            glm::mat4(1.0), // TODO
            0.0f
        }, s->lightData);
    }

    for(entity child : this->children) {
        child.render(s, cam);
    }

    for(int i = 0; i < matAdded; ++i) s->mvStack.pop();
}