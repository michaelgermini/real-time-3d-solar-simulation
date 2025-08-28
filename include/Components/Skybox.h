#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

#include "../Engine/Model.h"
#include "../Engine/Texture.h"
#include "../Engine/Shader.h"

class Skybox {
public:
    enum class SkyType {
        CLEAR_DAY,
        CLOUDY_DAY,
        SUNSET,
        NIGHT,
        STORMY,
        CUSTOM
    };

    Skybox();
    Skybox(SkyType type);
    ~Skybox();

    // Skybox setup
    void SetType(SkyType type);
    void LoadCubemap(const std::vector<std::string>& facePaths);
    void SetCubemapTexture(std::shared_ptr<Texture> cubemap);
    void SetAtmosphere(const glm::vec3& skyColor, const glm::vec3& horizonColor, const glm::vec3& groundColor);

    // Atmospheric effects
    void SetTimeOfDay(float time); // 0.0 = midnight, 0.5 = noon, 1.0 = midnight
    void SetWeather(float cloudiness, float humidity);
    void SetWind(float speed, const glm::vec2& direction);
    void SetFog(float density, float height, const glm::vec3& color);

    // Sun and moon
    void SetSunPosition(const glm::vec3& position);
    void SetSunColor(const glm::vec3& color);
    void SetSunIntensity(float intensity);
    void SetMoonPosition(const glm::vec3& position);
    void SetMoonColor(const glm::vec3& color);
    void SetMoonIntensity(float intensity);

    // Stars
    void EnableStars(bool enable);
    void SetStarCount(int count);
    void SetStarBrightness(float brightness);
    void SetStarTwinkle(float twinkle);

    // Clouds
    void EnableClouds(bool enable);
    void SetCloudCoverage(float coverage);
    void SetCloudSpeed(float speed);
    void SetCloudHeight(float height);
    void SetCloudColor(const glm::vec3& color);

    // Rendering
    void Render(const glm::mat4& viewProjection);
    void Update(float deltaTime);
    
    // Getters
    SkyType GetType() const { return type; }
    std::shared_ptr<Texture> GetCubemap() const { return cubemapTexture; }
    glm::vec3 GetSkyColor() const { return skyColor; }
    glm::vec3 GetHorizonColor() const { return horizonColor; }
    glm::vec3 GetGroundColor() const { return groundColor; }
    float GetTimeOfDay() const { return timeOfDay; }
    glm::vec3 GetSunPosition() const { return sunPosition; }
    glm::vec3 GetMoonPosition() const { return moonPosition; }

private:
    SkyType type;
    std::shared_ptr<Texture> cubemapTexture;
    std::shared_ptr<Model> skyboxModel;
    std::shared_ptr<Shader> skyboxShader;
    
    // Colors
    glm::vec3 skyColor;
    glm::vec3 horizonColor;
    glm::vec3 groundColor;
    
    // Time and weather
    float timeOfDay;
    float cloudiness;
    float humidity;
    glm::vec2 windDirection;
    float windSpeed;
    
    // Fog
    float fogDensity;
    float fogHeight;
    glm::vec3 fogColor;
    
    // Sun and moon
    glm::vec3 sunPosition;
    glm::vec3 sunColor;
    float sunIntensity;
    glm::vec3 moonPosition;
    glm::vec3 moonColor;
    float moonIntensity;
    
    // Stars
    bool starsEnabled;
    int starCount;
    float starBrightness;
    float starTwinkle;
    std::vector<glm::vec3> starPositions;
    
    // Clouds
    bool cloudsEnabled;
    float cloudCoverage;
    float cloudSpeed;
    float cloudHeight;
    glm::vec3 cloudColor;
    std::vector<glm::vec3> cloudPositions;
    
    void GenerateSkyboxGeometry();
    void UpdateAtmosphere();
    void UpdateSunPosition();
    void UpdateMoonPosition();
    void GenerateStars();
    void GenerateClouds();
    void UpdateClouds(float deltaTime);
    glm::vec3 InterpolateSkyColor(float time);
};
