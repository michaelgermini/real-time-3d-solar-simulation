#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>

int main() {
    std::cout << "Simple 3D Solar Panel Simulation" << std::endl;
    std::cout << "=================================" << std::endl;

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
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Solar Panel 3D Simulation", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1200, 800);

    // Simple vertex shader
    const char* vertexShaderSource = R"(
        #version 430 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 Color;
        
        void main() {
            Color = aColor;
            gl_Position = projection * view * model * vec4(aPos, 1.0);
        }
    )";

    // Simple fragment shader
    const char* fragmentShaderSource = R"(
        #version 430 core
        in vec3 Color;
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(Color, 1.0);
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
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Create solar panel vertices (simple cube)
    float vertices[] = {
        // positions        // colors
        -0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,  // Blue
         0.5f, -0.5f, -0.5f,  1.0f, 0.8f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.8f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.8f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.8f, 0.0f
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,  // Front
        1, 5, 6, 6, 2, 1,  // Right
        5, 4, 7, 7, 6, 5,  // Back
        4, 0, 3, 3, 7, 4,  // Left
        3, 2, 6, 6, 7, 3,  // Top
        4, 5, 1, 1, 0, 4   // Bottom
    };

    // Create VAO and VBO
    GLuint VAO, VBO, EBO;
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

    // Camera setup
    glm::mat4 view = glm::lookAt(
        glm::vec3(3.0f, 3.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);

    std::cout << "Solar Panel Simulation Running!" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader
        glUseProgram(shaderProgram);

        // Set uniforms
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Draw solar panel
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    std::cout << "Simulation ended successfully!" << std::endl;
    return 0;
}
