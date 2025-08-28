#include "Components/SolarPanel.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"
#include "Utils/MathUtils.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

SolarPanel::SolarPanel(PanelType type, const glm::vec3& position, const glm::vec2& size)
    : panelType(type), position(position), size(size),
      tilt(30.0f), azimuth(180.0f), efficiency(0.22f),
      powerOutput(400.0f), temperature(25.0f),
      arrayRows(1), arrayCols(1), spacing(3.0f),
      energyGenerated(0.0f), currentPower(0.0f),
      shadingFactor(1.0f), soilingFactor(0.95f),
      model(nullptr) {
    
    // Set material properties based on panel type
    SetupMaterial();
    
    // Generate geometry
    GenerateGeometry();
}

SolarPanel::~SolarPanel() {
}

void SolarPanel::SetPosition(const glm::vec3& pos) {
    position = pos;
    if (model) {
        model->SetPosition(position);
    }
}

void SolarPanel::SetTilt(float t) {
    tilt = t;
    UpdateTransform();
}

void SolarPanel::SetAzimuth(float a) {
    azimuth = a;
    UpdateTransform();
}

void SolarPanel::SetEfficiency(float eff) {
    efficiency = eff;
}

void SolarPanel::SetPowerOutput(float power) {
    powerOutput = power;
}

void SolarPanel::SetTemperature(float temp) {
    temperature = temp;
}

void SolarPanel::CreateArray(int rows, int cols, float spacing) {
    arrayRows = rows;
    arrayCols = cols;
    this->spacing = spacing;
    
    // Recalculate total size
    totalSize.x = size.x * cols + spacing * (cols - 1);
    totalSize.y = size.y * rows + spacing * (rows - 1);
    
    // Regenerate geometry for array
    GenerateGeometry();
}

void SolarPanel::GenerateGeometry() {
    // Create vertices for a single panel
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // Panel dimensions
    float width = size.x;
    float height = size.y;
    float depth = 0.05f; // Panel thickness
    
    // Create panel geometry (simplified as a flat surface)
    // Front face (solar cells)
    vertices.push_back({{-width/2, -height/2, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ width/2, -height/2, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ width/2,  height/2, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-width/2,  height/2, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}});
    
    // Back face (aluminum frame)
    vertices.push_back({{-width/2, -height/2, -depth}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}});
    vertices.push_back({{ width/2, -height/2, -depth}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}});
    vertices.push_back({{ width/2,  height/2, -depth}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}});
    vertices.push_back({{-width/2,  height/2, -depth}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}});
    
    // Indices for front face
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(2); indices.push_back(3); indices.push_back(0);
    
    // Indices for back face
    indices.push_back(4); indices.push_back(6); indices.push_back(5);
    indices.push_back(6); indices.push_back(4); indices.push_back(7);
    
    // Side faces
    // Left
    indices.push_back(0); indices.push_back(4); indices.push_back(7);
    indices.push_back(7); indices.push_back(3); indices.push_back(0);
    
    // Right
    indices.push_back(1); indices.push_back(5); indices.push_back(6);
    indices.push_back(6); indices.push_back(2); indices.push_back(1);
    
    // Top
    indices.push_back(3); indices.push_back(2); indices.push_back(6);
    indices.push_back(6); indices.push_back(7); indices.push_back(3);
    
    // Bottom
    indices.push_back(0); indices.push_back(1); indices.push_back(5);
    indices.push_back(5); indices.push_back(4); indices.push_back(0);
    
    // Create mesh
    auto mesh = std::make_shared<Mesh>(vertices, indices);
    
    // Create model
    model = std::make_shared<Model>();
    model->AddMesh(mesh);
    model->SetPosition(position);
    model->SetMaterial(material);
    
    // Update transform
    UpdateTransform();
}

void SolarPanel::SetupMaterial() {
    material = Material();
    
    switch (panelType) {
        case PanelType::MONOCRYSTALLINE:
            material.albedo = glm::vec3(0.1f, 0.1f, 0.1f); // Dark blue/black
            material.metallic = 0.0f;
            material.roughness = 0.1f; // Very smooth surface
            material.ao = 1.0f;
            break;
            
        case PanelType::POLYCRYSTALLINE:
            material.albedo = glm::vec3(0.15f, 0.15f, 0.2f); // Blue tint
            material.metallic = 0.0f;
            material.roughness = 0.15f;
            material.ao = 1.0f;
            break;
            
        case PanelType::THIN_FILM:
            material.albedo = glm::vec3(0.2f, 0.2f, 0.25f); // Darker blue
            material.metallic = 0.0f;
            material.roughness = 0.2f;
            material.ao = 1.0f;
            break;
    }
}

void SolarPanel::UpdateTransform() {
    if (!model) return;
    
    // Create transformation matrix
    glm::mat4 transform = glm::mat4(1.0f);
    
    // Apply tilt and azimuth
    transform = glm::rotate(transform, glm::radians(azimuth), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(tilt), glm::vec3(1.0f, 0.0f, 0.0f));
    
    model->SetTransform(transform);
}

