#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string>

shader::shader(
    std::vector<shader_source> src,
    std::vector<shader_attribute> attributes
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
}

void shader::use(std::vector<GLuint> vbo) {
    glUseProgram(this->id);

    for(int i = 0; i < this->attributes.size(); ++i) {
        shader_attribute sa = this->attributes[i];
        glBindBuffer(sa.bufferType, vbo[i]);
        glVertexAttribPointer(sa.location, sa.size, sa.type, sa.normalize ? GL_TRUE : GL_FALSE, sa.stride, 0);
        glEnableVertexAttribArray(sa.location);
    }
}

void shader::setFloat(const char* name, float val) {
    GLuint loc = glGetUniformLocation(this->id, name);
    glUniform1f(loc, val);
}

void shader::setVec4(const char* name, glm::vec4 val) {
    GLuint loc = glGetUniformLocation(this->id, name);
    glUniform4fv(loc, 1, glm::value_ptr(val));
}

void shader::setMat4(const char* name, glm::mat4 val) {
    GLuint loc = glGetUniformLocation(this->id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void shader::setFloat(const char* name, GLsizei count, float* val) {
    GLuint loc = glGetUniformLocation(this->id, name);
    glUniform1fv(loc, count, val);
}

void shader::setVec4(const char* name, GLsizei count, glm::f32* val) {
    GLuint loc = glGetUniformLocation(this->id, name);
    glUniform4fv(loc, count, val);
}

void shader::setMat4(const char* name, GLsizei count, glm::f32* val) {
    GLuint loc = glGetUniformLocation(this->id, name);
    glUniformMatrix4fv(loc, count, GL_FALSE, val);
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
    stream.close();
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