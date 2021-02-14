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

#include "texture.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "model_importer.hpp"

// Globals
float cameraFOV;
float cameraX, cameraY, cameraZ;
std::vector<mesh> meshes;

texture brickTexture;
shader shaderProgram;

int width, height;
float aspect;

glm::mat4 pMat, vMat;
std::stack<glm::mat4> mvStack;

// Prototypes
void init(GLFWwindow* window);
void window_resize(GLFWwindow* window, int width, int height);
void display(GLFWwindow* window, double currentTime);

void loadShaders();
void createMeshes();

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

    brickTexture = texture("../../assets/textures/PavingStones070_1K_Color.jpg");
    if(brickTexture.id == 0) exit(EXIT_FAILURE);

    loadShaders();
    createMeshes();

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

    // Update meshes
    meshes[0].rotation = (float) currentTime;

    // Render meshes
    for(mesh m : meshes) {
        int matrixCount = m.render(&mvStack, pMat, (float) currentTime);
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
    uniforms.push_back({shader_uniform::MAT4, "rot_matrix"});
    uniforms.push_back({shader_uniform::MAT4, "mv_matrix"});
    uniforms.push_back({shader_uniform::MAT4, "proj_matrix"});
    uniforms.push_back({shader_uniform::FLOAT, "tf"});

    shaderProgram = shader(src, attributes, uniforms);
    if(!shaderProgram.compiled) exit(EXIT_FAILURE);
}

void createMeshes() {
    // Load suzanne mesh
    mesh suzanne;
    const char* suzannePath = "../../assets/models/suzanne.obj";
    if(!importModel(suzannePath, &suzanne)) {
        printf("failed to load model '%s'\n", suzannePath);
        exit(EXIT_FAILURE);
    }

    suzanne.shaderProgram = shaderProgram;
    suzanne.rotationAxis = glm::normalize(glm::vec3(1.0f, 0.75f, 0.5f));
    suzanne.textures.push_back({brickTexture.id, GL_TEXTURE0});

    meshes.push_back(suzanne);
}