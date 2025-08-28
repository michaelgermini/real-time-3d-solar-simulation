#include "Components/Landscape.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"
#include "Utils/MathUtils.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>

Landscape::Landscape(TerrainType type, const glm::vec2& size, int resolution)
    : terrainType(type), size(size), resolution(resolution),
      heightScale(50.0f), model(nullptr) {
    
    SetupMaterial();
    GenerateGeometry();
}

Landscape::~Landscape() {
}

void Landscape::SetHeightScale(float scale) {
    heightScale = scale;
    GenerateGeometry();
}

void Landscape::SetTerrainType(TerrainType type) {
    terrainType = type;
    GenerateGeometry();
}

void Landscape::GenerateGeometry() {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Generate height map
    std::vector<std::vector<float>> heightMap = GenerateHeightMap();
    
    // Generate vertices
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            float xPos = (float)x / (resolution - 1) * size.x - size.x / 2;
            float zPos = (float)z / (resolution - 1) * size.y - size.y / 2;
            float yPos = heightMap[z][x] * heightScale;
            
            // Calculate normal
            glm::vec3 normal = CalculateNormal(heightMap, x, z);
            
            // Texture coordinates
            float u = (float)x / (resolution - 1);
            float v = (float)z / (resolution - 1);
            
            vertices.push_back({{xPos, yPos, zPos}, normal, {u, v}});
        }
    }
    
    // Generate indices
    for (int z = 0; z < resolution - 1; ++z) {
        for (int x = 0; x < resolution - 1; ++x) {
            int topLeft = z * resolution + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * resolution + x;
            int bottomRight = bottomLeft + 1;
            
            // First triangle
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Second triangle
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    // Create mesh
    auto mesh = std::make_shared<Mesh>(vertices, indices);
    
    // Create model
    model = std::make_shared<Model>();
    model->AddMesh(mesh);
    model->SetMaterial(material);
}

std::vector<std::vector<float>> Landscape::GenerateHeightMap() {
    std::vector<std::vector<float>> heightMap(resolution, std::vector<float>(resolution));
    
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Generate base height map based on terrain type
    switch (terrainType) {
        case TerrainType::FLAT:
            GenerateFlatTerrain(heightMap);
            break;
        case TerrainType::HILLY:
            GenerateHillyTerrain(heightMap, gen, dis);
            break;
        case TerrainType::MOUNTAINOUS:
            GenerateMountainousTerrain(heightMap, gen, dis);
            break;
        case TerrainType::VALLEY:
            GenerateValleyTerrain(heightMap, gen, dis);
            break;
    }
    
    // Apply smoothing
    SmoothHeightMap(heightMap);
    
    return heightMap;
}

void Landscape::GenerateFlatTerrain(std::vector<std::vector<float>>& heightMap) {
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            heightMap[z][x] = 0.0f;
        }
    }
}

void Landscape::GenerateHillyTerrain(std::vector<std::vector<float>>& heightMap, 
                                   std::mt19937& gen, std::uniform_real_distribution<float>& dis) {
    // Generate multiple noise layers for hilly terrain
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            float height = 0.0f;
            
            // Large scale hills
            float largeScale = sin(x * 0.1f) * cos(z * 0.1f) * 0.5f;
            
            // Medium scale variation
            float mediumScale = sin(x * 0.3f) * cos(z * 0.3f) * 0.3f;
            
            // Small scale noise
            float smallScale = dis(gen) * 0.2f;
            
            height = largeScale + mediumScale + smallScale;
            heightMap[z][x] = std::max(0.0f, height);
        }
    }
}

void Landscape::GenerateMountainousTerrain(std::vector<std::vector<float>>& heightMap,
                                         std::mt19937& gen, std::uniform_real_distribution<float>& dis) {
    // Generate mountainous terrain with peaks
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            float height = 0.0f;
            
            // Create mountain peaks
            float centerX = resolution / 2.0f;
            float centerZ = resolution / 2.0f;
            float distanceFromCenter = sqrt((x - centerX) * (x - centerX) + (z - centerZ) * (z - centerZ));
            float maxDistance = resolution / 3.0f;
            
            if (distanceFromCenter < maxDistance) {
                float peakHeight = 1.0f - (distanceFromCenter / maxDistance);
                peakHeight = peakHeight * peakHeight; // Square for sharper peaks
                height = peakHeight;
            }
            
            // Add noise
            height += dis(gen) * 0.1f;
            
            heightMap[z][x] = std::max(0.0f, height);
        }
    }
}

