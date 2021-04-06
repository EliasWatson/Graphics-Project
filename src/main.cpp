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
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "util.hpp"
#include "scene.hpp"
#include "importers/scene_importer.hpp"
#include "importers/texture_importer.hpp"

// Constants
const double fpsUpdateFrequency = 0.1; // In seconds
const double cameraSpeed = 2.5;
const double lookSensitivity = 0.1;

// Globals
scene* mainScene = nullptr;
bool mouseEnabled = false;
double lastMouseX, lastMouseY;
double frameTimeSum = 0.0, frameCount = 0.0;

// Prototypes
void init(GLFWwindow* window);
void window_resize(GLFWwindow* window, int width, int height);
void display(GLFWwindow* window, double currentTime);
void processInput(GLFWwindow* window, double deltaTime);
void processMouse(GLFWwindow* window, double x, double y);

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

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    ImGui::StyleColorsDark();

    // Setup performance overlay options
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoNav
        | ImGuiWindowFlags_NoMove;

    // Render
    glfwSwapInterval(0);
    init(window);

    ImGuiViewport* viewport;

    double lastFrame = 0.0, currentFrame;
    double deltaTime, guiDeltaTime;
    double timeToUpdate = 0.0;

    while(!glfwWindowShouldClose(window)) {
        // Calculate frametime
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        timeToUpdate -= deltaTime;
        if(timeToUpdate < 0.0) {
            timeToUpdate = fpsUpdateFrequency;
            guiDeltaTime = deltaTime;

            if(currentFrame > 5.0) {
                frameTimeSum += deltaTime;
                frameCount += 1.0;
            }
        }

        // Process input
        processInput(window, deltaTime);

        // Initialize ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); 

        // Render scene
        display(window, glfwGetTime());

        // Create performance overlay
        viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10.0f, viewport->WorkPos.y + 10.0f), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.35f);

        if(ImGui::Begin("Performance", NULL, windowFlags)) {
            ImGui::Text("Performance");
            ImGui::Separator();
            ImGui::Text("FPS: %.02f", float(1.0 / guiDeltaTime));
            ImGui::Text("Avg FPS: %.02f", float(frameCount / frameTimeSum));
            ImGui::Text("Frametime: %.02f ms", float(guiDeltaTime * 1000.0f));
            ImGui::Text("Avg Frametime: %.02f ms", float((frameTimeSum / frameCount) * 1000.0f));
        }
        ImGui::End();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Draw frame to screen & check input
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void init(GLFWwindow* window) {
    mainScene = new scene(scene::DEFAULT);
    importScene(mainScene, "../../assets/scenes/cash_register/cash_register.gltf", SCENE_IMPORT_CAMERA_PARENT);

    mainScene->env.loadTextures("../../assets/textures/environment/oberer_kuhberg/");
    mainScene->env.intensity = 1.5;

    mainScene->rootNode->updateWorldPosition(glm::mat4(1.0f));
    camera* cam = &mainScene->cameras[mainScene->mainCamera];

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    window_resize(window, width, height);
    glfwSetWindowSizeCallback(window, window_resize);

    glfwSetCursorPosCallback(window, processMouse);
}

void window_resize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if(mainScene != nullptr) mainScene->resize(width, height);
}

void display(GLFWwindow* window, double currentTime) {
    // Render scene
    if(mainScene != nullptr) {
        // TEMP
        camera* cam = &mainScene->cameras[mainScene->mainCamera];
        double animationTime = currentTime * 0.5;
        cam->parentEntity->pos.x = cos(animationTime) * 2.0;
        cam->parentEntity->pos.y = 1.0;
        cam->parentEntity->pos.z = sin(animationTime) * 2.0;
        cam->yaw = ((fmod(animationTime, 3.1415 * 2.0) / (3.1415 * 2.0)) * 360.0) + 180.0;

        mainScene->render(float(currentTime));
        mainScene->renderGUI();
    }
}

void processInput(GLFWwindow* window, double deltaTime) {
    camera* cam = &mainScene->cameras[mainScene->mainCamera];
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camSide = glm::normalize(glm::cross(cam->direction, camUp));

    float speed = float(cameraSpeed * deltaTime);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam->parentEntity->pos += glm::vec4(cam->direction * speed, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam->parentEntity->pos -= glm::vec4(cam->direction * speed, 0.0f);

    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam->parentEntity->pos += glm::vec4(camSide * speed, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam->parentEntity->pos -= glm::vec4(camSide * speed, 0.0f);

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cam->parentEntity->pos += glm::vec4(camUp * speed, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cam->parentEntity->pos -= glm::vec4(camUp * speed, 0.0f);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, 1);

    static bool lastMousePressed = false;
    bool mousePressed = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    if(!lastMousePressed && mousePressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseEnabled = true;
    } else if(lastMousePressed && !mousePressed) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        mouseEnabled = false;
    }

    lastMousePressed = mousePressed;
}

void processMouse(GLFWwindow* window, double x, double y) {
    double xOffset = x - lastMouseX;
    double yOffset = y - lastMouseY;

    lastMouseX = x;
    lastMouseY = y;

    if(mouseEnabled) {
        xOffset *= lookSensitivity;
        yOffset *= lookSensitivity;

        camera* cam = &mainScene->cameras[mainScene->mainCamera];
        cam->yaw += float(xOffset);
        cam->pitch -= float(yOffset);

        if(cam->pitch >  89.0f) cam->pitch =  89.0f;
        if(cam->pitch < -89.0f) cam->pitch = -89.0f;
    }
}