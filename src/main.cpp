#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

// Constants
#define NUM_VAO 1
#define NUM_VBO 2

// Globals
float cameraFOV;
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;

GLuint renderingProgram;
GLuint vao[NUM_VAO];
GLuint vbo[NUM_VBO];

GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

// Prototypes
void init(GLFWwindow* window);
void display(GLFWwindow* window, double currentTime);

void createVertices();
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

    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 8.0f;

    cubeLocX =  0.0f;
    cubeLocY = -2.0f;
    cubeLocZ =  0.0f;

    renderingProgram = createShaderProgram();
    createVertices();
}

void display(GLFWwindow* window, double currentTime) {
    // Clear buffers
    glClear(GL_DEPTH_BUFFER_BIT);

    // Load shader
    glUseProgram(renderingProgram);
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

    // Build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float) width / (float) height;
    pMat = glm::perspective(glm::radians(cameraFOV), aspect, 0.1f, 1000.0f);

    // Build view matrix, model matrix, and model-view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
    mvMat = vMat * mMat;

    // Copy matrices to uniforms
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

    // Bind VBO to shader vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Draw model
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void createVertices() {
    float vertexPositions[108] = {
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

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    glGenBuffers(NUM_VBO, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
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