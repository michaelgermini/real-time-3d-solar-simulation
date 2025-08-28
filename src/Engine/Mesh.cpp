#include "Engine/Mesh.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : vertices(vertices), indices(indices), vao(0), vbo(0), ebo(0) {
    SetupMesh();
}

Mesh::~Mesh() {
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
    if (ebo != 0) {
        glDeleteBuffers(1, &ebo);
    }
}

void Mesh::SetupMesh() {
    // Create vertex array object
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    
    glBindVertexArray(vao);
    
    // Load vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    // Load index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Set vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    // TexCoord attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    
    glBindVertexArray(0);
}

void Mesh::Draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::DrawInstanced(unsigned int instanceCount) const {
    glBindVertexArray(vao);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0, instanceCount);
    glBindVertexArray(0);
}

const std::vector<Vertex>& Mesh::GetVertices() const {
    return vertices;
}

const std::vector<unsigned int>& Mesh::GetIndices() const {
    return indices;
}

GLuint Mesh::GetVAO() const {
    return vao;
}
