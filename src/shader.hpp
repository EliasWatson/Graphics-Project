#ifndef H_SHADER
#define H_SHADER

#include <GL/glew.h>
#include <vector>

struct shader_source {
    GLenum type;
    const char* path;
};

struct shader_uniform {
    enum uniform_type {
        FLOAT, MAT4
    };

    uniform_type type;
    const char* name;
};

struct shader {
    GLuint id;
    std::vector<shader_uniform> uniforms;
    std::vector<GLuint> uniformLocs;

    bool compiled = false;
    bool updatedThisFrame = false;

    shader() { }
    shader(std::vector<shader_source> src, std::vector<shader_uniform> uniforms);

    std::vector<GLuint>* use();
    void reset();

private:
    bool loadShader(GLuint* id, GLenum type, const char* src);
    bool loadShaderFromFile(GLuint* id, GLenum type, const char* path);
    bool createShaderProgram(GLuint* id, GLuint* shaders, int numShaders);
};

#endif