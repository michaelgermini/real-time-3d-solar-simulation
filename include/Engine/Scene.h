#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <unordered_map>
#include <array>

#include "Model.h"
#include "Light.h"
#include "Components/Skybox.h"

class Scene {
public:
    Scene();
    ~Scene();

    // Scene management
    void AddModel(std::shared_ptr<Model> model);
    void RemoveModel(std::shared_ptr<Model> model);
    void AddLight(std::shared_ptr<Light> light);
    void RemoveLight(std::shared_ptr<Light> light);
    void SetSkybox(std::shared_ptr<Skybox> skybox);

    // Scene queries
    std::vector<std::shared_ptr<Model>> GetModels() const { return models; }
    std::vector<std::shared_ptr<Light>> GetLights() const { return lights; }
    std::shared_ptr<Skybox> GetSkybox() const { return skybox; }

    // Spatial queries
    std::vector<std::shared_ptr<Model>> GetModelsInFrustum(const glm::vec4 frustumPlanes[6]) const;
    std::vector<std::shared_ptr<Model>> GetModelsNear(const glm::vec3& position, float radius) const;

    // Scene properties
    void SetAmbientLight(const glm::vec3& ambient);
    glm::vec3 GetAmbientLight() const { return ambientLight; }

    // Update
    void Update(float deltaTime);

    // Clear
    void Clear();

private:
    std::vector<std::shared_ptr<Model>> models;
    std::vector<std::shared_ptr<Light>> lights;
    std::shared_ptr<Skybox> skybox;
    glm::vec3 ambientLight;

    // Spatial partitioning for optimization
    struct OctreeNode {
        glm::vec3 center;
        float size;
        std::vector<std::shared_ptr<Model>> models;
        std::array<std::unique_ptr<OctreeNode>, 8> children;
        bool isLeaf;
    };

    std::unique_ptr<OctreeNode> octree;
    void BuildOctree();
    void InsertModelInOctree(std::shared_ptr<Model> model, OctreeNode* node);
    std::vector<std::shared_ptr<Model>> QueryOctree(const glm::vec3& position, float radius, OctreeNode* node) const;
};
