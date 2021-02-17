#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <stack>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "light.hpp"
#include "model_importer.hpp"

// Constants
float empty_4f[4] = {0.0, 0.0, 0.0, 1.0};

// Globals
float cameraFOV;
float cameraX, cameraY, cameraZ;
std::vector<mesh> meshes;
std::vector<light> lights;

texture brickAlbedoTexture, brickRoughnessTexture;
shader shaderProgram;
material mat;

int width, height;
float aspect;

glm::mat4 pMat, vMat;
std::stack<glm::mat4> mvStack;
material::light_data lightData;

// Prototypes
void init(GLFWwindow* window);
void window_resize(GLFWwindow* window, int width, int height);
void display(GLFWwindow* window, double currentTime);

void loadShaders();
void createMaterials();
void createMeshes();
void createLights();

// Methods
int main() {
    // Initialize GLFW
    if(!glfwInit()) exit(EXIT_FAILURE);

    // Create GLFWwindow
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Graphics Project", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if(glewInit() != GLEW_OK) exit(EXIT_FAILURE);

    // Render
    glfwSwapInterval(1);
    init(window);

    while(!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void init(GLFWwindow* window) {
    cameraFOV = 60.0f;

    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 3.0f;

    brickAlbedoTexture = texture("../../assets/textures/PavingStones070_1K_Color.jpg");
    if(brickAlbedoTexture.id == 0) exit(EXIT_FAILURE);

    brickRoughnessTexture = texture("../../assets/textures/PavingStones070_1K_Roughness.jpg");
    if(brickRoughnessTexture.id == 0) exit(EXIT_FAILURE);

    loadShaders();
    createMaterials();
    createMeshes();
    createLights();

    glfwGetFramebufferSize(window, &width, &height);
    window_resize(window, width, height);
    glfwSetWindowSizeCallback(window, window_resize);
}

void window_resize(GLFWwindow* window, int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;

    // Set render resolution
    glViewport(0, 0, width, height);

    // Build perspective matrix
    aspect = (float) width / (float) height;
    pMat = glm::perspective(glm::radians(cameraFOV), aspect, 0.1f, 1000.0f);
}

void display(GLFWwindow* window, double currentTime) {
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    // Build view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvStack.push(vMat);

    // Update lights
    lights[0].pos[0] = cosf(currentTime) * 5.0;

    // Update meshes
    //meshes[0].rotation = (float) currentTime;

    // Build light data
    // TODO: Make this async
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        if(i < lights.size()) {
            glm::vec4 l_pos = mvStack.top() * lights[i].pos;

            COPY_VEC4(lightData.pos[i],      l_pos);
            COPY_VEC4(lightData.ambient[i],  lights[i].ambient);
            COPY_VEC4(lightData.diffuse[i],  lights[i].diffuse);
            COPY_VEC4(lightData.specular[i], lights[i].specular);
        } else {
            COPY_VEC4(lightData.pos[i],      empty_4f);
            COPY_VEC4(lightData.ambient[i],  empty_4f);
            COPY_VEC4(lightData.diffuse[i],  empty_4f);
            COPY_VEC4(lightData.specular[i], empty_4f);
        }
    }

    // Render meshes
    for(mesh m : meshes) {
        int matrixCount = m.render(&mvStack, pMat, (float) currentTime, lightData);
        for(; matrixCount > 1; --matrixCount) mvStack.pop();
    }
}

void loadShaders() {
    std::vector<shader_source> src;
    src.push_back({GL_VERTEX_SHADER, "../../assets/shaders/basic.vsh"});
    src.push_back({GL_FRAGMENT_SHADER, "../../assets/shaders/basic.fsh"});

    std::vector<shader_attribute> attributes;
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        0,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        false               // bool normalize = false;
    });
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        1,                  // int location;
        2,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        false               // bool normalize = false;
    });
    attributes.push_back({
        GL_ARRAY_BUFFER,    // GLenum bufferType;
        2,                  // int location;
        3,                  // int size;
        0,                  // int stride = 0;
        GL_FLOAT,           // GLenum type = GL_FLOAT;
        true                // bool normalize = false;
    });

    std::vector<shader_uniform> uniforms;
    uniforms.push_back({shader_uniform::MAT4, "proj_matrix"});
    uniforms.push_back({shader_uniform::MAT4, "mv_matrix"});
    uniforms.push_back({shader_uniform::MAT4, "norm_matrix"});
    uniforms.push_back({shader_uniform::FLOAT, "tf"});

    uniforms.push_back({shader_uniform::VEC4_ARR, "light_pos"});
    uniforms.push_back({shader_uniform::VEC4_ARR, "light_ambient"});
    uniforms.push_back({shader_uniform::VEC4_ARR, "light_diffuse"});
    uniforms.push_back({shader_uniform::VEC4_ARR, "light_specular"});

    uniforms.push_back({shader_uniform::VEC4,  "ambient_color"});
    uniforms.push_back({shader_uniform::VEC4,  "diffuse_color"});
    uniforms.push_back({shader_uniform::VEC4,  "specular_color"});
    uniforms.push_back({shader_uniform::FLOAT, "shininess_factor"});

    shaderProgram = shader(src, attributes, uniforms);
    if(!shaderProgram.compiled) exit(EXIT_FAILURE);
}

void createMaterials() {
    mat = material(shaderProgram);
    mat.textures.push_back({brickAlbedoTexture.id, GL_TEXTURE0});
    mat.textures.push_back({brickRoughnessTexture.id, GL_TEXTURE1});

    mat.ambient   = glm::vec4(0.2, 0.2, 0.2, 1);
    mat.diffuse   = glm::vec4(1.0, 1.0, 1.0, 1);
    mat.specular  = glm::vec4(1.0, 1.0, 1.0, 1);
    mat.shininess = 51.2;
}

void createMeshes() {
    // Load suzanne mesh
    mesh suzanne;
    const char* suzannePath = "../../assets/models/suzanne.obj";
    if(!importModel(suzannePath, &suzanne)) {
        printf("failed to load model '%s'\n", suzannePath);
        exit(EXIT_FAILURE);
    }

    suzanne.mat = mat;
    suzanne.rotationAxis = glm::normalize(glm::vec3(1.0f, 0.75f, 0.5f));

    meshes.push_back(suzanne);
}

void createLights() {
    lights.push_back({
        glm::vec4(5.0, 2.0, 2.0, 1.0),
        glm::vec4(0.2, 0.2, 0.2, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0),
        glm::vec4(1.0, 1.0, 1.0, 1.0)
    });
}