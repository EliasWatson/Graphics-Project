#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

void init(GLFWwindow* window);
void display(GLFWwindow* window, double current_time);
GLuint createShaderProgram();

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
    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window, double current_time) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);
    glPointSize(30.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);
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