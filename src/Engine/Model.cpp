#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/Material.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Model::Model() : position(0.0f), rotation(0.0f), scale(1.0f) {
    transform = glm::mat4(1.0f);
    UpdateTransform();
}

Model::~Model() {
}

void Model::AddMesh(std::shared_ptr<Mesh> mesh) {
    meshes.push_back(mesh);
}

void Model::SetPosition(const glm::vec3& pos) {
    position = pos;
    UpdateTransform();
}

void Model::SetRotation(const glm::vec3& rot) {
    rotation = rot;
    UpdateTransform();
}

void Model::SetScale(const glm::vec3& scl) {
    scale = scl;
    UpdateTransform();
}

void Model::SetTransform(const glm::mat4& trans) {
    transform = trans;
}

void Model::SetMaterial(const Material& mat) {
    material = mat;
}

void Model::Update(float deltaTime) {
    // Update model logic here if needed
}

void Model::Draw() const {
    for (const auto& mesh : meshes) {
        mesh->Draw();
    }
}

void Model::UpdateTransform() {
    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);
}

glm::mat4 Model::GetTransform() const {
    return transform;
}

glm::vec3 Model::GetPosition() const {
    return position;
}

glm::vec3 Model::GetRotation() const {
    return rotation;
}

glm::vec3 Model::GetScale() const {
    return scale;
}

const Material& Model::GetMaterial() const {
    return material;
}

const std::vector<std::shared_ptr<Mesh>>& Model::GetMeshes() const {
    return meshes;
}

glm::vec3 Model::GetBoundingBoxMin() const {
    if (meshes.empty()) return glm::vec3(0.0f);
    
    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    for (const auto& mesh : meshes) {
        for (const auto& vertex : mesh->GetVertices()) {
            glm::vec3 worldPos = glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
            min = glm::min(min, worldPos);
        }
    }
    return min;
}

glm::vec3 Model::GetBoundingBoxMax() const {
    if (meshes.empty()) return glm::vec3(0.0f);
    
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());
    for (const auto& mesh : meshes) {
        for (const auto& vertex : mesh->GetVertices()) {
            glm::vec3 worldPos = glm::vec3(transform * glm::vec4(vertex.position, 1.0f));
            max = glm::max(max, worldPos);
        }
    }
    return max;
}

float Model::GetBoundingSphereRadius() const {
    glm::vec3 min = GetBoundingBoxMin();
    glm::vec3 max = GetBoundingBoxMax();
    glm::vec3 center = (min + max) * 0.5f;
    float radius = glm::length(max - center);
    return radius;
}