void SolarPanel::Update(float deltaTime) {
    // Update energy generation
    UpdateEnergyGeneration(deltaTime);
    
    // Update temperature effects
    UpdateTemperature(deltaTime);
    
    // Update shading analysis
    UpdateShadingAnalysis();
}

void SolarPanel::UpdateEnergyGeneration(float deltaTime) {
    // Calculate solar irradiance based on time of day and weather
    float solarIrradiance = CalculateSolarIrradiance();
    
    // Calculate panel efficiency at current temperature
    float temperatureEfficiency = CalculateTemperatureEfficiency();
    
    // Calculate total efficiency
    float totalEfficiency = efficiency * temperatureEfficiency * shadingFactor * soilingFactor;
    
    // Calculate current power output
    float panelArea = size.x * size.y; // m²
    currentPower = solarIrradiance * panelArea * totalEfficiency;
    
    // Accumulate energy generated
    energyGenerated += currentPower * deltaTime / 3600.0f; // Convert to kWh
    
    // Update material based on power output (visual feedback)
    UpdateMaterialBasedOnPower();
}

float SolarPanel::CalculateSolarIrradiance() {
    // Simplified solar irradiance calculation
    // In a real implementation, this would use actual solar position and weather data
    
    // Get current time (simplified)
    float timeOfDay = static_cast<float>(glfwGetTime()) / 86400.0f; // 0-1 over 24 hours
    timeOfDay = fmod(timeOfDay, 1.0f);
    
    // Solar noon is at 0.5
    float solarNoon = 0.5f;
    float timeFromNoon = abs(timeOfDay - solarNoon);
    
    // Maximum irradiance at solar noon (1000 W/m²)
    float maxIrradiance = 1000.0f;
    
    // Calculate irradiance based on time of day
    float irradiance = maxIrradiance * cos(timeFromNoon * M_PI);
    
    // Apply weather conditions (simplified)
    float weatherFactor = 0.8f; // 80% clear sky
    irradiance *= weatherFactor;
    
    return std::max(0.0f, irradiance);
}

float SolarPanel::CalculateTemperatureEfficiency() {
    // Temperature coefficient for silicon panels (typically -0.4% per °C)
    float temperatureCoefficient = -0.004f;
    float referenceTemperature = 25.0f;
    
    float temperatureEfficiency = 1.0f + temperatureCoefficient * (temperature - referenceTemperature);
    return std::max(0.5f, temperatureEfficiency); // Minimum 50% efficiency
}

void SolarPanel::UpdateTemperature(float deltaTime) {
    // Simplified temperature model
    float ambientTemperature = 20.0f;
    float solarHeat = currentPower * 0.1f; // 10% of power becomes heat
    
    // Temperature change rate
    float targetTemperature = ambientTemperature + solarHeat;
    float temperatureRate = 0.1f; // °C per second
    
    if (temperature < targetTemperature) {
        temperature += temperatureRate * deltaTime;
    } else {
        temperature -= temperatureRate * deltaTime;
    }
    
    temperature = std::clamp(temperature, ambientTemperature, 80.0f);
}

void SolarPanel::UpdateShadingAnalysis() {
    // Simplified shading analysis
    // In a real implementation, this would use ray tracing to detect shadows
    
    // For now, use a simple time-based shading factor
    float timeOfDay = static_cast<float>(glfwGetTime()) / 86400.0f;
    timeOfDay = fmod(timeOfDay, 1.0f);
    
    // Simulate shading in morning and evening
    if (timeOfDay < 0.25f || timeOfDay > 0.75f) {
        shadingFactor = 0.3f; // Heavy shading
    } else if (timeOfDay < 0.35f || timeOfDay > 0.65f) {
        shadingFactor = 0.7f; // Light shading
    } else {
        shadingFactor = 1.0f; // No shading
    }
}

void SolarPanel::UpdateMaterialBasedOnPower() {
    // Update material appearance based on power output
    float powerRatio = currentPower / powerOutput;
    
    // Adjust albedo based on power (more power = brighter appearance)
    material.albedo = glm::vec3(0.1f + powerRatio * 0.2f);
    
    // Adjust roughness based on temperature (higher temp = more rough)
    material.roughness = 0.1f + (temperature - 25.0f) / 100.0f;
    material.roughness = std::clamp(material.roughness, 0.05f, 0.3f);
}

std::shared_ptr<Model> SolarPanel::GetModel() const {
    return model;
}

// Getters
SolarPanel::PanelType SolarPanel::GetType() const { return panelType; }
glm::vec3 SolarPanel::GetPosition() const { return position; }
glm::vec2 SolarPanel::GetSize() const { return size; }
float SolarPanel::GetTilt() const { return tilt; }
float SolarPanel::GetAzimuth() const { return azimuth; }
float SolarPanel::GetEfficiency() const { return efficiency; }
float SolarPanel::GetPowerOutput() const { return powerOutput; }
float SolarPanel::GetCurrentPower() const { return currentPower; }
float SolarPanel::GetEnergyGenerated() const { return energyGenerated; }
float SolarPanel::GetTemperature() const { return temperature; }
float SolarPanel::GetShadingFactor() const { return shadingFactor; }
