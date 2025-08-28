#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <string>

// Solar Panel class with click functionality
class SolarPanel {
public:
    enum class PanelType { MONOCRYSTALLINE, POLYCRYSTALLINE, THIN_FILM, BIFACIAL };
    
    SolarPanel(PanelType type, const glm::vec3& pos, const glm::vec2& sz)
        : type(type), position(pos), size(sz), efficiency(0.22f), powerOutput(400.0f),
          temperature(25.0f), dirtLevel(0.0f), tilt(30.0f), azimuth(180.0f),
          dailyEnergyOutput(0.0f), age(0.0f), health(1.0f), isSelected(false) {
        GenerateGeometry();
        InitializeRandom();
    }
    
    void Update(float deltaTime, float timeOfDay, float solarIntensity, float ambientTemp) {
        // Temperature simulation
        float targetTemp = ambientTemp + solarIntensity * 0.5f;
        temperature = temperature + (targetTemp - temperature) * deltaTime * 0.1f;
        
        // Dirt accumulation
        dirtLevel = std::min(dirtLevel + deltaTime * 0.001f, 1.0f);
        
        // Angle-based efficiency
        float angleEfficiency = CalculateAngleEfficiency(timeOfDay);
        
        // Calculate power output
        float basePower = powerOutput * (1.0f - temperature * 0.004f);
        float dirtEffect = 1.0f - dirtLevel * 0.3f;
        currentPower = basePower * angleEfficiency * dirtEffect * health * solarIntensity;
        
        // Energy accumulation
        dailyEnergyOutput += currentPower * deltaTime / 3600.0f;
        
        // Age and degradation
        age += deltaTime / 86400.0f;
        health = std::max(0.5f, 1.0f - age * 0.001f);
    }
    
    void Render(const glm::mat4& viewProjection) {
        glUseProgram(shaderProgram);
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(size.x, 0.1f, size.y));
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        // Set color based on panel state
        glm::vec3 color = CalculateColor();
        if (isSelected) {
            color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow for selected panel
        }
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    // Getters for production data
    float GetCurrentPower() const { return currentPower; }
    float GetDailyEnergy() const { return dailyEnergyOutput; }
    float GetTemperature() const { return temperature; }
    float GetEfficiency() const { return efficiency; }
    float GetDirtLevel() const { return dirtLevel; }
    float GetHealth() const { return health; }
    float GetAge() const { return age; }
    PanelType GetType() const { return type; }
    glm::vec3 GetPosition() const { return position; }
    
    // Selection methods
    void SetSelected(bool selected) { isSelected = selected; }
    bool IsSelected() const { return isSelected; }
    
    // Check if ray intersects with this panel
    bool IntersectsRay(const glm::vec3& rayOrigin, const glm::vec3& rayDirection) const {
        // Transform ray to panel's local space
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 invModel = glm::inverse(model);
        glm::vec3 localRayOrigin = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 localRayDir = glm::vec3(invModel * glm::vec4(rayDirection, 0.0f));
        
        // Panel bounds in local space
        glm::vec3 minBounds = glm::vec3(-size.x * 0.5f, -0.05f, -size.y * 0.5f);
        glm::vec3 maxBounds = glm::vec3(size.x * 0.5f, 0.05f, size.y * 0.5f);
        
        // Ray-box intersection
        float tMin = (minBounds.x - localRayOrigin.x) / localRayDir.x;
        float tMax = (maxBounds.x - localRayOrigin.x) / localRayDir.x;
        
        if (tMin > tMax) std::swap(tMin, tMax);
        
        float tyMin = (minBounds.y - localRayOrigin.y) / localRayDir.y;
        float tyMax = (maxBounds.y - localRayOrigin.y) / localRayDir.y;
        
        if (tyMin > tyMax) std::swap(tyMin, tyMax);
        
        if (tMin > tyMax || tyMin > tMax) return false;
        
        if (tyMin > tMin) tMin = tyMin;
        if (tyMax < tMax) tMax = tyMax;
        
        float tzMin = (minBounds.z - localRayOrigin.z) / localRayDir.z;
        float tzMax = (maxBounds.z - localRayOrigin.z) / localRayDir.z;
        
        if (tzMin > tzMax) std::swap(tzMin, tzMax);
        
        if (tMin > tzMax || tzMin > tMax) return false;
        
        if (tzMin > tMin) tMin = tzMin;
        if (tzMax < tMax) tMax = tzMax;
        
        return tMax > 0.0f;
    }
    
private:
    PanelType type;
    glm::vec3 position;
    glm::vec2 size;
    float efficiency, powerOutput, currentPower;
    float temperature, dirtLevel, tilt, azimuth;
    float dailyEnergyOutput, age, health;
    bool isSelected;
    
    GLuint VAO, VBO, EBO, shaderProgram;
    
