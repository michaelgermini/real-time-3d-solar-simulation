#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "../Engine/Model.h"
#include "../Engine/Texture.h"

class Landscape {
public:
    enum class TerrainType {
        FLAT,
        HILLY,
        MOUNTAINOUS,
        COASTAL,
        URBAN
    };

    Landscape();
    Landscape(TerrainType type, const glm::vec2& size, int resolution);
    ~Landscape();

    // Terrain properties
    void SetType(TerrainType type);
    void SetSize(const glm::vec2& size);
    void SetResolution(int resolution);
    void SetHeightScale(float scale);
    void SetHeightOffset(float offset);

    // Height map
    void LoadHeightMap(const std::string& filePath);
    void GenerateHeightMap();
    void SetHeightAt(int x, int z, float height);
    float GetHeightAt(int x, int z) const;
    float GetHeightAt(const glm::vec3& position) const;

    // Texturing
    void SetBaseTexture(const std::string& texturePath);
    void SetDetailTexture(const std::string& texturePath);
    void SetNormalMap(const std::string& normalMapPath);
    void SetBlendMap(const std::string& blendMapPath);

    // Vegetation
    void AddVegetation(const std::string& modelPath, float density);
    void SetVegetationDensity(float density);
    void SetVegetationHeight(float minHeight, float maxHeight);

    // Water
    void AddWater(const glm::vec3& position, const glm::vec2& size, float depth);
    void SetWaterLevel(float level);
    void SetWaterColor(const glm::vec3& color);

    // Rendering
    void GenerateGeometry();
    void Render(const glm::mat4& viewProjection);
    std::shared_ptr<Model> GetModel() const { return model; }

    // Getters
    TerrainType GetType() const { return type; }
    glm::vec2 GetSize() const { return size; }
    int GetResolution() const { return resolution; }
    float GetHeightScale() const { return heightScale; }
    glm::mat4 GetTransform() const;

    // Collision
    bool IsPointOnTerrain(const glm::vec3& point) const;
    glm::vec3 GetTerrainNormal(const glm::vec3& position) const;

private:
    TerrainType type;
    glm::vec2 size;
    int resolution;
    float heightScale;
    float heightOffset;

    // Height data
    std::vector<float> heightMap;
    std::vector<glm::vec3> normals;

    // Textures
    std::shared_ptr<Texture> baseTexture;
    std::shared_ptr<Texture> detailTexture;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> blendMap;

    // Vegetation
    struct Vegetation {
        std::shared_ptr<Model> model;
        float density;
        float minHeight;
        float maxHeight;
        std::vector<glm::mat4> instances;
    };
    std::vector<Vegetation> vegetation;

    // Water
    struct Water {
        glm::vec3 position;
        glm::vec2 size;
        float depth;
        float level;
        glm::vec3 color;
        std::shared_ptr<Model> model;
    };
    std::vector<Water> waterBodies;

    std::shared_ptr<Model> model;

    void GenerateTerrainGeometry();
    void GenerateVegetationGeometry();
    void GenerateWaterGeometry();
    void CalculateNormals();
    void ApplyTextures();
    glm::vec3 InterpolateHeight(const glm::vec3& position) const;
};
