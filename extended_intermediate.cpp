#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <random>

// Extended Solar Panel class with advanced features
class ExtendedSolarPanel {
public:
    enum class PanelType {
        MONOCRYSTALLINE,
        POLYCRYSTALLINE,
        THIN_FILM,
        BIFACIAL
    };

    ExtendedSolarPanel(PanelType type, const glm::vec3& pos, const glm::vec2& sz) 
        : type(type), position(pos), size(sz), efficiency(0.22f), powerOutput(400.0f), 
          temperature(25.0f), dirtLevel(0.0f), tilt(30.0f), azimuth(180.0f),
          dailyEnergyOutput(0.0f), monthlyEnergyOutput(0.0f), yearlyEnergyOutput(0.0f), age(0.0f),
          health(1.0f), maintenanceLevel(1.0f) {
        GenerateGeometry();
        InitializeRandom();
    }

    void Update(float deltaTime, float timeOfDay, float solarIntensity, float ambientTemp, float windSpeed) {
        // Advanced temperature simulation with wind cooling
        float solarHeating = solarIntensity * 0.15f;
        float windCooling = windSpeed * 0.02f;
        float cooling = (temperature - ambientTemp) * 0.02f + windCooling;
        temperature = ambientTemp + solarHeating - cooling;
        
        // Advanced dirt accumulation with wind cleaning
        float dirtAccumulation = deltaTime * 0.0005f;
        float windCleaning = windSpeed * deltaTime * 0.0001f;
        dirtLevel += dirtAccumulation - windCleaning;
        if (dirtLevel > 1.0f) dirtLevel = 1.0f;
        if (dirtLevel < 0.0f) dirtLevel = 0.0f;
        
        // Advanced efficiency calculation with multiple factors
        float tempEfficiency = 0.22f - (temperature - 25.0f) * 0.004f;
        float dirtEfficiency = 1.0f - dirtLevel * 0.25f;
        float angleEfficiency = CalculateAngleEfficiency(timeOfDay);
        float degradationEfficiency = 1.0f - (age / 365.0f) * 0.005f;
        float healthEfficiency = health;
        float maintenanceEfficiency = maintenanceLevel;
        
        efficiency = tempEfficiency * dirtEfficiency * angleEfficiency * degradationEfficiency * 
                    healthEfficiency * maintenanceEfficiency;
        if (efficiency < 0.05f) efficiency = 0.05f;
        
        // Power output calculation
        currentPowerOutput = powerOutput * efficiency * solarIntensity;
        
        // Energy tracking
        float energyThisFrame = currentPowerOutput * deltaTime / 3600.0f; // kWh
        dailyEnergyOutput += energyThisFrame;
        monthlyEnergyOutput += energyThisFrame;
        yearlyEnergyOutput += energyThisFrame;
        
        // Age tracking and degradation
        age += deltaTime / 86400.0f; // Days
        
        // Health degradation over time
        health -= deltaTime * 0.00001f; // Very slow degradation
        if (health < 0.5f) health = 0.5f; // Minimum health
        
        // Maintenance level decrease
        maintenanceLevel -= deltaTime * 0.00002f;
        if (maintenanceLevel < 0.7f) maintenanceLevel = 0.7f;
    }

    void Render(const glm::mat4& viewProjection) {
        glUseProgram(shaderProgram);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(size.x, 0.1f, size.y));
        
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewProjection"), 1, GL_FALSE, glm::value_ptr(viewProjection));
        
        // Advanced color coding with multiple factors
        glm::vec3 color = CalculateAdvancedColor();
        
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // Advanced getters
    float GetCurrentPower() const { return currentPowerOutput; }
    float GetTemperature() const { return temperature; }
    float GetEfficiency() const { return efficiency; }
    float GetEnergyGenerated() const { return dailyEnergyOutput; }
    float GetMonthlyEnergy() const { return monthlyEnergyOutput; }
    float GetYearlyEnergy() const { return yearlyEnergyOutput; }
    float GetDirtLevel() const { return dirtLevel; }
    float GetAge() const { return age; }
    float GetHealth() const { return health; }
    float GetMaintenanceLevel() const { return maintenanceLevel; }