void Landscape::GenerateValleyTerrain(std::vector<std::vector<float>>& heightMap,
                                    std::mt19937& gen, std::uniform_real_distribution<float>& dis) {
    // Generate valley terrain
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            float height = 0.0f;
            
            // Create valley
            float centerX = resolution / 2.0f;
            float centerZ = resolution / 2.0f;
            float distanceFromCenter = sqrt((x - centerX) * (x - centerX) + (z - centerZ) * (z - centerZ));
            float maxDistance = resolution / 4.0f;
            
            if (distanceFromCenter < maxDistance) {
                float valleyDepth = (distanceFromCenter / maxDistance);
                valleyDepth = valleyDepth * valleyDepth; // Square for deeper valleys
                height = 1.0f - valleyDepth;
            } else {
                height = 0.5f; // Elevated edges
            }
            
            // Add noise
            height += dis(gen) * 0.05f;
            
            heightMap[z][x] = std::max(0.0f, height);
        }
    }
}

void Landscape::SmoothHeightMap(std::vector<std::vector<float>>& heightMap) {
    std::vector<std::vector<float>> smoothed = heightMap;
    
    for (int z = 1; z < resolution - 1; ++z) {
        for (int x = 1; x < resolution - 1; ++x) {
            float sum = 0.0f;
            int count = 0;
            
            // Average with neighbors
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dx = -1; dx <= 1; ++dx) {
                    sum += heightMap[z + dz][x + dx];
                    count++;
                }
            }
            
            smoothed[z][x] = sum / count;
        }
    }
    
    heightMap = smoothed;
}

glm::vec3 Landscape::CalculateNormal(const std::vector<std::vector<float>>& heightMap, int x, int z) {
    // Calculate normal using finite differences
    float left = (x > 0) ? heightMap[z][x - 1] : heightMap[z][x];
    float right = (x < resolution - 1) ? heightMap[z][x + 1] : heightMap[z][x];
    float up = (z > 0) ? heightMap[z - 1][x] : heightMap[z][x];
    float down = (z < resolution - 1) ? heightMap[z + 1][x] : heightMap[z][x];
    
    float dx = (right - left) * heightScale;
    float dz = (down - up) * heightScale;
    
    glm::vec3 normal(-dx, 2.0f, -dz);
    return glm::normalize(normal);
}

void Landscape::SetupMaterial() {
    material = Material();
    
    switch (terrainType) {
        case TerrainType::FLAT:
            material.albedo = glm::vec3(0.4f, 0.6f, 0.3f); // Grass green
            material.metallic = 0.0f;
            material.roughness = 0.8f;
            material.ao = 1.0f;
            break;
            
        case TerrainType::HILLY:
            material.albedo = glm::vec3(0.5f, 0.7f, 0.4f); // Light green
            material.metallic = 0.0f;
            material.roughness = 0.7f;
            material.ao = 1.0f;
            break;
            
        case TerrainType::MOUNTAINOUS:
            material.albedo = glm::vec3(0.6f, 0.6f, 0.6f); // Gray
            material.metallic = 0.0f;
            material.roughness = 0.9f;
            material.ao = 1.0f;
            break;
            
        case TerrainType::VALLEY:
            material.albedo = glm::vec3(0.3f, 0.5f, 0.2f); // Dark green
            material.metallic = 0.0f;
            material.roughness = 0.8f;
            material.ao = 1.0f;
            break;
    }
}

void Landscape::Update(float deltaTime) {
    // Landscape-specific updates if needed
}

std::shared_ptr<Model> Landscape::GetModel() const {
    return model;
}

// Getters
Landscape::TerrainType Landscape::GetType() const { return terrainType; }
glm::vec2 Landscape::GetSize() const { return size; }
int Landscape::GetResolution() const { return resolution; }
float Landscape::GetHeightScale() const { return heightScale; }
