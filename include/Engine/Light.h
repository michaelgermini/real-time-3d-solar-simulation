#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class Light {
public:
    enum class LightType {
        DIRECTIONAL,
        POINT,
        SPOT
    };

    Light();
    Light(LightType type, const glm::vec3& position, const glm::vec3& direction = glm::vec3(0.0f, -1.0f, 0.0f));

    // Light properties
    void SetPosition(const glm::vec3& position);
    void SetDirection(const glm::vec3& direction);
    void SetColor(const glm::vec3& color);
    void SetIntensity(float intensity);
    void SetAmbient(float ambient);
    void SetDiffuse(float diffuse);
    void SetSpecular(float specular);

    // Attenuation (for point and spot lights)
    void SetAttenuation(float constant, float linear, float quadratic);
    void SetRange(float range);

    // Spot light specific
    void SetCutOff(float cutOff);
    void SetOuterCutOff(float outerCutOff);

    // Shadow mapping
    void EnableShadows(bool enable);
    void SetShadowMapSize(int size);
    glm::mat4 GetLightSpaceMatrix() const;
    GLuint GetShadowMap() const { return shadowMap; }

    // Getters
    LightType GetType() const { return type; }
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetDirection() const { return direction; }
    glm::vec3 GetColor() const { return color; }
    float GetIntensity() const { return intensity; }
    float GetAmbient() const { return ambient; }
    float GetDiffuse() const { return diffuse; }
    float GetSpecular() const { return specular; }
    bool IsShadowEnabled() const { return shadowsEnabled; }

    // Update
    void Update();
    
    // Factory methods
    static std::shared_ptr<Light> CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color);
    static std::shared_ptr<Light> CreatePointLight(const glm::vec3& position, const glm::vec3& color);
    static std::shared_ptr<Light> CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color);

private:
    LightType type;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float ambient;
    float diffuse;
    float specular;

    // Attenuation
    float constant;
    float linear;
    float quadratic;
    float range;

    // Spot light
    float cutOff;
    float outerCutOff;

    // Shadows
    bool shadowsEnabled;
    GLuint shadowMapFBO;
    GLuint shadowMap;
    int shadowMapSize;
    float shadowNearPlane;
    float shadowFarPlane;

    void SetupShadowMap();
    void UpdateShadowMap();
};
