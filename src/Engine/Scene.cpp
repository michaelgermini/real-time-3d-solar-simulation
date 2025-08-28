#include "Engine/Scene.h"
#include "Engine/Model.h"
#include "Engine/Light.h"
#include "Components/Skybox.h"
#include <algorithm>

Scene::Scene() : ambientLight(0.1f, 0.1f, 0.1f) {
    octree = std::make_unique<OctreeNode>();
    octree->center = glm::vec3(0.0f, 0.0f, 0.0f);
    octree->size = 1000.0f;
    octree->isLeaf = true;
}

Scene::~Scene() {
    Clear();
}

void Scene::AddModel(std::shared_ptr<Model> model) {
    models.push_back(model);
    InsertModelInOctree(model, octree.get());
}

void Scene::RemoveModel(std::shared_ptr<Model> model) {
    auto it = std::find(models.begin(), models.end(), model);
    if (it != models.end()) {
        models.erase(it);
        // Rebuild octree
        BuildOctree();
    }
}

void Scene::AddLight(std::shared_ptr<Light> light) {
    lights.push_back(light);
}

void Scene::RemoveLight(std::shared_ptr<Light> light) {
    auto it = std::find(lights.begin(), lights.end(), light);
    if (it != lights.end()) {
        lights.erase(it);
    }
}

void Scene::SetSkybox(std::shared_ptr<Skybox> sky) {
    skybox = sky;
}

void Scene::SetAmbientLight(const glm::vec3& ambient) {
    ambientLight = ambient;
}

std::vector<std::shared_ptr<Model>> Scene::GetModelsInFrustum(const glm::vec4 frustumPlanes[6]) const {
    std::vector<std::shared_ptr<Model>> visibleModels;
    
    for (const auto& model : models) {
        // Simple AABB frustum culling
        glm::vec3 min = model->GetBoundingBoxMin();
        glm::vec3 max = model->GetBoundingBoxMax();
        
        bool visible = true;
        for (int i = 0; i < 6; ++i) {
            if (glm::dot(glm::vec3(frustumPlanes[i]), min) + frustumPlanes[i].w < 0 &&
                glm::dot(glm::vec3(frustumPlanes[i]), max) + frustumPlanes[i].w < 0) {
                visible = false;
                break;
            }
        }
        
        if (visible) {
            visibleModels.push_back(model);
        }
    }
    
    return visibleModels;
}

std::vector<std::shared_ptr<Model>> Scene::GetModelsNear(const glm::vec3& position, float radius) const {
    return QueryOctree(position, radius, octree.get());
}

void Scene::Update(float deltaTime) {
    // Update all models
    for (auto& model : models) {
        model->Update(deltaTime);
    }
    
    // Update lights
    for (auto& light : lights) {
        light->Update(deltaTime);
    }
    
    // Update skybox
    if (skybox) {
        skybox->Update(deltaTime);
    }
}

void Scene::Clear() {
    models.clear();
    lights.clear();
    skybox.reset();
    octree.reset();
}

void Scene::BuildOctree() {
    // Clear existing octree
    octree = std::make_unique<OctreeNode>();
    octree->center = glm::vec3(0.0f, 0.0f, 0.0f);
    octree->size = 1000.0f;
    octree->isLeaf = true;
    
    // Insert all models
    for (const auto& model : models) {
        InsertModelInOctree(model, octree.get());
    }
}

void Scene::InsertModelInOctree(std::shared_ptr<Model> model, OctreeNode* node) {
    if (!node) return;
    
    // Check if model fits in this node
    glm::vec3 modelCenter = model->GetPosition();
    float modelRadius = model->GetBoundingSphereRadius();
    
    if (glm::distance(modelCenter, node->center) + modelRadius > node->size * 0.5f) {
        // Model doesn't fit, skip
        return;
    }
    
    if (node->isLeaf) {
        // Add to leaf node
        node->models.push_back(model);
        
        // Split if too many models
        if (node->models.size() > 8 && node->size > 10.0f) {
            SplitOctreeNode(node);
        }
    } else {
        // Find appropriate child
        glm::vec3 direction = modelCenter - node->center;
        int childIndex = 0;
        if (direction.x > 0) childIndex |= 1;
        if (direction.y > 0) childIndex |= 2;
        if (direction.z > 0) childIndex |= 4;
        
        if (node->children[childIndex]) {
            InsertModelInOctree(model, node->children[childIndex].get());
        }
    }
}

void Scene::SplitOctreeNode(OctreeNode* node) {
    if (!node || !node->isLeaf) return;
    
    node->isLeaf = false;
    float halfSize = node->size * 0.5f;
    
    // Create 8 children
    for (int i = 0; i < 8; ++i) {
        node->children[i] = std::make_unique<OctreeNode>();
        auto& child = node->children[i];
        
        child->size = halfSize;
        child->isLeaf = true;
        
        // Calculate child center
        glm::vec3 offset = glm::vec3(
            (i & 1) ? halfSize * 0.5f : -halfSize * 0.5f,
            (i & 2) ? halfSize * 0.5f : -halfSize * 0.5f,
            (i & 4) ? halfSize * 0.5f : -halfSize * 0.5f
        );
        child->center = node->center + offset;
    }
    
    // Redistribute models to children
    for (const auto& model : node->models) {
        glm::vec3 modelCenter = model->GetPosition();
        glm::vec3 direction = modelCenter - node->center;
        
        int childIndex = 0;
        if (direction.x > 0) childIndex |= 1;
        if (direction.y > 0) childIndex |= 2;
        if (direction.z > 0) childIndex |= 4;
        
        node->children[childIndex]->models.push_back(model);
    }
    
    // Clear parent node models
    node->models.clear();
}

std::vector<std::shared_ptr<Model>> Scene::QueryOctree(const glm::vec3& position, float radius, OctreeNode* node) const {
    std::vector<std::shared_ptr<Model>> result;
    
    if (!node) return result;
    
    // Check if query sphere intersects with this node
    float distance = glm::distance(position, node->center);
    if (distance > radius + node->size * 0.5f) {
        return result;
    }
    
    if (node->isLeaf) {
        // Check models in leaf node
        for (const auto& model : node->models) {
            glm::vec3 modelCenter = model->GetPosition();
            float modelRadius = model->GetBoundingSphereRadius();
            
            if (glm::distance(position, modelCenter) <= radius + modelRadius) {
                result.push_back(model);
            }
        }
    } else {
        // Query children
        for (const auto& child : node->children) {
            if (child) {
                auto childResult = QueryOctree(position, radius, child.get());
                result.insert(result.end(), childResult.begin(), childResult.end());
            }
        }
    }
    
    return result;
}
