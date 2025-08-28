#include "Components/Building.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Building::Building(BuildingType type, const glm::vec3& position, const glm::vec3& size)
    : buildingType(type), position(position), size(size),
      height(20.0f), floorCount(5), model(nullptr) {
    
    SetupMaterial();
    GenerateGeometry();
}

Building::~Building() {
}

void Building::SetHeight(float h) {
    height = h;
    GenerateGeometry();
}

void Building::SetFloorCount(int floors) {
    floorCount = floors;
    GenerateGeometry();
}

void Building::SetPosition(const glm::vec3& pos) {
    position = pos;
    if (model) {
        model->SetPosition(position);
    }
}

void Building::GenerateGeometry() {
    // Create building geometry
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    float width = size.x;
    float depth = size.z;
    float buildingHeight = height;
    
    // Create building box
    // Front face
    vertices.push_back({{-width/2, 0.0f, -depth/2}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ width/2, 0.0f, -depth/2}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ width/2, buildingHeight, -depth/2}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-width/2, buildingHeight, -depth/2}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}});
    
    // Back face
    vertices.push_back({{-width/2, 0.0f, depth/2}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ width/2, 0.0f, depth/2}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ width/2, buildingHeight, depth/2}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-width/2, buildingHeight, depth/2}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}});
    
    // Left face
    vertices.push_back({{-width/2, 0.0f, -depth/2}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{-width/2, 0.0f, depth/2}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{-width/2, buildingHeight, depth/2}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-width/2, buildingHeight, -depth/2}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // Right face
    vertices.push_back({{ width/2, 0.0f, -depth/2}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ width/2, 0.0f, depth/2}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ width/2, buildingHeight, depth/2}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{ width/2, buildingHeight, -depth/2}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}});
    
    // Top face
    vertices.push_back({{-width/2, buildingHeight, -depth/2}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ width/2, buildingHeight, -depth/2}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ width/2, buildingHeight, depth/2}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-width/2, buildingHeight, depth/2}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
    
    // Generate indices for all faces
    for (int face = 0; face < 6; ++face) {
        int baseIndex = face * 4;
        indices.push_back(baseIndex); indices.push_back(baseIndex + 1); indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 2); indices.push_back(baseIndex + 3); indices.push_back(baseIndex);
    }
    
    // Add windows based on building type
    AddWindows(vertices, indices);
    
    // Create mesh
    auto mesh = std::make_shared<Mesh>(vertices, indices);
    
    // Create model
    model = std::make_shared<Model>();
    model->AddMesh(mesh);
    model->SetPosition(position);
    model->SetMaterial(material);
}

void Building::AddWindows(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    float windowWidth = 1.5f;
    float windowHeight = 2.0f;
    float windowDepth = 0.1f;
    float floorHeight = height / floorCount;
    
    for (int floor = 1; floor < floorCount; ++floor) {
        float floorY = floor * floorHeight;
        
        // Add windows on front and back faces
        for (int window = 0; window < 3; ++window) {
            float windowX = (window - 1) * (size.x / 3);
            
            // Front window
            AddWindow(vertices, indices, glm::vec3(windowX, floorY, -size.z/2 - windowDepth/2), 
                     windowWidth, windowHeight, windowDepth, glm::vec3(0.0f, 0.0f, 1.0f));
            
            // Back window
            AddWindow(vertices, indices, glm::vec3(windowX, floorY, size.z/2 + windowDepth/2), 
                     windowWidth, windowHeight, windowDepth, glm::vec3(0.0f, 0.0f, -1.0f));
        }
        
        // Add windows on left and right faces
        for (int window = 0; window < 2; ++window) {
            float windowZ = (window - 0.5f) * (size.z / 2);
            
            // Left window
            AddWindow(vertices, indices, glm::vec3(-size.x/2 - windowDepth/2, floorY, windowZ), 
                     windowWidth, windowHeight, windowDepth, glm::vec3(1.0f, 0.0f, 0.0f));
            
            // Right window
            AddWindow(vertices, indices, glm::vec3(size.x/2 + windowDepth/2, floorY, windowZ), 
                     windowWidth, windowHeight, windowDepth, glm::vec3(-1.0f, 0.0f, 0.0f));
        }
    }
}

