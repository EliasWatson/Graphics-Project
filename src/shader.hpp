#ifndef H_SHADER
#define H_SHADER

#include <GL/glew.h>

bool loadShader(GLuint* id, GLenum type, const char* src);
bool loadShaderFromFile(GLuint* id, GLenum type, const char* path);
bool createShaderProgram(GLuint* id, GLuint* shaders, int numShaders);

#endif