    void GenerateGeometry() {
        // Create shaders
        const char* vertexShaderSource = R"(
            #version 430 core
            layout (location = 0) in vec3 aPos;
            
            uniform mat4 model;
            uniform mat4 viewProjection;
            
            void main() {
                gl_Position = viewProjection * model * vec4(aPos, 1.0);
            }
        )";

        const char* fragmentShaderSource = R"(
            #version 430 core
            out vec4 FragColor;
            
            uniform vec3 color;
            
            void main() {
                FragColor = vec4(color, 1.0);
            }
        )";

        // Compile shaders
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        // Create shader program
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        // Clean up shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Create cube geometry
        float vertices[] = {
            // Front face
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            // Back face
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
        };

        unsigned int indices[] = {
            0, 1, 2, 2, 3, 0,  // Front
            1, 5, 6, 6, 2, 1,  // Right
            5, 4, 7, 7, 6, 5,  // Back
            4, 0, 3, 3, 7, 4,  // Left
            3, 2, 6, 6, 7, 3,  // Top
            4, 5, 1, 1, 0, 4   // Bottom
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
    
    void InitializeRandom() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.8f, 1.2f);
        
        efficiency *= dis(gen);
        powerOutput *= dis(gen);
        temperature += dis(gen) * 10.0f - 5.0f;
    }
    
    float CalculateAngleEfficiency(float timeOfDay) {
        float sunAngle = timeOfDay * 2.0f * 3.14159f;
        float sunHeight = sin(sunAngle);
        float panelAngle = glm::radians(tilt);
        float angleDiff = abs(sunHeight - sin(panelAngle));
        return std::max(0.1f, 1.0f - angleDiff);
    }
    
    glm::vec3 CalculateColor() {
        // Base color based on panel type
        glm::vec3 baseColor;
        switch (type) {
            case PanelType::MONOCRYSTALLINE: baseColor = glm::vec3(0.2f, 0.4f, 0.8f); break;
            case PanelType::POLYCRYSTALLINE: baseColor = glm::vec3(0.3f, 0.5f, 0.9f); break;
            case PanelType::THIN_FILM: baseColor = glm::vec3(0.4f, 0.6f, 0.7f); break;
            case PanelType::BIFACIAL: baseColor = glm::vec3(0.5f, 0.7f, 0.6f); break;
        }
        
        // Temperature effect
        float tempFactor = std::min(1.0f, temperature / 80.0f);
        baseColor = glm::mix(baseColor, glm::vec3(1.0f, 0.3f, 0.0f), tempFactor * 0.3f);
        
        // Dirt effect
        baseColor = glm::mix(baseColor, glm::vec3(0.3f, 0.3f, 0.3f), dirtLevel * 0.4f);
        
        // Health effect
        baseColor = glm::mix(baseColor, glm::vec3(0.5f, 0.5f, 0.5f), (1.0f - health) * 0.2f);
        
        return baseColor;
    }
};

// Camera class
class Camera {
public:
    Camera() : position(0.0f, 10.0f, 20.0f), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f),
               yaw(-90.0f), pitch(0.0f), movementSpeed(5.0f), mouseSensitivity(0.1f), zoom(45.0f) {
        UpdateCameraVectors();
    }
    
    void ProcessKeyboard(int direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        if (direction == 0) position += front * velocity;      // Forward
        if (direction == 1) position -= front * velocity;      // Backward
        if (direction == 2) position -= right * velocity;      // Left
        if (direction == 3) position += right * velocity;      // Right
        if (direction == 4) position += up * velocity;         // Up
        if (direction == 5) position -= up * velocity;         // Down
    }
    
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;
        
        yaw += xoffset;
        pitch += yoffset;
        
        if (constrainPitch) {
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }
        
        UpdateCameraVectors();
    }
    
    void ProcessMouseScroll(float yoffset) {
        zoom -= yoffset;
        if (zoom < 1.0f) zoom = 1.0f;
        if (zoom > 45.0f) zoom = 45.0f;
    }
    
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }
    
    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(zoom), 1200.0f / 800.0f, 0.1f, 100.0f);
    }
    
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetFront() const { return front; }
    
private:
    glm::vec3 position, front, up, right, worldUp;
    float yaw, pitch, movementSpeed, mouseSensitivity, zoom;
    
    void UpdateCameraVectors() {
        glm::vec3 front_vec;
        front_vec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front_vec.y = sin(glm::radians(pitch));
        front_vec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(front_vec);
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, front));
    }
};

// Global variables
Camera camera;
std::vector<SolarPanel> panels;
SolarPanel* selectedPanel = nullptr;
bool firstMouse = true;
float lastX = 600.0f, lastY = 400.0f;

