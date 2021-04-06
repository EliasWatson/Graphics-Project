#include "environment.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <SOIL2.h>
#include "util.hpp"

const float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

environment::environment() {
    // Load mesh
    GLuint vao[1];
    GLuint vbo[1];

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(1, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

    skybox.materialIndex = -1;
    skybox.vbo = std::vector<GLuint>(vbo, vbo + 1);
    skybox.ebo = 0;
    skybox.vertexCount = (GLsizei) ((sizeof(skyboxVertices) / sizeof(float)) / 3);

    this->loadShadowShader();
    this->loadSkyboxShader();

    this->setupShadowBuffer(1, 1);
}

void environment::loadTextures(std::string rootDir) {
    // Load cubemaps
    GLuint irradianceID, reflectionID;
    this->loadCubemap(rootDir + "irradiance/", &irradianceID);
    this->loadCubemap(rootDir + "reflection/", &reflectionID);
    this->irradiance = texture(irradianceID, texture::type::IRRADIANCE);
    this->reflection = texture(reflectionID, texture::type::REFLECTION);
}

void environment::startShadowmapRender() {
    // Update matrices
    this->shadowVMat = glm::lookAt(
        this->sunDir * this->sunDist,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    this->shadowPMat = glm::perspective(
        glm::radians(this->sunFOV),
        float(this->shadowWidth) / float(this->shadowHeight),
        this->sunClipNear, this->sunClipFar
    );

    // Setup framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->shadowBuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->shadowTex.id, 0);
    glDrawBuffer(GL_NONE);
    glEnable(GL_DEPTH_TEST);

    // Clear framebuffer
    glClear(GL_DEPTH_BUFFER_BIT);
}

void environment::render(glm::mat4 pMat, glm::mat4 vMat) {
    // Load shader
    this->skyboxShader.use(this->skybox.vbo);
    glBindVertexArray(this->skybox.vao);

    // Load textures
    texture::clearTextureContrib(&this->skyboxShader);
    reflection.loadTexture(&this->skyboxShader);
    glActiveTexture(GL_TEXTURE0);

    // Copy to uniforms
    this->skyboxShader.setMat4("proj_matrix", pMat);
    this->skyboxShader.setMat4("v_matrix", vMat);
    this->skyboxShader.setFloat("sky_brightness", this->intensity);

    // Setup options
    glDepthMask(GL_FALSE);
    glFrontFace(GL_CCW);

    // Reduce shadow artifacts
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(2.0f, 4.0f);

    // Draw model
    glDrawArrays(GL_TRIANGLES, 0, this->skybox.vertexCount);

    // Cleanup
    glDepthMask(GL_TRUE);
}

void environment::renderShadowmap(mesh* m, glm::mat4 mMat) {
    // Load shader
    this->shadowShader.use(m->vbo);
    glBindVertexArray(m->vao);

    // Calculate Matrices
    glm::mat4 shadowMVP = this->shadowPMat * this->shadowVMat * mMat;

    // Copy to uniforms
    this->shadowShader.setMat4("shadowMVP", shadowMVP);

    // Setup options
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Draw model
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ebo);
    glDrawElements(GL_TRIANGLES, m->vertexCount, GL_UNSIGNED_INT, 0);
}

glm::mat4 environment::getShadowMatrix(glm::mat4 mMat) {
    return this->b * this->shadowPMat * this->shadowVMat * mMat;
}

void environment::loadSkyboxShader() {
    std::vector<shader_source> src;
    src.push_back({GL_VERTEX_SHADER, "../../assets/shaders/skybox.vsh"});
    src.push_back({GL_FRAGMENT_SHADER, "../../assets/shaders/skybox.fsh"});

    std::vector<shader_attribute> attributes;
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        0,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        false               // bool normalize = false;
    });

    this->skyboxShader = shader(src, attributes);
    if(!this->skyboxShader.compiled) exit(EXIT_FAILURE);
}

void environment::loadShadowShader() {
    std::vector<shader_source> src;
    src.push_back({GL_VERTEX_SHADER, "../../assets/shaders/shadow.vsh"});
    src.push_back({GL_FRAGMENT_SHADER, "../../assets/shaders/shadow.fsh"});

    std::vector<shader_attribute> attributes;
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        0,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        false               // bool normalize = false;
    });

    this->shadowShader = shader(src, attributes);
    if(!shadowShader.compiled) exit(EXIT_FAILURE);
}

void environment::loadCubemap(std::string texDir, GLuint* id) {
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, id);

    this->loadCubemapSide(texDir + "px.png", *id, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    this->loadCubemapSide(texDir + "nx.png", *id, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    this->loadCubemapSide(texDir + "py.png", *id, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    this->loadCubemapSide(texDir + "ny.png", *id, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    this->loadCubemapSide(texDir + "pz.png", *id, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
    this->loadCubemapSide(texDir + "nz.png", *id, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void environment::loadCubemapSide(std::string path, GLuint id, GLenum side) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    int width, height, channels;
    unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &channels, 4);

    glTexImage2D(
        side,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    free(data);
}

void environment::setupShadowBuffer(int width, int height) {
    this->shadowWidth = width;
    this->shadowHeight = height;

    glGenFramebuffers(1, &this->shadowBuffer);

    // TODO: Delete old data

    GLuint shadowTexID;
    glGenTextures(1, &shadowTexID);
    glBindTexture(GL_TEXTURE_2D, shadowTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
        width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

    this->shadowTex = texture(shadowTexID, texture::type::SHADOW);
}