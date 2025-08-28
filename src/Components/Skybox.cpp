#include "Components/Skybox.h"
#include "Engine/Texture.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Skybox::Skybox(SkyType type) : skyType(type), timeOfDay(0.5f), cubemapTexture(0), vao(0), vbo(0) {
    InitializeGeometry();
    GenerateCubemap();
}

Skybox::~Skybox() {
    if (cubemapTexture != 0) {
        glDeleteTextures(1, &cubemapTexture);
    }
    if (vao != 0) {
        glDeleteVertexArrays(1, &vao);
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
}

void Skybox::SetTimeOfDay(float time) {
    timeOfDay = std::clamp(time, 0.0f, 1.0f);
    UpdateCubemap();
}

void Skybox::SetSkyType(SkyType type) {
    skyType = type;
    GenerateCubemap();
}

void Skybox::Update(float deltaTime) {
    // Animate sky if needed
    static float animationSpeed = 0.1f;
    timeOfDay += animationSpeed * deltaTime / 86400.0f; // 24-hour cycle
    if (timeOfDay > 1.0f) {
        timeOfDay -= 1.0f;
    }
    UpdateCubemap();
}

void Skybox::Draw() const {
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

GLuint Skybox::GetCubemap() const {
    return cubemapTexture;
}

void Skybox::InitializeGeometry() {
    // Skybox cube vertices (positions only)
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Skybox::GenerateCubemap() {
    if (cubemapTexture != 0) {
        glDeleteTextures(1, &cubemapTexture);
    }

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Generate cubemap faces based on sky type and time of day
    for (unsigned int i = 0; i < 6; ++i) {
        GenerateCubemapFace(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i));
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::GenerateCubemapFace(GLenum target) {
    const int size = 512;
    std::vector<unsigned char> data(size * size * 3);

    // Generate sky color based on time of day and sky type
    glm::vec3 skyColor = GetSkyColor();
    glm::vec3 horizonColor = GetHorizonColor();
    glm::vec3 zenithColor = GetZenithColor();

    for (int y = 0; y < size; ++y) {
        for (int x = 0; x < size; ++x) {
            float nx = (float)x / size * 2.0f - 1.0f;
            float ny = (float)y / size * 2.0f - 1.0f;
            float nz = 1.0f;

            // Convert to world space based on face
            glm::vec3 direction;
            switch (target) {
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X: direction = glm::vec3(1.0f, -ny, -nx); break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: direction = glm::vec3(-1.0f, -ny, nx); break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y: direction = glm::vec3(nx, 1.0f, -ny); break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: direction = glm::vec3(nx, -1.0f, ny); break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: direction = glm::vec3(nx, -ny, 1.0f); break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: direction = glm::vec3(-nx, -ny, -1.0f); break;
            }

            direction = glm::normalize(direction);
            glm::vec3 color = CalculateSkyColor(direction, skyColor, horizonColor, zenithColor);

            int index = (y * size + x) * 3;
            data[index] = static_cast<unsigned char>(color.r * 255);
            data[index + 1] = static_cast<unsigned char>(color.g * 255);
            data[index + 2] = static_cast<unsigned char>(color.b * 255);
        }
    }

    glTexImage2D(target, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
}

glm::vec3 Skybox::GetSkyColor() const {
    switch (skyType) {
        case SkyType::CLEAR_DAY:
            return glm::vec3(0.5f, 0.7f, 1.0f);
        case SkyType::CLOUDY_DAY:
            return glm::vec3(0.6f, 0.7f, 0.8f);
        case SkyType::SUNSET:
            return glm::vec3(1.0f, 0.6f, 0.4f);
        case SkyType::NIGHT:
            return glm::vec3(0.05f, 0.05f, 0.1f);
        default:
            return glm::vec3(0.5f, 0.7f, 1.0f);
    }
}

glm::vec3 Skybox::GetHorizonColor() const {
    switch (skyType) {
        case SkyType::CLEAR_DAY:
            return glm::vec3(0.8f, 0.9f, 1.0f);
        case SkyType::CLOUDY_DAY:
            return glm::vec3(0.7f, 0.8f, 0.9f);
        case SkyType::SUNSET:
            return glm::vec3(1.0f, 0.4f, 0.2f);
        case SkyType::NIGHT:
            return glm::vec3(0.1f, 0.1f, 0.2f);
        default:
            return glm::vec3(0.8f, 0.9f, 1.0f);
    }
}

glm::vec3 Skybox::GetZenithColor() const {
    switch (skyType) {
        case SkyType::CLEAR_DAY:
            return glm::vec3(0.3f, 0.5f, 0.8f);
        case SkyType::CLOUDY_DAY:
            return glm::vec3(0.4f, 0.5f, 0.6f);
        case SkyType::SUNSET:
            return glm::vec3(0.8f, 0.3f, 0.1f);
        case SkyType::NIGHT:
            return glm::vec3(0.02f, 0.02f, 0.05f);
        default:
            return glm::vec3(0.3f, 0.5f, 0.8f);
    }
}

glm::vec3 Skybox::CalculateSkyColor(const glm::vec3& direction, const glm::vec3& skyColor, 
                                   const glm::vec3& horizonColor, const glm::vec3& zenithColor) const {
    // Calculate elevation angle (0 = horizon, 1 = zenith)
    float elevation = (direction.y + 1.0f) * 0.5f;
    
    // Interpolate between horizon and zenith colors
    glm::vec3 baseColor = glm::mix(horizonColor, zenithColor, elevation);
    
    // Add time of day variation
    float timeVariation = sin(timeOfDay * 2.0f * M_PI);
    baseColor += glm::vec3(timeVariation * 0.1f);
    
    // Add atmospheric scattering effect
    float scattering = 1.0f - elevation * 0.3f;
    baseColor *= scattering;
    
    // Clamp to valid range
    baseColor = glm::clamp(baseColor, 0.0f, 1.0f);
    
    return baseColor;
}

void Skybox::UpdateCubemap() {
    // Regenerate cubemap with new time of day
    for (unsigned int i = 0; i < 6; ++i) {
        GenerateCubemapFace(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i));
    }
}