    // Advanced actions
    void Clean() { dirtLevel = 0.0f; }
    void Maintain() { maintenanceLevel = 1.0f; }
    void Repair() { health = 1.0f; }
    void SetTilt(float newTilt) { tilt = newTilt; }
    void SetAzimuth(float newAzimuth) { azimuth = newAzimuth; }
    void ResetDailyEnergy() { dailyEnergyOutput = 0.0f; }
    void ResetMonthlyEnergy() { monthlyEnergyOutput = 0.0f; }
    void ResetYearlyEnergy() { yearlyEnergyOutput = 0.0f; }

private:
    PanelType type;
    glm::vec3 position;
    glm::vec2 size;
    float efficiency;
    float powerOutput;
    float temperature;
    float dirtLevel;
    float tilt;
    float azimuth;
    float currentPowerOutput;
    float dailyEnergyOutput;
    float monthlyEnergyOutput;
    float yearlyEnergyOutput;
    float age;
    float health;
    float maintenanceLevel;

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

        // Create cube geometry
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            
            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f
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
    }
    
    void InitializeRandom() {
        // Initialize random number generator for realistic variations
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.8f, 1.2f);
        
        // Add some randomness to panel characteristics
        efficiency *= dis(gen);
        powerOutput *= dis(gen);
    }
    
    float CalculateAngleEfficiency(float timeOfDay) {
        // Calculate sun position (more accurate)
        float sunAngle = timeOfDay * 2.0f * glm::pi<float>();
        glm::vec3 sunDir(cos(sunAngle), sin(sunAngle), 0.0f);
        
        // Calculate panel normal
        glm::vec3 panelNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        panelNormal = glm::rotate(panelNormal, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
        panelNormal = glm::rotate(panelNormal, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Calculate angle between sun and panel
        float cosAngle = glm::dot(sunDir, panelNormal);
        if (cosAngle < 0.0f) cosAngle = 0.0f; // No power when sun is behind panel
        
        return cosAngle;
    }
    
    glm::vec3 CalculateAdvancedColor() {
        // Base color based on panel type
        glm::vec3 baseColor;
        switch (type) {
            case PanelType::MONOCRYSTALLINE:
                baseColor = glm::vec3(0.2f, 0.8f, 0.2f); // Dark green
                break;
            case PanelType::POLYCRYSTALLINE:
                baseColor = glm::vec3(0.4f, 0.6f, 0.2f); // Light green
                break;
            case PanelType::THIN_FILM:
                baseColor = glm::vec3(0.8f, 0.6f, 0.2f); // Brown
                break;
            case PanelType::BIFACIAL:
                baseColor = glm::vec3(0.3f, 0.7f, 0.3f); // Medium green
                break;
        }
        
        // Efficiency-based color variation
        if (efficiency < 0.15f) {
            baseColor = glm::mix(baseColor, glm::vec3(1.0f, 0.3f, 0.0f), 0.7f); // Red when inefficient
        } else if (efficiency > 0.18f) {
            baseColor = glm::mix(baseColor, glm::vec3(0.3f, 1.0f, 0.3f), 0.5f); // Green when efficient
        } else if (efficiency > 0.16f) {
            baseColor = glm::mix(baseColor, glm::vec3(1.0f, 1.0f, 0.3f), 0.3f); // Yellow when good
        }
        
        // Add dirt effect
        baseColor = glm::mix(baseColor, glm::vec3(0.3f, 0.3f, 0.3f), dirtLevel * 0.6f);
        
        // Add health effect
        baseColor = glm::mix(baseColor, glm::vec3(0.5f, 0.5f, 0.5f), (1.0f - health) * 0.3f);
        
        return baseColor;
    }
};

// Extended Camera class with smooth movement
class ExtendedCamera {
public:
    ExtendedCamera() : position(15.0f, 12.0f, 15.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
                       yaw(-45.0f), pitch(-25.0f), speed(8.0f), sensitivity(0.08f), fov(45.0f),
                       smoothFactor(0.1f), targetPosition(position), targetYaw(yaw), targetPitch(pitch) {}

    void ProcessKeyboard(int direction, float deltaTime) {
        float velocity = speed * deltaTime;
        glm::vec3 front = glm::normalize(target - position);
        glm::vec3 right = glm::normalize(glm::cross(front, up));
        
        if (direction == GLFW_KEY_W) targetPosition += front * velocity;
        if (direction == GLFW_KEY_S) targetPosition -= front * velocity;
        if (direction == GLFW_KEY_A) targetPosition -= right * velocity;
        if (direction == GLFW_KEY_D) targetPosition += right * velocity;
        if (direction == GLFW_KEY_SPACE) targetPosition += up * velocity;
        if (direction == GLFW_KEY_LEFT_SHIFT) targetPosition -= up * velocity;
        
        // Smooth camera movement
        position = glm::mix(position, targetPosition, smoothFactor);
        target = position + front;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        targetYaw += xoffset;
        targetPitch += yoffset;

        if (constrainPitch) {
            if (targetPitch > 89.0f) targetPitch = 89.0f;
            if (targetPitch < -89.0f) targetPitch = -89.0f;
        }

        // Smooth camera rotation
        yaw = glm::mix(yaw, targetYaw, smoothFactor);
        pitch = glm::mix(pitch, targetPitch, smoothFactor);

        UpdateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset) {
        fov -= yoffset;
        if (fov < 1.0f) fov = 1.0f;
        if (fov > 90.0f) fov = 90.0f;
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), 1200.0f / 800.0f, 0.1f, 100.0f);
    }

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float fov;
    float smoothFactor;
    
    // Target values for smooth movement
    glm::vec3 targetPosition;
    float targetYaw;
    float targetPitch;

    void UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        target = position + glm::normalize(front);
    }
};

