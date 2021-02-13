#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <string>

shader::shader(
    std::vector<shader_source> src,
    std::vector<shader_attribute> attributes,
    std::vector<shader_uniform> uniforms
) {
    std::vector<GLuint> compiledShaders;
    for(shader_source s : src) {
        GLuint sid;
        if(!loadShaderFromFile(&sid, s.type, s.path)) {
            return;
        }
        compiledShaders.push_back(sid);
    }

    if(!createShaderProgram(&this->id, &compiledShaders[0], (int) src.size())) {
        return;
    }

    compiled = true;
    this->attributes = attributes;
    this->uniforms = uniforms;
    this->uniformLocs = std::vector<GLuint>(uniforms.size());
}

std::vector<GLuint>* shader::use(std::vector<GLuint> vbo, std::vector<shader_texture> textures) {
    glUseProgram(this->id);

    if(!this->updatedThisFrame) {
        for(int i = 0; i < this->uniformLocs.size(); ++i) {
            shader_uniform uniform = this->uniforms[i];
            this->uniformLocs[i] = glGetUniformLocation(this->id, uniform.name);
        }
        this->updatedThisFrame = true;
    }

    for(int i = 0; i < this->attributes.size(); ++i) {
        shader_attribute sa = this->attributes[i];
        glBindBuffer(sa.bufferType, vbo[i]);
        glVertexAttribPointer(sa.location, sa.size, sa.type, sa.normalize ? GL_TRUE : GL_FALSE, sa.stride, 0);
        glEnableVertexAttribArray(sa.location);
    }

    for(shader_texture st : textures) {
        glActiveTexture(st.slot);
        glBindTexture(GL_TEXTURE_2D, st.id);
    }

    return &this->uniformLocs;
}

void shader::reset() {
    this->updatedThisFrame = false;
}

bool shader::loadShader(GLuint* id, GLenum type, const char* src) {
    // Create shader
    *id = glCreateShader(type);

    // Compile shader
    glShaderSource(*id, 1, &src, NULL);
    glCompileShader(*id);

    // Check for compile errors
    GLint result = GL_FALSE;
    int infoLogLength = 0;

    glGetShaderiv(*id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(*id, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> errorMessage(infoLogLength + 1);
        glGetShaderInfoLog(*id, infoLogLength, NULL, &errorMessage[0]);
        fprintf(stderr, "%s\n", &errorMessage[0]);
        return false;
    }

    return true;
}

bool shader::loadShaderFromFile(GLuint* id, GLenum type, const char* path) {
    std::ifstream stream(path);
    std::string source(
        (std::istreambuf_iterator<char>(stream)),
        (std::istreambuf_iterator<char>())
    );
    return loadShader(id, type, source.c_str());
}

bool shader::createShaderProgram(GLuint* id, GLuint* shaders, int numShaders) {
    // Create Program
    *id = glCreateProgram();

    // Attach Shaders
    for(int i = 0; i < numShaders; ++i) {
        glAttachShader(*id, shaders[i]);
    }

    // Link Program
    glLinkProgram(*id);

    // Check for linking errors
    GLint result = GL_FALSE;
    int infoLogLength = 0;

    glGetProgramiv(*id, GL_LINK_STATUS, &result);
    glGetProgramiv(*id, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0) {
        std::vector<char> errorMessage(infoLogLength + 1);
        glGetProgramInfoLog(*id, infoLogLength, NULL, &errorMessage[0]);
        fprintf(stderr, "%s\n", &errorMessage[0]);
        return false;
    }

    return true;
}