#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <chrono>

// Simple vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 ourColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
}
)";

// Simple fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0);
}
)";

class Simple3DSimulation {
private:
    GLFWwindow* window;
    GLuint shaderProgram;
    GLuint VAO, VBO;
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    float rotationAngle;
    
public:
    Simple3DSimulation() : window(nullptr), shaderProgram(0), VAO(0), VBO(0), rotationAngle(0.0f) {}
    
    bool Initialize() {
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        
        // Configure GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        // Create window
        window = glfwCreateWindow(1200, 800, "Real-Time 3D Solar Panel Simulation", nullptr, nullptr);
        if (!window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        
        // Initialize GLEW
        if (glewInit() != GLEW_OK) {
            std::cerr << "Failed to initialize GLEW" << std::endl;
            return false;
        }
        
        // Configure OpenGL
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, 1200, 800);
        
        // Create shaders
        if (!CreateShaders()) {
            return false;
        }
        
        // Create geometry
        CreateGeometry();
        
        // Set up matrices
        projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);
        view = glm::lookAt(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::mat4(1.0f);
        
        return true;
    }
    
    bool CreateShaders() {
        // Vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        
        // Check for shader compile errors
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return false;
        }
        
        // Fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            return false;
        }
        
        // Link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            return false;
        }
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        return true;
    }
    
    void CreateGeometry() {
        // Solar panel vertices (complete cube with all faces)
        float vertices[] = {
            // positions          // colors
            // Front face
            -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 0
             0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 1
             0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 2
            -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 3
            
            // Back face
            -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 4
             0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 5
             0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 6
            -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 7
            
            // Left face
            -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 8
            -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 9
            -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 10
            -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 11
            
            // Right face
             0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 12
             0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 13
             0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 14
             0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 15
            
            // Top face
            -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 16
             0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 17
             0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 18
            -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 19
            
            // Bottom face
            -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 20
             0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // 21
             0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,  // 22
            -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f   // 23
        };
        
        unsigned int indices[] = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Back face
            4, 5, 6, 6, 7, 4,
            // Left face
            8, 9, 10, 10, 11, 8,
            // Right face
            12, 13, 14, 14, 15, 12,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 21, 22, 22, 23, 20
        };
        
        GLuint EBO;
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
        
        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
    }
    
    void Render() {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(shaderProgram);
        
        // Update rotation (slower)
        rotationAngle += 0.005f;
        model = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Set uniforms
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Render solar panel
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // Changed from glDrawArrays to glDrawElements
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    void Run() {
        std::cout << "Real-Time 3D Solar Panel Simulation" << std::endl;
        std::cout << "=====================================" << std::endl;
        std::cout << "Controls:" << std::endl;
        std::cout << "- ESC: Exit" << std::endl;
        std::cout << "- Watch the rotating solar panel!" << std::endl;
        std::cout << std::endl;
        
        while (!glfwWindowShouldClose(window)) {
            Render();
            
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, true);
            }
        }
    }
    
    void Cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        glfwTerminate();
    }
};

int main() {
    Simple3DSimulation simulation;
    
    if (!simulation.Initialize()) {
        std::cerr << "Failed to initialize simulation" << std::endl;
        return -1;
    }
    
    simulation.Run();
    simulation.Cleanup();
    
    return 0;
}
