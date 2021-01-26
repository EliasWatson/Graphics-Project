#include "shader.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

bool loadShader(GLuint* id, GLenum type, const char* src) {
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

bool loadShaderFromFile(GLuint* id, GLenum type, const char* path) {
    std::ifstream stream(path);
    std::string source(
        (std::istreambuf_iterator<char>(stream)),
        (std::istreambuf_iterator<char>())
    );
    return loadShader(id, type, source.c_str());
}

bool createShaderProgram(GLuint* id, GLuint* shaders, int numShaders) {
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