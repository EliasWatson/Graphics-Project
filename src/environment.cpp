#include "environment.hpp"

#include <SOIL2.h>

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

    // Load shader
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

void environment::loadTextures(std::string rootDir) {
    // Load cubemaps
    GLuint irradianceID, reflectionID;
    this->loadCubemap(rootDir + "irradiance/", &irradianceID);
    this->loadCubemap(rootDir + "reflection/", &reflectionID);
    this->irradiance = texture(irradianceID, texture::type::IRRADIANCE);
    this->reflection = texture(reflectionID, texture::type::REFLECTION);
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

    // Draw model
    glDrawArrays(GL_TRIANGLES, 0, this->skybox.vertexCount);

    // Cleanup
    glDepthMask(GL_TRUE);
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