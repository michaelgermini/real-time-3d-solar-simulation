#include "Engine/Light.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

Light::Light(LightType type, const glm::vec3& pos, const glm::vec3& dir)
    : type(type), position(pos), direction(glm::normalize(dir)),
      color(1.0f, 1.0f, 1.0f), intensity(1.0f),
      ambient(0.1f), diffuse(0.8f), specular(1.0f),
      constant(1.0f), linear(0.09f), quadratic(0.032f),
      cutOff(12.5f), outerCutOff(17.5f),
      shadowsEnabled(false), shadowMapSize(1024),
      shadowMapFBO(0), shadowMapTexture(0) {
    
    // Initialize shadow mapping for directional and spot lights
    if (type == LightType::DIRECTIONAL || type == LightType::SPOT) {
        InitializeShadowMapping();
    }
}

Light::~Light() {
    if (shadowMapFBO != 0) {
        glDeleteFramebuffers(1, &shadowMapFBO);
    }
    if (shadowMapTexture != 0) {
        glDeleteTextures(1, &shadowMapTexture);
    }
}

void Light::SetPosition(const glm::vec3& pos) {
    position = pos;
}

void Light::SetDirection(const glm::vec3& dir) {
    direction = glm::normalize(dir);
}

void Light::SetColor(const glm::vec3& col) {
    color = col;
}

void Light::SetIntensity(float inten) {
    intensity = inten;
}

void Light::SetAmbient(float amb) {
    ambient = amb;
}

void Light::SetDiffuse(float diff) {
    diffuse = diff;
}

void Light::SetSpecular(float spec) {
    specular = spec;
}

void Light::SetAttenuation(float c, float l, float q) {
    constant = c;
    linear = l;
    quadratic = q;
}

void Light::SetSpotAngles(float cutoff, float outerCutoff) {
    cutOff = cutoff;
    outerCutOff = outerCutoff;
}

void Light::EnableShadows(bool enable) {
    shadowsEnabled = enable;
    if (enable && shadowMapFBO == 0) {
        InitializeShadowMapping();
    }
}

void Light::SetShadowMapSize(int size) {
    shadowMapSize = size;
    if (shadowMapFBO != 0) {
        // Recreate shadow map with new size
        glDeleteFramebuffers(1, &shadowMapFBO);
        glDeleteTextures(1, &shadowMapTexture);
        InitializeShadowMapping();
    }
}

glm::mat4 Light::GetLightSpaceMatrix() const {
    if (type == LightType::DIRECTIONAL) {
        // Orthographic projection for directional light
        float size = 10.0f;
        glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, 0.1f, 100.0f);
        glm::mat4 lightView = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
        return lightProjection * lightView;
    } else if (type == LightType::SPOT) {
        // Perspective projection for spot light
        glm::mat4 lightProjection = glm::perspective(glm::radians(outerCutOff * 2.0f), 1.0f, 0.1f, 100.0f);
        glm::mat4 lightView = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
        return lightProjection * lightView;
    }
    
    return glm::mat4(1.0f);
}

void Light::Update(float deltaTime) {
    // Animate light if needed
    if (animated) {
        time += deltaTime;
        
        // Simple rotation animation
        float radius = 10.0f;
        float speed = 0.5f;
        
        if (type == LightType::POINT || type == LightType::SPOT) {
            position.x = radius * cos(time * speed);
            position.z = radius * sin(time * speed);
        } else if (type == LightType::DIRECTIONAL) {
            // Animate sun position
            float sunAngle = time * speed;
            position.x = radius * cos(sunAngle);
            position.y = radius * sin(sunAngle);
            direction = glm::normalize(-position);
        }
    }
}

void Light::InitializeShadowMapping() {
    // Create shadow map texture
    glGenTextures(1, &shadowMapTexture);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Create framebuffer for shadow mapping
    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::BeginShadowPass() {
    if (!shadowsEnabled) return;
    
    glViewport(0, 0, shadowMapSize, shadowMapSize);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Light::EndShadowPass() {
    if (!shadowsEnabled) return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Light::GetShadowMapTexture() const {
    return shadowMapTexture;
}

// Getters
Light::LightType Light::GetType() const { return type; }
glm::vec3 Light::GetPosition() const { return position; }
glm::vec3 Light::GetDirection() const { return direction; }
glm::vec3 Light::GetColor() const { return color; }
float Light::GetIntensity() const { return intensity; }
float Light::GetAmbient() const { return ambient; }
float Light::GetDiffuse() const { return diffuse; }
float Light::GetSpecular() const { return specular; }
float Light::GetConstant() const { return constant; }
float Light::GetLinear() const { return linear; }
float Light::GetQuadratic() const { return quadratic; }
float Light::GetCutOff() const { return cutOff; }
float Light::GetOuterCutOff() const { return outerCutOff; }
bool Light::GetShadowsEnabled() const { return shadowsEnabled; }
int Light::GetShadowMapSize() const { return shadowMapSize; }

// Factory methods for common light types
std::shared_ptr<Light> Light::CreateDirectionalLight(const glm::vec3& direction, const glm::vec3& color) {
    auto light = std::make_shared<Light>(LightType::DIRECTIONAL, glm::vec3(0.0f, 10.0f, 0.0f), direction);
    light->SetColor(color);
    light->SetIntensity(1.0f);
    light->EnableShadows(true);
    return light;
}

std::shared_ptr<Light> Light::CreatePointLight(const glm::vec3& position, const glm::vec3& color) {
    auto light = std::make_shared<Light>(LightType::POINT, position, glm::vec3(0.0f, -1.0f, 0.0f));
    light->SetColor(color);
    light->SetIntensity(1.0f);
    light->SetAttenuation(1.0f, 0.09f, 0.032f);
    return light;
}

std::shared_ptr<Light> Light::CreateSpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color) {
    auto light = std::make_shared<Light>(LightType::SPOT, position, direction);
    light->SetColor(color);
    light->SetIntensity(1.0f);
    light->SetAttenuation(1.0f, 0.09f, 0.032f);
    light->SetSpotAngles(12.5f, 17.5f);
    light->EnableShadows(true);
    return light;
}
