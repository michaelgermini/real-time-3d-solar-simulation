#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class Shader {
public:
    Shader();
    ~Shader();

    bool LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    bool LoadFromFiles(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath);
    void Use();
    void Unuse();
    
    // Uniform setters
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);
    
    // Array uniforms
    void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& values);
    void SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values);
    
    GLuint GetID() const { return programID; }
    bool IsValid() const { return programID != 0; }

private:
    GLuint programID;
    std::unordered_map<std::string, GLint> uniformCache;
    
    bool CompileShader(GLuint& shaderID, GLenum shaderType, const std::string& source);
    bool LinkProgram();
    GLint GetUniformLocation(const std::string& name);
    std::string ReadFile(const std::string& filePath);
    void CheckCompileErrors(GLuint shader, const std::string& type);
};
