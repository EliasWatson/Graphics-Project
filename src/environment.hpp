#ifndef H_ENVIRONMENT
#define H_ENVIRONMENT

#include "texture.hpp"
#include "mesh.hpp"
#include "shader.hpp"

#include "glm/glm.hpp"

struct environment {
    texture irradiance, reflection;
    float intensity = 1.0f;

    shader skyboxShader;
    mesh skybox;

    environment();
    void loadTextures(std::string rootDir);
    void render(glm::mat4 pMat, glm::mat4 vMat);

private:
    void loadCubemap(std::string texDir, GLuint* id);
    void loadCubemapSide(std::string path, GLuint id, GLenum side);
};

#endif