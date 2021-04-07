#ifndef H_ENVIRONMENT
#define H_ENVIRONMENT

#include "texture.hpp"
#include "mesh.hpp"
#include "shader.hpp"

#include "glm/glm.hpp"

struct environment {
    // Shading
    texture irradiance, reflection;
    float intensity = 1.0f;

    // Skybox
    shader skyboxShader;
    mesh skybox;

    // Shadows
    glm::vec3 sunDir = glm::vec3(0.0, -1.0, 0.0); // Default to straight up, so under the map
    float shadowSize = 2.0f;
    float shadowBias = 0.000005f;
    float shadowBlur = 2.5f;
    bool shadowOrtholinear = true;
    float sunDist = 10.0f;
    float sunFOV = 15.0f;
    float sunClipNear = 0.1f, sunClipFar = 1000.0f;

    shader shadowShader;
    int shadowWidth, shadowHeight;
    texture shadowTex;
    GLuint shadowBuffer;
    glm::mat4 shadowVMat, shadowPMat;
    glm::mat4 b = glm::mat4(
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    );

    environment();
    void loadTextures(std::string rootDir);
    void render(glm::mat4 pMat, glm::mat4 vMat);

    void setupShadowBuffer(int width, int height);
    void startShadowmapRender();
    void renderShadowmap(mesh* m, glm::mat4 mMat);
    glm::mat4 getShadowMatrix(glm::mat4 mMat);

private:
    void loadSkyboxShader();
    void loadShadowShader();

    void loadCubemap(std::string texDir, GLuint* id);
    void loadCubemapSide(std::string path, GLuint id, GLenum side);
};

#endif