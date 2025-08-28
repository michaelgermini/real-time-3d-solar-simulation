#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "../Engine/Model.h"
#include "../Engine/Texture.h"

class Building {
public:
    enum class BuildingType {
        OFFICE,
        RESIDENTIAL,
        INDUSTRIAL,
        COMMERCIAL,
        SKYSCRAPER
    };

    Building();
    Building(BuildingType type, const glm::vec3& position, const glm::vec3& size);
    ~Building();

    // Building properties
    void SetType(BuildingType type);
    void SetPosition(const glm::vec3& position);
    void SetSize(const glm::vec3& size);
    void SetRotation(float angle, const glm::vec3& axis);
    void SetMaterial(const std::string& materialName);

    // Building features
    void AddWindows(int count);
    void AddEntrance(const glm::vec3& position);
    void AddBalcony(const glm::vec3& position, const glm::vec2& size);
    void SetHeight(float height);
    void SetFloorCount(int floors);

    // Rendering
    void GenerateGeometry();
    void Render(const glm::mat4& viewProjection);
    std::shared_ptr<Model> GetModel() const { return model; }

    // Getters
    BuildingType GetType() const { return type; }
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetSize() const { return size; }
    float GetHeight() const { return height; }
    int GetFloorCount() const { return floorCount; }
    glm::mat4 GetTransform() const;

    // Collision
    bool IsPointInside(const glm::vec3& point) const;
    glm::vec3 GetBoundingBoxMin() const;
    glm::vec3 GetBoundingBoxMax() const;

private:
    BuildingType type;
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 rotation;
    float height;
    int floorCount;
    std::string materialName;

    std::shared_ptr<Model> model;
    std::vector<glm::vec3> windows;
    std::vector<glm::vec3> entrances;
    std::vector<std::pair<glm::vec3, glm::vec2>> balconies;

    void GenerateBuildingGeometry();
    void GenerateWindows();
    void GenerateEntrances();
    void GenerateBalconies();
    void ApplyMaterials();
};