// Global variables
GLFWwindow* window;
std::vector<std::unique_ptr<ExtendedSolarPanel>> solarPanels;
std::unique_ptr<ExtendedCamera> camera;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float simulationTime = 0.0f;

// Weather simulation
float windSpeed = 0.0f;
float weatherTimer = 0.0f;

// Input handling
bool keys[1024];
double lastX = 600, lastY = 400;
bool firstMouse = true;

void ProcessInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(GLFW_KEY_W, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(GLFW_KEY_S, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(GLFW_KEY_A, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(GLFW_KEY_D, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera->ProcessKeyboard(GLFW_KEY_SPACE, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->ProcessKeyboard(GLFW_KEY_LEFT_SHIFT, deltaTime);
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

    camera->ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera->ProcessMouseScroll(yoffset);
}

void UpdateWeather(float deltaTime) {
    weatherTimer += deltaTime;
    
    // Simulate weather changes over time
    if (weatherTimer > 15.0f) { // Change weather every 15 seconds
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> windDis(0.0f, 12.0f);
        
        windSpeed = windDis(gen);
        weatherTimer = 0.0f;
    }
}

int main() {
    std::cout << "Extended 3D Solar Panel Simulation with OpenGL" << std::endl;
    std::cout << "=============================================" << std::endl;

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
    window = glfwCreateWindow(1200, 800, "Extended Solar Panel 3D Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1200, 800);

    // Create camera
    camera = std::make_unique<ExtendedCamera>();

    // Create extended solar panel array (10x10 grid)
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            glm::vec3 position(i * 2.0f - 9.0f, 0.0f, j * 2.0f - 9.0f);
            
            // Random panel type for variety
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> typeDis(0, 3);
            
            ExtendedSolarPanel::PanelType type = static_cast<ExtendedSolarPanel::PanelType>(typeDis(gen));
            
            solarPanels.push_back(std::make_unique<ExtendedSolarPanel>(
                type, 
                position, 
                glm::vec2(1.5f, 1.0f)
            ));
        }
    }

    std::cout << "Extended Solar Panel Simulation Running!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move camera" << std::endl;
    std::cout << "  Space/Shift - Move up/down" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Scroll - Zoom in/out" << std::endl;
    std::cout << "  ESC - Exit" << std::endl;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        simulationTime += deltaTime;

        ProcessInput();
        UpdateWeather(deltaTime);

        // Calculate time of day and solar intensity
        float timeOfDay = fmod(simulationTime / 86400.0f, 1.0f); // 24-hour cycle
        float solarIntensity = 0.0f;
        float ambientTemp = 20.0f;
        
        if (timeOfDay > 0.2f && timeOfDay < 0.8f) {
            // Day time
            float noonTime = 0.5f;
            float timeFromNoon = abs(timeOfDay - noonTime);
            solarIntensity = 1.0f - (timeFromNoon / 0.3f) * (timeFromNoon / 0.3f);
            if (solarIntensity < 0.0f) solarIntensity = 0.0f;
            
            // Temperature variation
            ambientTemp = 20.0f + 15.0f * sin(timeOfDay * 2.0f * glm::pi<float>());
        }

        // Update solar panels with weather
        for (auto& panel : solarPanels) {
            panel->Update(deltaTime, timeOfDay, solarIntensity, ambientTemp, windSpeed);
        }

        // Clear
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get matrices
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix();
        glm::mat4 viewProjection = projection * view;

        // Render solar panels
        for (auto& panel : solarPanels) {
            panel->Render(viewProjection);
        }

        // Display comprehensive performance info
        static float performanceTimer = 0.0f;
        performanceTimer += deltaTime;
        if (performanceTimer >= 1.0f) {
            float totalPower = 0.0f;
            float avgTemp = 0.0f;
            float avgEfficiency = 0.0f;
            float totalDailyEnergy = 0.0f;
            float totalMonthlyEnergy = 0.0f;
            float totalYearlyEnergy = 0.0f;
            float avgDirtLevel = 0.0f;
            float avgAge = 0.0f;
            float avgHealth = 0.0f;
            float avgMaintenance = 0.0f;
            
            for (auto& panel : solarPanels) {
                totalPower += panel->GetCurrentPower();
                avgTemp += panel->GetTemperature();
                avgEfficiency += panel->GetEfficiency();
                totalDailyEnergy += panel->GetEnergyGenerated();
                totalMonthlyEnergy += panel->GetMonthlyEnergy();
                totalYearlyEnergy += panel->GetYearlyEnergy();
                avgDirtLevel += panel->GetDirtLevel();
                avgAge += panel->GetAge();
                avgHealth += panel->GetHealth();
                avgMaintenance += panel->GetMaintenanceLevel();
            }
            
            avgTemp /= solarPanels.size();
            avgEfficiency /= solarPanels.size();
            avgDirtLevel /= solarPanels.size();
            avgAge /= solarPanels.size();
            avgHealth /= solarPanels.size();
            avgMaintenance /= solarPanels.size();
            
            int hours = static_cast<int>(timeOfDay * 24);
            int minutes = static_cast<int>((timeOfDay * 24 - hours) * 60);
            
            std::cout << "\rTime: " << hours << ":" << (minutes < 10 ? "0" : "") << minutes
                      << " | Power: " << totalPower << "W | Temp: " << avgTemp << "°C"
                      << " | Efficiency: " << (avgEfficiency * 100) << "%"
                      << " | Daily: " << totalDailyEnergy << "kWh"
                      << " | Monthly: " << totalMonthlyEnergy << "kWh"
                      << " | Yearly: " << totalYearlyEnergy << "kWh"
                      << " | Dirt: " << (avgDirtLevel * 100) << "%"
                      << " | Health: " << (avgHealth * 100) << "%"
                      << " | Maintenance: " << (avgMaintenance * 100) << "%"
                      << " | Wind: " << windSpeed << "m/s"
                      << " | FPS: " << 1.0f/deltaTime << "    " << std::flush;
            performanceTimer = 0.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << std::endl << "Extended simulation ended successfully!" << std::endl;
    return 0;
}
