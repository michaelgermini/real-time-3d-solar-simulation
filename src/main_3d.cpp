#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <chrono>

#include "Engine/Renderer.h"
#include "Engine/Camera.h"
#include "Engine/Light.h"
#include "Engine/Scene.h"
#include "Components/Skybox.h"
#include "Components/Building.h"
#include "Components/SolarPanel.h"
#include "Components/Landscape.h"
#include "Utils/FileUtils.h"
#include "Utils/MathUtils.h"

// Global variables
GLFWwindow* window;
std::unique_ptr<Renderer> renderer;
std::unique_ptr<Camera> camera;
std::unique_ptr<Scene> scene;
std::shared_ptr<Light> sunLight;

// Input handling
bool keys[1024];
bool mouseButtons[8];
double lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Camera settings
float cameraSpeed = 5.0f;
float mouseSensitivity = 0.1f;

// Solar panel simulation
std::shared_ptr<SolarPanel> solarArray;
float simulationTime = 0.0f;

// Function declarations
void InitializeGLFW();
void InitializeOpenGL();
void SetupScene();
void ProcessInput();
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void UpdateSolarPanelSimulation();
void DisplayPerformanceInfo();
void Cleanup();

int main() {
    std::cout << "Real-Time 3D Solar Panel Simulation with OpenGL" << std::endl;
    std::cout << "===============================================" << std::endl;

    // Initialize GLFW
    InitializeGLFW();
    
    // Initialize OpenGL
    InitializeOpenGL();
    
    // Setup scene
    SetupScene();
    
    std::cout << "Solar Panel Simulation Started!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move camera" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Scroll - Zoom in/out" << std::endl;
    std::cout << "  F1 - Toggle performance overlay" << std::endl;
    std::cout << "  F2 - Toggle wireframe mode" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << std::endl;
    
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Process input
        ProcessInput();
        
        // Update solar panel simulation
        UpdateSolarPanelSimulation();
        
        // Update scene
        scene->Update(deltaTime);
        
        // Render scene
        renderer->BeginFrame();
        renderer->Render(*scene, *camera);
        renderer->EndFrame();
        
        // Display performance info
        DisplayPerformanceInfo();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    Cleanup();
    
    return 0;
}

void InitializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA
    
    window = glfwCreateWindow(1280, 720, "Solar Panel 3D Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    
    // Enable cursor capture for first-person camera
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InitializeOpenGL() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }
    
    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
    
    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Initialize renderer
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    renderer = std::make_unique<Renderer>(width, height);
    renderer->Initialize();
}