void Building::AddWindow(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
                        const glm::vec3& position, float width, float height, float depth,
                        const glm::vec3& normal) {
    int baseIndex = vertices.size();
    
    // Window frame vertices
    vertices.push_back({{position.x - width/2, position.y - height/2, position.z - depth/2}, normal, {0.0f, 0.0f}});
    vertices.push_back({{position.x + width/2, position.y - height/2, position.z - depth/2}, normal, {1.0f, 0.0f}});
    vertices.push_back({{position.x + width/2, position.y + height/2, position.z - depth/2}, normal, {1.0f, 1.0f}});
    vertices.push_back({{position.x - width/2, position.y + height/2, position.z - depth/2}, normal, {0.0f, 1.0f}});
    
    vertices.push_back({{position.x - width/2, position.y - height/2, position.z + depth/2}, normal, {0.0f, 0.0f}});
    vertices.push_back({{position.x + width/2, position.y - height/2, position.z + depth/2}, normal, {1.0f, 0.0f}});
    vertices.push_back({{position.x + width/2, position.y + height/2, position.z + depth/2}, normal, {1.0f, 1.0f}});
    vertices.push_back({{position.x - width/2, position.y + height/2, position.z + depth/2}, normal, {0.0f, 1.0f}});
    
    // Window frame indices
    indices.push_back(baseIndex); indices.push_back(baseIndex + 1); indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 2); indices.push_back(baseIndex + 3); indices.push_back(baseIndex);
    
    indices.push_back(baseIndex + 4); indices.push_back(baseIndex + 6); indices.push_back(baseIndex + 5);
    indices.push_back(baseIndex + 6); indices.push_back(baseIndex + 4); indices.push_back(baseIndex + 7);
    
    // Window glass (transparent)
    int glassBaseIndex = vertices.size();
    vertices.push_back({{position.x - width/2 + 0.05f, position.y - height/2 + 0.05f, position.z}, normal, {0.0f, 0.0f}});
    vertices.push_back({{position.x + width/2 - 0.05f, position.y - height/2 + 0.05f, position.z}, normal, {1.0f, 0.0f}});
    vertices.push_back({{position.x + width/2 - 0.05f, position.y + height/2 - 0.05f, position.z}, normal, {1.0f, 1.0f}});
    vertices.push_back({{position.x - width/2 + 0.05f, position.y + height/2 - 0.05f, position.z}, normal, {0.0f, 1.0f}});
    
    indices.push_back(glassBaseIndex); indices.push_back(glassBaseIndex + 1); indices.push_back(glassBaseIndex + 2);
    indices.push_back(glassBaseIndex + 2); indices.push_back(glassBaseIndex + 3); indices.push_back(glassBaseIndex);
}

void Building::SetupMaterial() {
    material = Material();
    
    switch (buildingType) {
        case BuildingType::OFFICE:
            material.albedo = glm::vec3(0.8f, 0.8f, 0.8f); // Light gray
            material.metallic = 0.0f;
            material.roughness = 0.3f;
            material.ao = 1.0f;
            break;
            
        case BuildingType::RESIDENTIAL:
            material.albedo = glm::vec3(0.9f, 0.85f, 0.8f); // Beige
            material.metallic = 0.0f;
            material.roughness = 0.4f;
            material.ao = 1.0f;
            break;
            
        case BuildingType::INDUSTRIAL:
            material.albedo = glm::vec3(0.6f, 0.6f, 0.6f); // Dark gray
            material.metallic = 0.1f;
            material.roughness = 0.6f;
            material.ao = 1.0f;
            break;
    }
}

void Building::Update(float deltaTime) {
    // Building-specific updates if needed
}

std::shared_ptr<Model> Building::GetModel() const {
    return model;
}

// Getters
Building::BuildingType Building::GetType() const { return buildingType; }
glm::vec3 Building::GetPosition() const { return position; }
glm::vec3 Building::GetSize() const { return size; }
float Building::GetHeight() const { return height; }
int Building::GetFloorCount() const { return floorCount; }
