#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <random>
#include <chrono>

// Ultra Advanced Shader class with multiple shader support
class UltraAdvancedShader {
public:
    UltraAdvancedShader() : program(0) {}
    
    bool CreateFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
        const char* vShaderCode = vertexSource.c_str();
        const char* fShaderCode = fragmentSource.c_str();
        
        GLuint vertex, fragment;
        
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        CheckCompileErrors(vertex, "VERTEX");
        
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        CheckCompileErrors(fragment, "FRAGMENT");
        
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        CheckCompileErrors(program, "PROGRAM");
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        
        return true;
    }
    
    void Use() { glUseProgram(program); }
    void Unuse() { glUseProgram(0); }
    
    void SetMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    
    void SetVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, glm::value_ptr(value));
    }
    
    void SetFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(program, name.c_str()), value);
    }
    
    void SetInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(program, name.c_str()), value);
    }
    
    GLuint GetID() const { return program; }

private:
    GLuint program;
    
    void CheckCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};

// Ultra Advanced Solar Panel class with advanced physics
class UltraAdvancedSolarPanel {
public:
    enum class PanelType {
        MONOCRYSTALLINE,
        POLYCRYSTALLINE,
        THIN_FILM,
        BIFACIAL,
        PEROVSKITE
    };

    UltraAdvancedSolarPanel(PanelType type, const glm::vec3& pos, const glm::vec2& sz) 
        : type(type), position(pos), size(sz), efficiency(0.22f), powerOutput(400.0f), 
          temperature(25.0f), dirtLevel(0.0f), tilt(30.0f), azimuth(180.0f),
          dailyEnergyOutput(0.0f), monthlyEnergyOutput(0.0f), yearlyEnergyOutput(0.0f), age(0.0f),
          health(1.0f), maintenanceLevel(1.0f), weatherResistance(0.9f), degradationRate(0.005f) {
        GenerateGeometry();
        InitializePhysics();
    }