void SetupScene() {
    // Create camera
    camera = std::make_unique<Camera>(glm::vec3(0.0f, 10.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    camera->SetFOV(45.0f);
    camera->SetNearPlane(0.1f);
    camera->SetFarPlane(1000.0f);
    
    // Create scene
    scene = std::make_unique<Scene>();
    scene->SetAmbientLight(glm::vec3(0.1f, 0.1f, 0.1f));
    
    // Create sun light
    sunLight = Light::CreateDirectionalLight(
        glm::vec3(-0.5f, -1.0f, -0.5f),
        glm::vec3(1.0f, 0.95f, 0.8f)
    );
    sunLight->SetIntensity(1.0f);
    sunLight->SetAmbient(0.1f);
    sunLight->SetDiffuse(0.8f);
    sunLight->SetSpecular(1.0f);
    sunLight->EnableShadows(true);
    sunLight->SetShadowMapSize(2048);
    
    scene->AddLight(sunLight);
    
    // Create skybox
    auto skybox = std::make_shared<Skybox>(Skybox::SkyType::CLEAR_DAY);
    skybox->SetTimeOfDay(0.5f); // Noon
    scene->SetSkybox(skybox);
    
    // Create landscape
    auto landscape = std::make_shared<Landscape>(Landscape::TerrainType::HILLY, 
                                                glm::vec2(1000.0f, 1000.0f), 256);
    landscape->SetHeightScale(50.0f);
    landscape->GenerateGeometry();
    scene->AddModel(landscape->GetModel());
    
    // Create buildings
    auto building1 = std::make_shared<Building>(Building::BuildingType::OFFICE,
                                               glm::vec3(-50.0f, 0.0f, -50.0f),
                                               glm::vec3(20.0f, 30.0f, 20.0f));
    building1->SetHeight(30.0f);
    building1->SetFloorCount(10);
    building1->GenerateGeometry();
    scene->AddModel(building1->GetModel());
    
    auto building2 = std::make_shared<Building>(Building::BuildingType::RESIDENTIAL,
                                               glm::vec3(50.0f, 0.0f, -30.0f),
                                               glm::vec3(15.0f, 25.0f, 15.0f));
    building2->SetHeight(25.0f);
    building2->SetFloorCount(8);
    building2->GenerateGeometry();
    scene->AddModel(building2->GetModel());
    
    // Create solar panel array
    solarArray = std::make_shared<SolarPanel>(SolarPanel::PanelType::MONOCRYSTALLINE,
                                             glm::vec3(0.0f, 5.0f, 50.0f),
                                             glm::vec2(2.0f, 1.0f));
    solarArray->SetTilt(30.0f);
    solarArray->SetAzimuth(180.0f); // South facing
    solarArray->SetEfficiency(0.22f);
    solarArray->SetPowerOutput(400.0f); // 400W panels
    solarArray->CreateArray(10, 20, 3.0f); // 10x20 array with 3m spacing
    solarArray->GenerateGeometry();
    scene->AddModel(solarArray->GetModel());
    
    std::cout << "Scene setup complete" << std::endl;
}

void ProcessInput() {
    if (keys[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, true);
    }
    
    // Camera movement
    float velocity = cameraSpeed * deltaTime;
    
    if (keys[GLFW_KEY_W]) {
        camera->ProcessKeyboard(Camera::CameraMovement::FORWARD, deltaTime);
    }
    if (keys[GLFW_KEY_S]) {
        camera->ProcessKeyboard(Camera::CameraMovement::BACKWARD, deltaTime);
    }
    if (keys[GLFW_KEY_A]) {
        camera->ProcessKeyboard(Camera::CameraMovement::LEFT, deltaTime);
    }
    if (keys[GLFW_KEY_D]) {
        camera->ProcessKeyboard(Camera::CameraMovement::RIGHT, deltaTime);
    }
    if (keys[GLFW_KEY_SPACE]) {
        camera->ProcessKeyboard(Camera::CameraMovement::UP, deltaTime);
    }
    if (keys[GLFW_KEY_LEFT_SHIFT]) {
        camera->ProcessKeyboard(Camera::CameraMovement::DOWN, deltaTime);
    }
    
    // Performance overlay toggle
    static bool f1Pressed = false;
    if (keys[GLFW_KEY_F1] && !f1Pressed) {
        // Toggle performance overlay
        f1Pressed = true;
    }
    if (!keys[GLFW_KEY_F1]) {
        f1Pressed = false;
    }
    
    // Wireframe toggle
    static bool f2Pressed = false;
    if (keys[GLFW_KEY_F2] && !f2Pressed) {
        // Toggle wireframe mode
        static bool wireframe = false;
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        f2Pressed = true;
    }
    if (!keys[GLFW_KEY_F2]) {
        f2Pressed = false;
    }
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera->ProcessMouseMovement(xoffset, yoffset, true);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera->ProcessMouseScroll(yoffset);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button >= 0 && button < 8) {
        if (action == GLFW_PRESS) {
            mouseButtons[button] = true;
        } else if (action == GLFW_RELEASE) {
            mouseButtons[button] = false;
        }
    }
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (renderer) {
        renderer->SetViewport(width, height);
    }
    if (camera) {
        camera->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }
}

void UpdateSolarPanelSimulation() {
    if (!solarArray) return;
    
    // Update simulation time
    simulationTime += deltaTime;
    
    // Update solar panel
    solarArray->Update(deltaTime);
    
    // Animate sun position based on time
    float timeOfDay = fmod(simulationTime / 86400.0f, 1.0f); // 24-hour cycle
    float sunAngle = timeOfDay * 2.0f * glm::pi<float>();
    
    // Update sun light position
    float sunHeight = 100.0f;
    float sunRadius = 200.0f;
    glm::vec3 sunPosition(
        sunRadius * cos(sunAngle),
        sunHeight * sin(sunAngle),
        sunRadius * sin(sunAngle)
    );
    
    sunLight->SetPosition(sunPosition);
    sunLight->SetDirection(glm::normalize(-sunPosition));
    
    // Update skybox time of day
    if (scene->GetSkybox()) {
        scene->GetSkybox()->SetTimeOfDay(timeOfDay);
    }
}

void DisplayPerformanceInfo() {
    static float performanceTimer = 0.0f;
    performanceTimer += deltaTime;
    
    if (performanceTimer >= 1.0f) {
        std::cout << "\rFPS: " << renderer->GetFPS() 
                  << " | Draw Calls: " << renderer->GetDrawCalls();
        
        if (solarArray) {
            std::cout << " | Solar Power: " << solarArray->GetCurrentPower() << "W"
                      << " | Energy: " << solarArray->GetEnergyGenerated() << "kWh"
                      << " | Temp: " << solarArray->GetTemperature() << "°C";
        }
        
        std::cout << "    " << std::flush;
        performanceTimer = 0.0f;
    }
}

void Cleanup() {
    glfwTerminate();
    std::cout << std::endl << "Solar Panel Simulation ended." << std::endl;
}
