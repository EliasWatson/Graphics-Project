#include "entity.hpp"

#include "mesh.hpp"
#include "scene.hpp"
#include "camera.hpp"

#include <imgui.h>
#include <glm/ext/matrix_transform.hpp>

void entity::setModelMatrix(glm::mat4 inMat) {
    this->scale = glm::vec4(inMat[0].length(), inMat[1].length(), inMat[2].length(), 1.0f);
    inMat *= glm::scale(glm::mat4(1.0f), 1.0f / glm::vec3(this->scale));

    this->pos = inMat[3];

    // TODO: Extract rotation

    this->modelMatrix = inMat;
}

glm::mat4 entity::applyLocalTransform(glm::mat4 inMat) {
    this->modelMatrix[3] = this->pos;
    return inMat * this->modelMatrix * glm::scale(glm::mat4(1.0f), glm::vec3(this->scale));
}

void entity::updateWorldPosition(glm::mat4 inMat) {
    glm::mat4 localMat = this->applyLocalTransform(inMat);
    this->worldPos = localMat[3];

    for(entity* e : this->children) {
        e->updateWorldPosition(localMat);
    }
}

void entity::renderShadowmap(scene* s, glm::mat4 parentMat) {
    glm::mat4 localMat = this->applyLocalTransform(parentMat);

    for(int meshIndex : this->meshIndices) {
        mesh* m = &s->meshes[meshIndex];
        s->env.renderShadowmap(m, localMat);
    }

    for(entity* child : this->children) {
        child->renderShadowmap(s, localMat);
    }
}

void entity::render(scene* s, camera* cam, glm::mat4 parentMat) {
    glm::mat4 localMat = this->applyLocalTransform(parentMat);

    for(int meshIndex : this->meshIndices) {
        mesh* m = &s->meshes[meshIndex];
        s->materials[m->materialIndex].render({
            // TODO: Replace this with just the mesh pointer
            m->vao,
            m->vbo,
            m->ebo,
            m->vertexCount,
            m->invertBackface
        }, {
            cam->parentEntity->worldPos,
            cam->pMat,
            cam->vMat,
            localMat,
            glm::transpose(glm::inverse(localMat)),
            0.0f
        }, s->lightData, s->env);
    }

    for(entity* child : this->children) {
        child->render(s, cam, localMat);
    }
}

void entity::renderGUI() {
    if(ImGui::TreeNode(this->name.c_str())) {
        for(entity* child : this->children) {
            child->renderGUI();
        }

        ImGui::DragFloat3("Position", &this->pos[0]);
        ImGui::DragFloat3("Scale", &this->scale[0]);
        ImGui::DragFloat3("World Position", &this->worldPos[0], ImGuiSliderFlags_NoInput);

        if(this->parent != nullptr) {
            if(ImGui::Button("Delete")) {
                this->parent->killChild(this);
            }
        }

        ImGui::TreePop();
    }
}

void entity::killChild(entity* child) {
    child->destroy();
    delete child;

    auto iter = this->children.begin();
    while(iter != this->children.end()) {
        if(*iter == child) {
            this->children.erase(iter);
            iter = this->children.begin();
        } else {
            iter++;
        }
    }
}

void entity::destroy() {
    for(entity* e : this->children) {
        e->destroy();
        delete e;
    }
}