// Mouse callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Mouse button callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Get mouse position in normalized device coordinates
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        // Convert to NDC
        float x = (2.0f * xpos) / 1200.0f - 1.0f;
        float y = 1.0f - (2.0f * ypos) / 800.0f;
        
        // Create ray in view space
        glm::vec4 rayStart_NDC(x, y, -1.0f, 1.0f);
        glm::vec4 rayEnd_NDC(x, y, 0.0f, 1.0f);
        
        // Transform to world space
        glm::mat4 invProjection = glm::inverse(camera.GetProjectionMatrix());
        glm::mat4 invView = glm::inverse(camera.GetViewMatrix());
        
        glm::vec4 rayStart_World = invView * invProjection * rayStart_NDC;
        rayStart_World /= rayStart_World.w;
        
        glm::vec4 rayEnd_World = invView * invProjection * rayEnd_NDC;
        rayEnd_World /= rayEnd_World.w;
        
        glm::vec3 rayDirection = glm::normalize(glm::vec3(rayEnd_World - rayStart_World));
        glm::vec3 rayOrigin = camera.GetPosition();
        
        // Deselect previous panel
        if (selectedPanel) {
            selectedPanel->SetSelected(false);
            selectedPanel = nullptr;
        }
        
        // Check intersection with panels
        for (auto& panel : panels) {
            if (panel.IntersectsRay(rayOrigin, rayDirection)) {
                panel.SetSelected(true);
                selectedPanel = &panel;
                
                // Display panel information
                std::cout << "\n=== PANEL SELECTED ===" << std::endl;
                std::cout << "Current Power: " << panel.GetCurrentPower() << " W" << std::endl;
                std::cout << "Daily Energy: " << panel.GetDailyEnergy() << " kWh" << std::endl;
                std::cout << "Temperature: " << panel.GetTemperature() << " °C" << std::endl;
                std::cout << "Efficiency: " << panel.GetEfficiency() * 100 << "%" << std::endl;
                std::cout << "Dirt Level: " << panel.GetDirtLevel() * 100 << "%" << std::endl;
                std::cout << "Health: " << panel.GetHealth() * 100 << "%" << std::endl;
                std::cout << "Age: " << panel.GetAge() << " days" << std::endl;
                
                switch (panel.GetType()) {
                    case SolarPanel::PanelType::MONOCRYSTALLINE: std::cout << "Type: Monocrystalline" << std::endl; break;
                    case SolarPanel::PanelType::POLYCRYSTALLINE: std::cout << "Type: Polycrystalline" << std::endl; break;
                    case SolarPanel::PanelType::THIN_FILM: std::cout << "Type: Thin Film" << std::endl; break;
                    case SolarPanel::PanelType::BIFACIAL: std::cout << "Type: Bifacial" << std::endl; break;
                }
                std::cout << "=====================" << std::endl;
                break;
            }
        }
    }
}

// Scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// Key callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Solar Panel Simulation - Click panels to see production data", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1200, 800);
    
    // Create solar panels
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            SolarPanel::PanelType type = static_cast<SolarPanel::PanelType>(rand() % 4);
            glm::vec3 pos(i * 3.0f - 6.0f, 0.0f, j * 3.0f - 6.0f);
            glm::vec2 size(2.0f, 1.5f);
            panels.emplace_back(type, pos, size);
        }
    }
    
    std::cout << "Solar Panel Simulation with Click Functionality" << std::endl;
    std::cout << "===============================================" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move camera" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Scroll - Zoom in/out" << std::endl;
    std::cout << "  Left Click - Select panel and view production data" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;
    std::cout << std::endl;
    
    // Main loop
    auto lastFrame = std::chrono::high_resolution_clock::now();
    float simulationTime = 0.0f;
    
    while (!glfwWindowShouldClose(window)) {
        auto currentFrame = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrame - lastFrame).count();
        lastFrame = currentFrame;
        
        simulationTime += deltaTime;
        
        // Process input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.ProcessKeyboard(0, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.ProcessKeyboard(1, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.ProcessKeyboard(2, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.ProcessKeyboard(3, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) camera.ProcessKeyboard(4, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) camera.ProcessKeyboard(5, deltaTime);
        
        // Update simulation
        float timeOfDay = fmod(simulationTime / 86400.0f, 1.0f); // 24-hour cycle
        float solarIntensity = std::max(0.0f, sin(timeOfDay * 3.14159f) * 0.8f + 0.2f);
        float ambientTemp = 20.0f + sin(timeOfDay * 2.0f * 3.14159f) * 10.0f;
        
        for (auto& panel : panels) {
            panel.Update(deltaTime, timeOfDay, solarIntensity, ambientTemp);
        }
        
        // Render
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 viewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        
        for (auto& panel : panels) {
            panel.Render(viewProjection);
        }
        
        // Display performance info
        static float performanceTimer = 0.0f;
        performanceTimer += deltaTime;
        if (performanceTimer >= 2.0f) {
            float totalPower = 0.0f;
            for (auto& panel : panels) {
                totalPower += panel.GetCurrentPower();
            }
            std::cout << "\rTotal Power: " << totalPower << "W | FPS: " << 1.0f/deltaTime << "    " << std::flush;
            performanceTimer = 0.0f;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    std::cout << std::endl << "Simulation ended successfully!" << std::endl;
    return 0;
}
