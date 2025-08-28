#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>

// Enhanced Solar Panel class
class EnhancedSolarPanel {
public:
    enum class PanelType {
        MONOCRYSTALLINE,
        POLYCRYSTALLINE,
        THIN_FILM,
        BIFACIAL
    };

    EnhancedSolarPanel(PanelType type, const glm::vec3& pos, const glm::vec2& sz) 
        : type(type), position(pos), size(sz), efficiency(0.22f), powerOutput(400.0f), 
          temperature(25.0f), dirtLevel(0.0f), tilt(30.0f), azimuth(180.0f),
          dailyEnergyOutput(0.0f), monthlyEnergyOutput(0.0f), yearlyEnergyOutput(0.0f), age(0.0f) {
        GenerateGeometry();
    }

    void Update(float deltaTime, float timeOfDay, float solarIntensity, float ambientTemp) {
        // Advanced temperature simulation
        float solarHeating = solarIntensity * 0.15f;
        float cooling = (temperature - ambientTemp) * 0.02f;
        temperature = ambientTemp + solarHeating - cooling;
        
        // Dirt accumulation (realistic)
        dirtLevel += deltaTime * 0.0005f;
        if (dirtLevel > 1.0f) dirtLevel = 1.0f;
        
        // Advanced efficiency calculation
        float tempEfficiency = 0.22f - (temperature - 25.0f) * 0.004f;
        float dirtEfficiency = 1.0f - dirtLevel * 0.25f;
        float angleEfficiency = CalculateAngleEfficiency(timeOfDay);
        float degradationEfficiency = 1.0f - (age / 365.0f) * 0.005f; // 0.5% per year
        
        efficiency = tempEfficiency * dirtEfficiency * angleEfficiency * degradationEfficiency;
        if (efficiency < 0.05f) efficiency = 0.05f;
        
        // Power output calculation
        currentPowerOutput = powerOutput * efficiency * solarIntensity;
        
        // Energy tracking
        float energyThisFrame = currentPowerOutput * deltaTime / 3600.0f; // kWh
        dailyEnergyOutput += energyThisFrame;
        monthlyEnergyOutput += energyThisFrame;
        yearlyEnergyOutput += energyThisFrame;
        
        // Age tracking
        age += deltaTime / 86400.0f; // Days
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
        
        // Advanced color coding
        glm::vec3 color = glm::vec3(1.0f, 0.8f, 0.0f);
        if (efficiency < 0.15f) {
            color = glm::vec3(1.0f, 0.3f, 0.0f); // Red when inefficient
        } else if (efficiency > 0.18f) {
            color = glm::vec3(0.3f, 1.0f, 0.3f); // Green when efficient
        } else if (efficiency > 0.16f) {
            color = glm::vec3(1.0f, 1.0f, 0.3f); // Yellow when good
        }
        
        // Add dirt effect
        color = glm::mix(color, glm::vec3(0.3f, 0.3f, 0.3f), dirtLevel * 0.6f);
        
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    // Getters
    float GetCurrentPower() const { return currentPowerOutput; }
    float GetTemperature() const { return temperature; }
    float GetEfficiency() const { return efficiency; }
    float GetEnergyGenerated() const { return dailyEnergyOutput; }
    float GetMonthlyEnergy() const { return monthlyEnergyOutput; }
    float GetYearlyEnergy() const { return yearlyEnergyOutput; }
    float GetDirtLevel() const { return dirtLevel; }
    float GetAge() const { return age; }

    // Actions
    void Clean() { dirtLevel = 0.0f; }
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
};

// Enhanced Camera class
class EnhancedCamera {
public:
    EnhancedCamera() : position(10.0f, 8.0f, 10.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
                       yaw(-45.0f), pitch(-20.0f), speed(6.0f), sensitivity(0.1f), fov(45.0f) {}

    void ProcessKeyboard(int direction, float deltaTime) {
        float velocity = speed * deltaTime;
        glm::vec3 front = glm::normalize(target - position);
        glm::vec3 right = glm::normalize(glm::cross(front, up));
        
        if (direction == GLFW_KEY_W) position += front * velocity;
        if (direction == GLFW_KEY_S) position -= front * velocity;
        if (direction == GLFW_KEY_A) position -= right * velocity;
        if (direction == GLFW_KEY_D) position += right * velocity;
        if (direction == GLFW_KEY_SPACE) position += up * velocity;
        if (direction == GLFW_KEY_LEFT_SHIFT) position -= up * velocity;
        
        target = position + front;
    }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }

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
std::vector<std::unique_ptr<EnhancedSolarPanel>> solarPanels;
std::unique_ptr<EnhancedCamera> camera;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float simulationTime = 0.0f;

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

int main() {
    std::cout << "Enhanced 3D Solar Panel Simulation with OpenGL" << std::endl;
    std::cout << "==============================================" << std::endl;

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
    window = glfwCreateWindow(1200, 800, "Enhanced Solar Panel 3D Simulation", nullptr, nullptr);
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
    camera = std::make_unique<EnhancedCamera>();

    // Create enhanced solar panel array (8x8 grid)
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            glm::vec3 position(i * 2.0f - 7.0f, 0.0f, j * 2.0f - 7.0f);
            solarPanels.push_back(std::make_unique<EnhancedSolarPanel>(
                EnhancedSolarPanel::PanelType::MONOCRYSTALLINE, 
                position, 
                glm::vec2(1.5f, 1.0f)
            ));
        }
    }

    std::cout << "Enhanced Solar Panel Simulation Running!" << std::endl;
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

        // Update solar panels
        for (auto& panel : solarPanels) {
            panel->Update(deltaTime, timeOfDay, solarIntensity, ambientTemp);
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
            
            for (auto& panel : solarPanels) {
                totalPower += panel->GetCurrentPower();
                avgTemp += panel->GetTemperature();
                avgEfficiency += panel->GetEfficiency();
                totalDailyEnergy += panel->GetEnergyGenerated();
                totalMonthlyEnergy += panel->GetMonthlyEnergy();
                totalYearlyEnergy += panel->GetYearlyEnergy();
                avgDirtLevel += panel->GetDirtLevel();
                avgAge += panel->GetAge();
            }
            
            avgTemp /= solarPanels.size();
            avgEfficiency /= solarPanels.size();
            avgDirtLevel /= solarPanels.size();
            avgAge /= solarPanels.size();
            
            int hours = static_cast<int>(timeOfDay * 24);
            int minutes = static_cast<int>((timeOfDay * 24 - hours) * 60);
            
            std::cout << "\rTime: " << hours << ":" << (minutes < 10 ? "0" : "") << minutes
                      << " | Power: " << totalPower << "W | Temp: " << avgTemp << "°C"
                      << " | Efficiency: " << (avgEfficiency * 100) << "%"
                      << " | Daily: " << totalDailyEnergy << "kWh"
                      << " | Monthly: " << totalMonthlyEnergy << "kWh"
                      << " | Yearly: " << totalYearlyEnergy << "kWh"
                      << " | Dirt: " << (avgDirtLevel * 100) << "%"
                      << " | Age: " << avgAge << " days"
                      << " | FPS: " << 1.0f/deltaTime << "    " << std::flush;
            performanceTimer = 0.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << std::endl << "Enhanced simulation ended successfully!" << std::endl;
    return 0;
}
