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
    this->lights[0].pos.x = cosf(time) * 2.0f;
    this->lights[0].pos.z = sinf(time) * 2.0f;

    // Get camera
    camera* cam = &this->cameras[this->mainCamera];
    cam->updateViewMat();

    // Create Model-View matrix stack
    if(!this->mvStack.empty()) {
        // This shouldn't happen
        std::cerr << "[*] Model-view stack wasn't emptied the last frame\n" << std::endl;
        while(!this->mvStack.empty()) this->mvStack.pop();
    }
    this->mvStack.push(cam->vMat);

    // Update light data
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        if(i < this->lights.size()) {
            glm::vec4 l_pos = mvStack.top() * this->lights[i].pos;

            COPY_VEC4(this->lightData.pos[i],      l_pos);
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

    // Render entities
    this->rootNode.render(this, cam);

    // Pop view matrix
    this->mvStack.pop();
}

void scene::renderGUI() {
    ImGui::Begin("Camera");
    ImGui::SliderFloat3("position", &this->cameras[this->mainCamera].pos[0], -10.0f, 10.0f);
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