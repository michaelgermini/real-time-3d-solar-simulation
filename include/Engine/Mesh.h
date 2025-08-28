#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    
    Vertex() : position(0.0f), normal(0.0f, 1.0f, 0.0f), 
               texCoords(0.0f), tangent(0.0f), bitangent(0.0f) {}
    
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex) 
        : position(pos), normal(norm), texCoords(tex), tangent(0.0f), bitangent(0.0f) {}
};

class Mesh {
public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Mesh creation
    void SetVertices(const std::vector<Vertex>& vertices);
    void SetIndices(const std::vector<unsigned int>& indices);
    void CalculateNormals();
    void CalculateTangents();
    void SetupMesh();

    // Rendering
    void Render();
    void RenderInstanced(int instanceCount);
    void RenderWireframe();
    
    // Buffer management
    void UpdateVertexBuffer();
    void UpdateIndexBuffer();
    void Bind();
    void Unbind();

    // Getters
    const std::vector<Vertex>& GetVertices() const { return vertices; }
    const std::vector<unsigned int>& GetIndices() const { return indices; }
    unsigned int GetVertexCount() const { return vertices.size(); }
    unsigned int GetIndexCount() const { return indices.size(); }
    GLuint GetVAO() const { return VAO; }
    GLuint GetVBO() const { return VBO; }
    GLuint GetEBO() const { return EBO; }

    // Bounding box
    glm::vec3 GetBoundingBoxMin() const { return boundingBoxMin; }
    glm::vec3 GetBoundingBoxMax() const { return boundingBoxMax; }
    float GetBoundingRadius() const { return boundingRadius; }

    // Utility
    void CalculateBoundingBox();
    void Optimize();
    void Subdivide(int levels);
    void Simplify(float targetRatio);

private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    // OpenGL buffers
    GLuint VAO, VBO, EBO;
    bool buffersInitialized;
    
    // Bounding box
    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;
    float boundingRadius;
    
    void InitializeBuffers();
    void DeleteBuffers();
    void CalculateVertexNormals();
    void CalculateVertexTangents();
};
