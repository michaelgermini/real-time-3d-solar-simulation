#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Engine/Model.h"
#include "Engine/Shader.h"
#include "Components/SolarPanel.h"

int main() {
    std::cout << "Testing compilation..." << std::endl;
    
    // Test GLM
    glm::vec3 testVec(1.0f, 2.0f, 3.0f);
    glm::mat4 testMat = glm::mat4(1.0f);
    
    std::cout << "GLM test: " << testVec.x << ", " << testVec.y << ", " << testVec.z << std::endl;
    
    // Test SolarPanel
    SolarPanel panel(SolarPanel::PanelType::MONOCRYSTALLINE, glm::vec3(0, 0, 0), glm::vec2(1, 1));
    
    std::cout << "SolarPanel created successfully" << std::endl;
    std::cout << "Panel type: " << static_cast<int>(panel.GetType()) << std::endl;
    
    return 0;
}
