#include "environment.hpp"

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