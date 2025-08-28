#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <memory>

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Scene.h"

class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    void Initialize();
    void Render(const Scene& scene, const Camera& camera);
    void SetViewport(int width, int height);
    void EnableFeature(GLenum feature);
    void DisableFeature(GLenum feature);
    
    // Performance monitoring
    void BeginFrame();
    void EndFrame();
    float GetFPS() const { return fps; }
    int GetDrawCalls() const { return drawCalls; }

private:
    int width, height;
    float fps;
    int drawCalls;
    double lastFrameTime;
    
    // Rendering state
    bool depthTestEnabled;
    bool cullingEnabled;
    bool blendingEnabled;
    
    // Shaders
    std::unique_ptr<Shader> mainShader;
    std::unique_ptr<Shader> shadowShader;
    std::unique_ptr<Shader> skyboxShader;
    
    // Framebuffers
    GLuint shadowMapFBO;
    GLuint shadowMap;
    
    void SetupShadowMapping();
    void RenderShadowMap(const Scene& scene, const Light& light);
    void RenderScene(const Scene& scene, const Camera& camera, const Light& light);
    void RenderSkybox(const Scene& scene, const Camera& camera);
    
    // Frustum culling
    bool IsInFrustum(const glm::vec3& position, float radius);
    void UpdateFrustum(const Camera& camera);
    glm::vec4 frustumPlanes[6];
};
