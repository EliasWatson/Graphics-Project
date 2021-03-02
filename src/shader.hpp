#ifndef H_SHADER
#define H_SHADER

#include "texture.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct shader_source {
    GLenum type;
    const char* path;
};

struct shader_attribute {
    GLenum bufferType;
    int location;
    int size;
    int stride = 0;
    GLenum type = GL_FLOAT;
    bool normalize = false;
};

struct shader_uniform {
    enum uniform_type {
        FLOAT, MAT4, VEC4, VEC4_ARR
    };

    uniform_type type;
    const char* name;
};

struct shader {
    GLuint id;
    std::vector<shader_attribute> attributes;

    bool compiled = false;

    shader() { }
    shader(
        std::vector<shader_source> src,
        std::vector<shader_attribute> attributes
    );

    void use(std::vector<GLuint> vbo);

    void setFloat(const char* name, float val);
    void setVec4(const char* name, glm::vec4 val);
    void setMat4(const char* name, glm::mat4 val);

    void setFloat(const char* name, GLsizei count, float* val);
    void setVec4(const char* name, GLsizei count, glm::f32* val);
    void setMat4(const char* name, GLsizei count, glm::f32* val);

private:
    bool loadShader(GLuint* id, GLenum type, const char* src);
    bool loadShaderFromFile(GLuint* id, GLenum type, const char* path);
    bool createShaderProgram(GLuint* id, GLuint* shaders, int numShaders);
};

#endif