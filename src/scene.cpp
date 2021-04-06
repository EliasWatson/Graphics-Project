#include "scene.hpp"

#include "util.hpp"

#include <iostream>
#include <stack>

#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>

// Constants
const float empty_4f[4] = {0.0, 0.0, 0.0, 1.0};

scene::scene(shader_type type) {
    switch(type) {
        case DEFAULT:
        default:
            this->loadDefaultShaders();
            break;
    }
}

void scene::render(float time) {
    // If we don't have a camera, we can't render anything
    if(this->mainCamera < 0 || this->mainCamera >= this->cameras.size()) return;

    // TODO: Call update function
    // this->lights[0].parentEntity->pos.x = cosf(time) * 2.0f;
    // this->lights[0].parentEntity->pos.z = sinf(time) * 2.0f;

    this->rootNode->updateWorldPosition(glm::mat4(1.0f));

    // Get camera
    camera* cam = &this->cameras[this->mainCamera];
    cam->updateViewMat();

    // Update light data
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        if(i < this->lights.size()) {
            COPY_VEC4(this->lightData.pos[i],      this->lights[i].parentEntity->worldPos);
            COPY_VEC4(this->lightData.ambient[i],  this->lights[i].ambient);
            COPY_VEC4(this->lightData.diffuse[i],  this->lights[i].diffuse);
            COPY_VEC4(this->lightData.specular[i], this->lights[i].specular);
        } else {
            COPY_VEC4(this->lightData.pos[i],      empty_4f);
            COPY_VEC4(this->lightData.ambient[i],  empty_4f);
            COPY_VEC4(this->lightData.diffuse[i],  empty_4f);
            COPY_VEC4(this->lightData.specular[i], empty_4f);
        }
    }

    // Render shadowmap
    this->env.startShadowmapRender();
    this->rootNode->renderShadowmap(this, glm::mat4(1.0f));

    // Setup main render pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_FRONT);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    // Render sky
    this->env.render(cam->pMat, cam->vMat);

    // Render entities
    this->rootNode->render(this, cam, glm::mat4(1.0f));
}

void scene::renderGUI() {
    ImGui::Begin("Scene");
    this->rootNode->renderGUI();
    ImGui::End();
}

void scene::resize(int width, int height) {
    if(this->mainCamera >= 0 && this->mainCamera < this->cameras.size()) {
        this->cameras[this->mainCamera].resize(width, height);
    }
}

void scene::loadDefaultShaders() {
    std::vector<shader_source> src;
    src.push_back({GL_VERTEX_SHADER, "../../assets/shaders/basic.vsh"});
    src.push_back({GL_FRAGMENT_SHADER, "../../assets/shaders/basic.fsh"});

    std::vector<shader_attribute> attributes;
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        0,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        false               // bool normalize = false;
    });
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        1,                  // int location;
        2,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        false               // bool normalize = false;
    });
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        2,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        true                // bool normalize = false;
    });
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        3,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        true                // bool normalize = false;
    });

    this->shaderProgram = shader(src, attributes);
    if(!shaderProgram.compiled) exit(EXIT_FAILURE);
}