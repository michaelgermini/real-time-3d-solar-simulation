#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>
#include <string>

#include "Mesh.h"
#include "Texture.h"

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float metallic;
    float roughness;
    float opacity;
    
    std::shared_ptr<Texture> diffuseMap;
    std::shared_ptr<Texture> normalMap;
    std::shared_ptr<Texture> specularMap;
    std::shared_ptr<Texture> roughnessMap;
    std::shared_ptr<Texture> metallicMap;
    std::shared_ptr<Texture> aoMap;
    
    Material() : ambient(0.1f), diffuse(0.7f), specular(0.5f), 
                 shininess(32.0f), metallic(0.0f), roughness(0.5f), opacity(1.0f) {}
};

class Model {
public:
    Model();
    Model(const std::string& filePath);
    ~Model();

    // Model loading
    bool LoadFromFile(const std::string& filePath);
    void AddMesh(std::shared_ptr<Mesh> mesh);
    void SetMaterial(const Material& material);
    void SetMaterial(int meshIndex, const Material& material);

    // Transformations
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& rotation);
    void SetScale(const glm::vec3& scale);
    void SetTransform(const glm::mat4& transform);
    
    // Animation
    void Update(float deltaTime);
    void SetAnimationSpeed(float speed);
    void PlayAnimation(bool play);
    void SetAnimationFrame(int frame);

    // Rendering
    void Render(const glm::mat4& viewProjection);
    void RenderInstanced(const glm::mat4& viewProjection, const std::vector<glm::mat4>& transforms);
    
    // Getters
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetRotation() const { return rotation; }
    glm::vec3 GetScale() const { return scale; }
    glm::mat4 GetTransform() const;
    glm::mat4 GetModelMatrix() const;
    
    // Bounding box
    glm::vec3 GetBoundingBoxMin() const { return boundingBoxMin; }
    glm::vec3 GetBoundingBoxMax() const { return boundingBoxMax; }
    float GetBoundingRadius() const { return boundingRadius; }
    
    // Model properties
    const std::vector<std::shared_ptr<Mesh>>& GetMeshes() const { return meshes; }
    const std::vector<Material>& GetMaterials() const { return materials; }
    bool IsVisible() const { return visible; }
    void SetVisible(bool visible) { this->visible = visible; }

    // LOD support
    void SetLODLevel(int level);
    int GetLODLevel() const { return currentLOD; }
    void AddLODModel(std::shared_ptr<Model> lodModel, float distance);

private:
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<Material> materials;
    
    // Transform
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 transform;
    bool transformDirty;
    
    // Bounding box
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
    float boundingRadius;
    
    // Animation
    float animationSpeed;
    bool animationPlaying;
    int currentFrame;
    int totalFrames;
    
    // LOD
    int currentLOD;
    struct LODLevel {
        std::shared_ptr<Model> model;
        float distance;
    };
    std::vector<LODLevel> lodLevels;
    
    // Visibility
    bool visible;
    
    void CalculateBoundingBox();
    void UpdateTransform();
    void LoadMaterials(const std::string& filePath);
};
