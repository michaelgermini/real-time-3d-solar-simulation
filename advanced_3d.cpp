#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// Simple Shader class
class Shader {
public:
    Shader() : program(0) {}
    
    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexCode = LoadShaderFile(vertexPath);
        std::string fragmentCode = LoadShaderFile(fragmentPath);
        
        if (vertexCode.empty() || fragmentCode.empty()) {
            return false;
        }
        
        return CreateFromSource(vertexCode, fragmentCode);
    }
    
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
    
    std::string LoadShaderFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << path << std::endl;
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
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

// Advanced Solar Panel class
class AdvancedSolarPanel {
public:
    enum class PanelType {
        MONOCRYSTALLINE,
        POLYCRYSTALLINE,
        THIN_FILM,
        BIFACIAL
    };

    AdvancedSolarPanel(PanelType type, const glm::vec3& pos, const glm::vec2& sz) 
        : type(type), position(pos), size(sz), efficiency(0.15f), powerOutput(300.0f), 
          temperature(25.0f), dirtLevel(0.0f), tilt(30.0f), azimuth(180.0f) {
        GenerateGeometry();
    }

    void Update(float deltaTime, float timeOfDay, float solarIntensity) {
        // Temperature simulation
        float ambientTemp = 20.0f + 15.0f * sin(timeOfDay * 2.0f * glm::pi<float>());
        float solarHeating = solarIntensity * 0.1f;
        temperature = ambientTemp + solarHeating;
        
        // Dirt accumulation
        dirtLevel += deltaTime * 0.001f;
        if (dirtLevel > 1.0f) dirtLevel = 1.0f;
        
        // Efficiency calculation
        float tempEfficiency = 0.15f - (temperature - 25.0f) * 0.004f;
        float dirtEfficiency = 1.0f - dirtLevel * 0.3f;
        float angleEfficiency = CalculateAngleEfficiency(timeOfDay);
        
        efficiency = tempEfficiency * dirtEfficiency * angleEfficiency;
        if (efficiency < 0.05f) efficiency = 0.05f;
        
        // Power output
        currentPowerOutput = powerOutput * efficiency * solarIntensity;
        dailyEnergyOutput += currentPowerOutput * deltaTime / 3600.0f; // Convert to kWh
    }

    void Render(const glm::mat4& viewProjection, const Shader& shader) {
        shader.Use();
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(size.x, 0.1f, size.y));
        
        shader.SetMat4("model", model);
        shader.SetMat4("viewProjection", viewProjection);
        
        // Set color based on efficiency
        glm::vec3 color = glm::vec3(1.0f, 0.8f, 0.0f);
        if (efficiency < 0.1f) {
            color = glm::vec3(1.0f, 0.3f, 0.0f); // Red when inefficient
        } else if (efficiency > 0.12f) {
            color = glm::vec3(0.3f, 1.0f, 0.3f); // Green when efficient
        }
        
        // Add dirt effect
        color = glm::mix(color, glm::vec3(0.3f, 0.3f, 0.3f), dirtLevel * 0.5f);
        
        shader.SetVec3("color", color);
        
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    float GetCurrentPower() const { return currentPowerOutput; }
    float GetTemperature() const { return temperature; }
    float GetEfficiency() const { return efficiency; }
    float GetEnergyGenerated() const { return dailyEnergyOutput; }
    float GetDirtLevel() const { return dirtLevel; }

    void Clean() { dirtLevel = 0.0f; }
    void SetTilt(float newTilt) { tilt = newTilt; }
    void SetAzimuth(float newAzimuth) { azimuth = newAzimuth; }

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

    GLuint VAO, VBO, EBO;

