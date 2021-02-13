#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <stack>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"

// Constants
#define NUM_VAO 1
#define NUM_VBO 2
#define NUM_CUBES 10000

// Globals
float cameraFOV;
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float pyraLocX, pyraLocY, pyraLocZ;

GLuint renderingProgram;
GLuint vao[NUM_VAO];
GLuint vbo[NUM_VBO];

GLuint mvLoc, projLoc, tfLoc;
int width, height;
float aspect;

glm::mat4 pMat, vMat;
std::stack<glm::mat4> mvStack;

// Prototypes
void init(GLFWwindow* window);
void window_resize(GLFWwindow* window, int width, int height);
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

    cameraX =  0.0f;
    cameraY =  0.0f;
    cameraZ = 10.0f;

    cubeLocX =  0.0f;
    cubeLocY = -2.0f;
    cubeLocZ =  0.0f;

    pyraLocX = 3.0f;
    pyraLocY = 3.0f;
    pyraLocZ = 0.0f;

    renderingProgram = createShaderProgram();
    createVertices();

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
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    tfLoc = glGetUniformLocation(renderingProgram, "tf");

    // Build view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
    mvStack.push(vMat);

    {
        // Build matrices
        mvStack.push(mvStack.top());
        mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        mvStack.push(mvStack.top());
        mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime, glm::vec3(1.0f, 0.0f, 0.0f));

        // Copy to uniforms
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
        glUniform1f(tfLoc, float(currentTime));

        // Bind VBO to shader vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // Draw model
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        // Remove rotation
        mvStack.pop();
    }

    {
        // Build matrices
        mvStack.push(mvStack.top());
        mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float) currentTime) * 4.0f, 0.0f, cos((float) currentTime) * 4.0f));
        mvStack.push(mvStack.top());
        mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime, glm::vec3(0.0f, 1.0f, 0.0f));

        // Copy to uniforms
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
        glUniform1f(tfLoc, float(currentTime));

        // Bind VBO to shader vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // Draw model
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CW);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Remove rotation
        mvStack.pop();
    }

    {
        // Build matrices
        mvStack.push(mvStack.top());
        mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float) currentTime) * 2.0f, cos((float) currentTime) * 2.0f));
        mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float) currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
        mvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));

        // Copy to uniforms
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
        glUniform1f(tfLoc, float(currentTime));

        // Bind VBO to shader vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        // Draw model
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CW);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Remove transformation
        mvStack.pop();
    }

    while(!mvStack.empty()) mvStack.pop();
}

void createVertices() {
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