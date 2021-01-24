#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void init(GLFWwindow* window);
void display(GLFWwindow* window, double current_time);

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

}

void display(GLFWwindow* window, double current_time) {
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}