    void GenerateGeometry() {
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
        // Calculate sun position
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

// Advanced Camera class
class AdvancedCamera {
public:
    AdvancedCamera() : position(10.0f, 8.0f, 10.0f), target(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f),
                       yaw(-45.0f), pitch(-20.0f), speed(5.0f), sensitivity(0.1f) {}

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
        speed += yoffset * 0.5f;
        if (speed < 0.1f) speed = 0.1f;
        if (speed > 20.0f) speed = 20.0f;
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
    }

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float yaw;
    float pitch;
    float speed;
    float sensitivity;

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
std::vector<std::unique_ptr<AdvancedSolarPanel>> solarPanels;
std::unique_ptr<AdvancedCamera> camera;
std::unique_ptr<Shader> mainShader;
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
    std::cout << "Advanced 3D Solar Panel Simulation" << std::endl;
    std::cout << "===================================" << std::endl;

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
    window = glfwCreateWindow(1200, 800, "Advanced Solar Panel 3D Simulation", nullptr, nullptr);
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

    // Create shader
    mainShader = std::make_unique<Shader>();
    
    // Create shader source
    std::string vertexShaderSource = R"(
        #version 430 core
        layout (location = 0) in vec3 aPos;
        
        uniform mat4 model;
        uniform mat4 viewProjection;
        
        void main() {
            gl_Position = viewProjection * model * vec4(aPos, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 430 core
        out vec4 FragColor;
        
        uniform vec3 color;
        
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";

    if (!mainShader->CreateFromSource(vertexShaderSource, fragmentShaderSource)) {
        std::cerr << "Failed to create shader" << std::endl;
        return -1;
    }

    // Create camera
    camera = std::make_unique<AdvancedCamera>();

    // Create solar panel array (7x7 grid)
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            glm::vec3 position(i * 2.0f - 6.0f, 0.0f, j * 2.0f - 6.0f);
            solarPanels.push_back(std::make_unique<AdvancedSolarPanel>(
                AdvancedSolarPanel::PanelType::MONOCRYSTALLINE, 
                position, 
                glm::vec2(1.5f, 1.0f)
            ));
        }
    }

    std::cout << "Advanced Solar Panel Simulation Running!" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  WASD - Move camera" << std::endl;
    std::cout << "  Space/Shift - Move up/down" << std::endl;
    std::cout << "  Mouse - Look around" << std::endl;
    std::cout << "  Scroll - Adjust camera speed" << std::endl;
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
        
        if (timeOfDay > 0.2f && timeOfDay < 0.8f) {
            // Day time
            float noonTime = 0.5f;
            float timeFromNoon = abs(timeOfDay - noonTime);
            solarIntensity = 1.0f - (timeFromNoon / 0.3f) * (timeFromNoon / 0.3f);
            if (solarIntensity < 0.0f) solarIntensity = 0.0f;
        }

        // Update solar panels
        for (auto& panel : solarPanels) {
            panel->Update(deltaTime, timeOfDay, solarIntensity);
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
            panel->Render(viewProjection, *mainShader);
        }

        // Display performance info
        static float performanceTimer = 0.0f;
        performanceTimer += deltaTime;
        if (performanceTimer >= 1.0f) {
            float totalPower = 0.0f;
            float avgTemp = 0.0f;
            float avgEfficiency = 0.0f;
            float totalEnergy = 0.0f;
            
            for (auto& panel : solarPanels) {
                totalPower += panel->GetCurrentPower();
                avgTemp += panel->GetTemperature();
                avgEfficiency += panel->GetEfficiency();
                totalEnergy += panel->GetEnergyGenerated();
            }
            
            avgTemp /= solarPanels.size();
            avgEfficiency /= solarPanels.size();
            
            int hours = static_cast<int>(timeOfDay * 24);
            int minutes = static_cast<int>((timeOfDay * 24 - hours) * 60);
            
            std::cout << "\rTime: " << hours << ":" << (minutes < 10 ? "0" : "") << minutes
                      << " | Power: " << totalPower << "W | Temp: " << avgTemp << "°C"
                      << " | Efficiency: " << (avgEfficiency * 100) << "%"
                      << " | Energy: " << totalEnergy << "kWh | FPS: " << 1.0f/deltaTime << "    " << std::flush;
            performanceTimer = 0.0f;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    std::cout << std::endl << "Advanced simulation ended successfully!" << std::endl;
    return 0;
}
