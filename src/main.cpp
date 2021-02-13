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

#include "shader.hpp"
#include "mesh.hpp"

// Constants
#define NUM_VAO 1
#define NUM_VBO 2
#define NUM_CUBES 10000

// Globals
float cameraFOV;
float cameraX, cameraY, cameraZ;
std::vector<mesh> meshes;

GLuint renderingProgram;
GLuint vao[NUM_VAO];
GLuint vbo[NUM_VBO];

std::vector<GLuint> uniformLocs;
int width, height;
float aspect;

glm::mat4 pMat, vMat;
std::stack<glm::mat4> mvStack;

// Prototypes
void init(GLFWwindow* window);
void window_resize(GLFWwindow* window, int width, int height);
void display(GLFWwindow* window, double currentTime);

void createMeshes();
GLuint createShaderProgram();

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

    cameraX =  0.0f;
    cameraY =  0.0f;
    cameraZ = 10.0f;

    renderingProgram = createShaderProgram();
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

    // Load shader
    glUseProgram(renderingProgram);

    while(uniformLocs.size() < 3) uniformLocs.push_back(0);
    uniformLocs[0] = glGetUniformLocation(renderingProgram, "mv_matrix");
    uniformLocs[1] = glGetUniformLocation(renderingProgram, "proj_matrix");
    uniformLocs[2] = glGetUniformLocation(renderingProgram, "tf");

    // Build view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvStack.push(vMat);

    // Update meshes
    meshes[0].rotation = (float) currentTime;

    meshes[1].position = glm::vec3(sin((float) currentTime) * 4.0f, 0.0f, cos((float) currentTime) * 4.0f);
    meshes[1].rotation = (float) currentTime;

    meshes[2].position = glm::vec3(0.0f, sin((float) currentTime) * 2.0f, cos((float) currentTime) * 2.0f);
    meshes[2].rotation = (float) currentTime;

    // Render meshes
    for(mesh m : meshes) {
        int matrixCount = m.render(&uniformLocs, &mvStack, pMat, (float) currentTime);
        for(; matrixCount > 1; --matrixCount) mvStack.pop();
    }
}

void createMeshes() {
    float cubePositions[108] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,
         1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
    };

    float pyramidPositions[54] = {
        -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f
    };

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(NUM_VBO, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositions), cubePositions, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

    // Create pyramid sun
    mesh pyramid = mesh();

    pyramid.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
    pyramid.vbo = vbo[1];
    pyramid.vertexCount = sizeof(pyramidPositions) / sizeof(float);
    pyramid.shader = renderingProgram;

    meshes.push_back(pyramid);

    // Create cube planet
    mesh cubePlanet = mesh();

    cubePlanet.vbo = vbo[0];
    cubePlanet.vertexCount = sizeof(cubePositions) / sizeof(float);
    cubePlanet.shader = renderingProgram;
    cubePlanet.invertBackface = true;

    meshes.push_back(cubePlanet);

    // Create cube moon
    mesh cubeMoon = mesh();
    cubeMoon.rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
    cubeMoon.scale = glm::vec3(0.25f, 0.25f, 0.25f);

    cubeMoon.vbo = vbo[0];
    cubeMoon.vertexCount = sizeof(cubePositions) / sizeof(float);
    cubeMoon.shader = renderingProgram;
    cubeMoon.invertBackface = true;

    meshes.push_back(cubeMoon);
}

GLuint createShaderProgram() {
    const char* vshPath = "../../assets/shaders/basic.vsh";
    const char* fshPath = "../../assets/shaders/basic.fsh";

    GLuint shaders[2];
    if(!loadShaderFromFile(&shaders[0], GL_VERTEX_SHADER, vshPath)
        || !loadShaderFromFile(&shaders[1], GL_FRAGMENT_SHADER, fshPath)) {
        exit(EXIT_FAILURE);
        return 0; // Hopefully unreachable
    }

    GLuint program;
    if(!createShaderProgram(&program, shaders, 2)) {
        exit(EXIT_FAILURE);
        return 0; // Hopefully unreachable
    }

    return program;
}