    void Update(float deltaTime, float timeOfDay, float solarIntensity, float ambientTemp, float windSpeed, float humidity) {
        // Advanced temperature simulation with wind cooling
        float solarHeating = solarIntensity * 0.15f;
        float windCooling = windSpeed * 0.02f;
        float humidityEffect = humidity * 0.01f;
        float cooling = (temperature - ambientTemp) * 0.02f + windCooling;
        temperature = ambientTemp + solarHeating - cooling + humidityEffect;
        
        // Advanced dirt accumulation with weather effects
        float dirtAccumulation = deltaTime * 0.0005f;
        float rainCleaning = (humidity > 0.8f) ? deltaTime * 0.001f : 0.0f;
        float windCleaning = windSpeed * deltaTime * 0.0001f;
        dirtLevel += dirtAccumulation - rainCleaning - windCleaning;
        if (dirtLevel > 1.0f) dirtLevel = 1.0f;
        if (dirtLevel < 0.0f) dirtLevel = 0.0f;
        
        // Advanced efficiency calculation with multiple factors
        float tempEfficiency = 0.22f - (temperature - 25.0f) * 0.004f;
        float dirtEfficiency = 1.0f - dirtLevel * 0.25f;
        float angleEfficiency = CalculateAngleEfficiency(timeOfDay);
        float degradationEfficiency = 1.0f - (age / 365.0f) * degradationRate;
        float healthEfficiency = health;
        float maintenanceEfficiency = maintenanceLevel;
        float weatherEfficiency = weatherResistance;
        
        efficiency = tempEfficiency * dirtEfficiency * angleEfficiency * degradationEfficiency * 
                    healthEfficiency * maintenanceEfficiency * weatherEfficiency;
        if (efficiency < 0.05f) efficiency = 0.05f;
        
        // Power output calculation with advanced physics
        currentPowerOutput = powerOutput * efficiency * solarIntensity;
        
        // Energy tracking with advanced metrics
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

    void Render(const glm::mat4& viewProjection, const UltraAdvancedShader& shader, const glm::vec3& lightPos, float timeOfDay) {
        shader.Use();
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(size.x, 0.1f, size.y));
        
        shader.SetMat4("model", model);
        shader.SetMat4("viewProjection", viewProjection);
        shader.SetVec3("lightPos", lightPos);
        shader.SetVec3("viewPos", glm::vec3(0.0f, 10.0f, 10.0f));
        shader.SetFloat("timeOfDay", timeOfDay);
        
        // Advanced color coding with multiple factors
        glm::vec3 color = CalculateAdvancedColor();
        
        shader.SetVec3("color", color);
        shader.SetFloat("efficiency", efficiency);
        shader.SetFloat("temperature", temperature);
        shader.SetFloat("dirtLevel", dirtLevel);
        shader.SetFloat("health", health);
        
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
    float weatherResistance;
    float degradationRate;

    GLuint VAO, VBO, EBO;

    void GenerateGeometry() {
        // Create detailed cube geometry with more vertices for better lighting
        float vertices[] = {
            // Front face
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            
            // Back face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
            
            // Left face
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            
            // Right face
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
            
            // Top face
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            
            // Bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f
        };

        unsigned int indices[] = {
            0,  1,  2,    2,  3,  0,   // Front
            4,  5,  6,    6,  7,  4,   // Back
            8,  9,  10,   10, 11, 8,   // Left
            12, 13, 14,   14, 15, 12,  // Right
            16, 17, 18,   18, 19, 16,  // Top
            20, 21, 22,   22, 23, 20   // Bottom
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // Normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    
    void InitializePhysics() {
        // Initialize random number generator for realistic variations
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.8f, 1.2f);
        
        // Add some randomness to panel characteristics
        efficiency *= dis(gen);
        powerOutput *= dis(gen);
        weatherResistance *= dis(gen);
        degradationRate *= dis(gen);
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
            case PanelType::PEROVSKITE:
                baseColor = glm::vec3(0.6f, 0.4f, 0.8f); // Purple
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

// Ultra Advanced Camera class with smooth movement
class UltraAdvancedCamera {
public:
    UltraAdvancedCamera() : position(20.0f, 15.0f, 20.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
                           yaw(-45.0f), pitch(-30.0f), speed(10.0f), sensitivity(0.05f), fov(45.0f),
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

    void SetSpeed(float newSpeed) { speed = newSpeed; }
    void SetSensitivity(float newSensitivity) { sensitivity = newSensitivity; }

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
std::vector<std::unique_ptr<UltraAdvancedSolarPanel>> solarPanels;
std::unique_ptr<UltraAdvancedCamera> camera;
std::unique_ptr<UltraAdvancedShader> mainShader;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float simulationTime = 0.0f;

// Weather simulation
float windSpeed = 0.0f;
float humidity = 0.5f;
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
    if (weatherTimer > 10.0f) { // Change weather every 10 seconds
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> windDis(0.0f, 15.0f);
        std::uniform_real_distribution<float> humidityDis(0.2f, 0.9f);
        
        windSpeed = windDis(gen);
        humidity = humidityDis(gen);
        weatherTimer = 0.0f;
    }
}

int main() {
    std::cout << "Ultra Advanced 3D Solar Panel Simulation with OpenGL" << std::endl;
    std::cout << "===================================================" << std::endl;

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
    window = glfwCreateWindow(1200, 800, "Ultra Advanced Solar Panel 3D Simulation", nullptr, nullptr);
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
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, 1200, 800);

    // Create advanced shader
    mainShader = std::make_unique<UltraAdvancedShader>();
    
    // Create advanced shader source with lighting
    std::string vertexShaderSource = R"(
        #version 430 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        
        uniform mat4 model;
        uniform mat4 viewProjection;
        
        out vec3 FragPos;
        out vec3 Normal;
        
        void main() {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            gl_Position = viewProjection * vec4(FragPos, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 430 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        
        uniform vec3 color;
        uniform vec3 lightPos;
        uniform vec3 viewPos;
        uniform float efficiency;
        uniform float temperature;
        uniform float dirtLevel;
        uniform float health;
        uniform float timeOfDay;
        
        void main() {
            // Ambient lighting
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * color;
            
            // Diffuse lighting
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * color * (0.5 + 0.5 * efficiency);
            
            // Specular lighting
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * color;
            
            // Efficiency-based glow
            vec3 efficiencyGlow = color * efficiency * 0.3;
            
            // Temperature-based color variation
            vec3 tempColor = mix(color, vec3(1.0, 0.3, 0.0), max(0.0, (temperature - 25.0) / 50.0));
            
            // Final color calculation
            vec3 result = ambient + diffuse + specular + efficiencyGlow;
            result = mix(result, tempColor, 0.3);
            
            // Dirt effect
            result = mix(result, vec3(0.3, 0.3, 0.3), dirtLevel * 0.4);
            
            // Health effect
            result = mix(result, vec3(0.5, 0.5, 0.5), (1.0 - health) * 0.2);
            
            FragColor = vec4(result, 1.0);
        }
    )";

    if (!mainShader->CreateFromSource(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to create shader" << std::endl;
        return -1;
    }

    // Create camera
    camera = std::make_unique<UltraAdvancedCamera>();

    // Create ultra advanced solar panel array (12x12 grid)
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            glm::vec3 position(i * 2.0f - 11.0f, 0.0f, j * 2.0f - 11.0f);
            
            // Random panel type for variety
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> typeDis(0, 4);
            
            UltraAdvancedSolarPanel::PanelType type = static_cast<UltraAdvancedSolarPanel::PanelType>(typeDis(gen));
            
            solarPanels.push_back(std::make_unique<UltraAdvancedSolarPanel>(
                type, 
                position, 
                glm::vec2(1.5f, 1.0f)
            ));
        }
    }

    std::cout << "Ultra Advanced Solar Panel Simulation Running!" << std::endl;
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

        // Calculate sun position for lighting
        float sunAngle = timeOfDay * 2.0f * glm::pi<float>();
        glm::vec3 sunPosition(
            50.0f * cos(sunAngle),
            50.0f * sin(sunAngle),
            0.0f
        );

        // Update solar panels with weather
        for (auto& panel : solarPanels) {
            panel->Update(deltaTime, timeOfDay, solarIntensity, ambientTemp, windSpeed, humidity);
        }

        // Clear
        glClearColor(0.05f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get matrices
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix();
        glm::mat4 viewProjection = projection * view;

        // Render solar panels
        for (auto& panel : solarPanels) {
            panel->Render(viewProjection, *mainShader, sunPosition, timeOfDay);
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
                      << " | Humidity: " << (humidity * 100) << "%"
                      << " | FPS: " << 1.0f/deltaTime << "    " << std::flush;
            performanceTimer = 0.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << std::endl << "Ultra Advanced simulation ended successfully!" << std::endl;
    return 0;
}
