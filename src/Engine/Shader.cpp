#include "Engine/Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : programID(0) {
}

Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = ReadFile(vertexPath);
    std::string fragmentCode = ReadFile(fragmentPath);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        return false;
    }
    
    GLuint vertexShader, fragmentShader;
    
    // Compile vertex shader
    if (!CompileShader(vertexShader, GL_VERTEX_SHADER, vertexCode)) {
        return false;
    }
    
    // Compile fragment shader
    if (!CompileShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentCode)) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // Create program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    
    if (!LinkProgram()) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

bool Shader::LoadFromFiles(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath) {
    std::string vertexCode = ReadFile(vertexPath);
    std::string geometryCode = ReadFile(geometryPath);
    std::string fragmentCode = ReadFile(fragmentPath);
    
    if (vertexCode.empty() || geometryCode.empty() || fragmentCode.empty()) {
        return false;
    }
    
    GLuint vertexShader, geometryShader, fragmentShader;
    
    // Compile vertex shader
    if (!CompileShader(vertexShader, GL_VERTEX_SHADER, vertexCode)) {
        return false;
    }
    
    // Compile geometry shader
    if (!CompileShader(geometryShader, GL_GEOMETRY_SHADER, geometryCode)) {
        glDeleteShader(vertexShader);
        return false;
    }
    
    // Compile fragment shader
    if (!CompileShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentCode)) {
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        return false;
    }
    
    // Create program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, geometryShader);
    glAttachShader(programID, fragmentShader);
    
    if (!LinkProgram()) {
        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    
    // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
    
    return true;
}

void Shader::Use() {
    glUseProgram(programID);
}

void Shader::Unuse() {
    glUseProgram(0);
}

void Shader::SetBool(const std::string& name, bool value) {
    glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4Array(const std::string& name, const std::vector<glm::mat4>& values) {
    glUniformMatrix4fv(GetUniformLocation(name), values.size(), GL_FALSE, glm::value_ptr(values[0]));
}

void Shader::SetVec3Array(const std::string& name, const std::vector<glm::vec3>& values) {
    glUniform3fv(GetUniformLocation(name), values.size(), glm::value_ptr(values[0]));
}

bool Shader::CompileShader(GLuint& shaderID, GLenum shaderType, const std::string& source) {
    shaderID = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shaderID, 1, &src, nullptr);
    glCompileShader(shaderID);
    
    CheckCompileErrors(shaderID, shaderType == GL_VERTEX_SHADER ? "VERTEX" : 
                      shaderType == GL_FRAGMENT_SHADER ? "FRAGMENT" : "GEOMETRY");
    
    return true;
}

bool Shader::LinkProgram() {
    glLinkProgram(programID);
    CheckCompileErrors(programID, "PROGRAM");
    return true;
}

GLint Shader::GetUniformLocation(const std::string& name) {
    if (uniformCache.find(name) != uniformCache.end()) {
        return uniformCache[name];
    }
    
    GLint location = glGetUniformLocation(programID, name.c_str());
    uniformCache[name] = location;
    return location;
}

std::string Shader::ReadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Shader::CheckCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];
    
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
                      << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
                      << infoLog << std::endl;
        }
    }
}
