#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "../Engine/Model.h"
#include "../Engine/Texture.h"

class SolarPanel {
public:
    enum class PanelType {
        MONOCRYSTALLINE,
        POLYCRYSTALLINE,
        THIN_FILM,
        BIFACIAL
    };

    SolarPanel();
    SolarPanel(PanelType type, const glm::vec3& position, const glm::vec2& size);
    ~SolarPanel();

    // Panel properties
    void SetType(PanelType type);
    void SetPosition(const glm::vec3& position);
    void SetSize(const glm::vec2& size);
    void SetRotation(float angle, const glm::vec3& axis);
    void SetTilt(float tilt);
    void SetAzimuth(float azimuth);

    // Solar properties
    void SetEfficiency(float efficiency);
    void SetPowerOutput(float watts);
    void SetTemperature(float temperature);
    void SetDirtLevel(float dirtLevel);

    // Array management
    void CreateArray(int rows, int cols, float spacing);
    void SetArraySpacing(float spacing);
    void SetArrayOrientation(const glm::vec3& direction);

    // Energy simulation
    void Update(float deltaTime);
    void UpdateEnergyOutput(float timeOfDay, float solarIntensity, float temperature);
    float GetCurrentPower() const { return currentPowerOutput; }
    float GetCurrentPowerOutput() const { return currentPowerOutput; }
    float GetEnergyGenerated() const { return dailyEnergyOutput; }
    float GetDailyEnergyOutput() const { return dailyEnergyOutput; }
    float GetEfficiency() const { return efficiency; }
    float GetTemperature() const { return temperature; }

    // Rendering
    void GenerateGeometry();
    void Render(const glm::mat4& viewProjection);
    std::shared_ptr<Model> GetModel() const { return model; }

    // Getters
    PanelType GetType() const { return type; }
    glm::vec3 GetPosition() const { return position; }
    glm::vec2 GetSize() const { return size; }
    float GetTilt() const { return tilt; }
    float GetAzimuth() const { return azimuth; }
    glm::mat4 GetTransform() const;

    // Array properties
    int GetArrayRows() const { return arrayRows; }
    int GetArrayCols() const { return arrayCols; }
    float GetArraySpacing() const { return arraySpacing; }

private:
    PanelType type;
    glm::vec3 position;
    glm::vec2 size;
    glm::vec3 rotation;
    float tilt;
    float azimuth;

    // Solar properties
    float efficiency;
    float powerOutput;
    float temperature;
    float dirtLevel;
    float currentPowerOutput;
    float dailyEnergyOutput;

    // Array properties
    int arrayRows;
    int arrayCols;
    float arraySpacing;
    glm::vec3 arrayDirection;

    std::shared_ptr<Model> model;
    std::vector<glm::mat4> panelTransforms;

    void GeneratePanelGeometry();
    void GenerateArrayGeometry();
    void CalculatePanelTransforms();
    void UpdateEfficiency();
    float CalculateSolarAngle(float timeOfDay);
};
