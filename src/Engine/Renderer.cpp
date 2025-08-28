#include "Engine/Renderer.h"
#include "Engine/Shader.h"
#include "Engine/Camera.h"
#include "Engine/Scene.h"
#include "Engine/Light.h"
#include "Engine/Model.h"
#include "Engine/Mesh.h"
#include "Engine/Texture.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Renderer::Renderer(int width, int height) 
    : width(width), height(height), fps(0.0f), drawCalls(0), lastFrameTime(0.0),
      depthTestEnabled(true), cullingEnabled(true), blendingEnabled(true),
      shadowMapFBO(0), shadowMap(0) {
}

Renderer::~Renderer() {
    if (shadowMapFBO != 0) {
        glDeleteFramebuffers(1, &shadowMapFBO);
    }
    if (shadowMap != 0) {
        glDeleteTextures(1, &shadowMap);
    }
}

void Renderer::Initialize() {
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }
    
    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    
    // Load shaders
    mainShader = std::make_unique<Shader>();
    if (!mainShader->LoadFromFiles("shaders/vertex/main.vert", "shaders/fragment/main.frag")) {
        std::cerr << "Failed to load main shader" << std::endl;
        return;
    }
    
    shadowShader = std::make_unique<Shader>();
    if (!shadowShader->LoadFromFiles("shaders/vertex/shadow.vert", "shaders/fragment/shadow.frag")) {
        std::cerr << "Failed to load shadow shader" << std::endl;
        return;
    }
    
    skyboxShader = std::make_unique<Shader>();
    if (!skyboxShader->LoadFromFiles("shaders/vertex/skybox.vert", "shaders/fragment/skybox.frag")) {
        std::cerr << "Failed to load skybox shader" << std::endl;
        return;
    }
    
    // Setup shadow mapping
    SetupShadowMapping();
    
    std::cout << "Renderer initialized successfully" << std::endl;
}

void Renderer::SetViewport(int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, width, height);
}

void Renderer::EnableFeature(GLenum feature) {
    glEnable(feature);
}

void Renderer::DisableFeature(GLenum feature) {
    glDisable(feature);
}

void Renderer::BeginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawCalls = 0;
}

void Renderer::Render(const Scene& scene, const Camera& camera) {
    // Get view and projection matrices
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
    
    // Render shadow maps first
    const auto& lights = scene.GetLights();
    if (!lights.empty()) {
        RenderShadowMap(scene, *lights[0]);
    }
    
    // Use main shader for scene rendering
    mainShader->Use();
    
    // Set camera matrices
    mainShader->SetMat4("view", viewMatrix);
    mainShader->SetMat4("projection", projectionMatrix);
    mainShader->SetVec3("viewPos", camera.GetPosition());
    
    // Set lighting
    mainShader->SetVec3("ambientLight", scene.GetAmbientLight());
    mainShader->SetInt("numLights", static_cast<int>(lights.size()));
    
    for (size_t i = 0; i < lights.size() && i < 16; ++i) {
        const auto& light = lights[i];
        std::string prefix = "lights[" + std::to_string(i) + "].";
        
        mainShader->SetInt(prefix + "type", static_cast<int>(light->GetType()));
        mainShader->SetVec3(prefix + "position", light->GetPosition());
        mainShader->SetVec3(prefix + "direction", light->GetDirection());
        mainShader->SetVec3(prefix + "color", light->GetColor());
        mainShader->SetFloat(prefix + "intensity", light->GetIntensity());
    }
    
    // Render all models in the scene
    for (const auto& model : scene.GetModels()) {
        // Set model matrix
        glm::mat4 modelMatrix = model->GetTransform();
        mainShader->SetMat4("model", modelMatrix);
        
        // Set material properties
        const auto& material = model->GetMaterial();
        mainShader->SetVec3("material.albedo", material.albedo);
        mainShader->SetFloat("material.metallic", material.metallic);
        mainShader->SetFloat("material.roughness", material.roughness);
        mainShader->SetFloat("material.ao", material.ao);
        
        // Render meshes
        for (const auto& mesh : model->GetMeshes()) {
            mesh->Render();
            drawCalls++;
        }
    }
    
    mainShader->Unuse();
    
    // Render skybox
    RenderSkybox(scene, camera);
}

void Renderer::EndFrame() {
    // Update FPS counter
    double currentTime = glfwGetTime();
    if (currentTime - lastFrameTime >= 1.0) {
        fps = 1.0f / (currentTime - lastFrameTime);
        lastFrameTime = currentTime;
    }
}

void Renderer::SetupShadowMapping() {
    // Create shadow map texture
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Create framebuffer for shadow mapping
    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderShadowMap(const Scene& scene, const Light& light) {
    // For now, just render a simple shadow map
    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    shadowShader->Use();
    
    // Calculate light space matrix
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    glm::mat4 lightView = glm::lookAt(
        glm::vec3(0.0f, 10.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    
    shadowShader->SetMat4("lightSpaceMatrix", lightSpaceMatrix);
    
    // Render scene to shadow map
    for (const auto& model : scene.GetModels()) {
        glm::mat4 modelMatrix = model->GetTransform();
        shadowShader->SetMat4("model", modelMatrix);
        
        for (const auto& mesh : model->GetMeshes()) {
            mesh->Render();
        }
    }
    
    shadowShader->Unuse();
    
    // Restore viewport
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

void Renderer::RenderScene(const Scene& scene, const Camera& camera, const Light& light) {
    // This is handled in the main Render method
}

void Renderer::RenderSkybox(const Scene& scene, const Camera& camera) {
    // For now, just render a simple skybox
    if (scene.GetSkybox()) {
        skyboxShader->Use();
        
        // Remove translation from view matrix for skybox
        glm::mat4 viewMatrix = camera.GetViewMatrix();
        glm::mat4 skyboxView = glm::mat4(glm::mat3(viewMatrix));
        skyboxShader->SetMat4("view", skyboxView);
        skyboxShader->SetMat4("projection", camera.GetProjectionMatrix());
        
        // Render skybox
        scene.GetSkybox()->Render(camera.GetProjectionMatrix() * skyboxView);
        
        skyboxShader->Unuse();
    }
}

bool Renderer::IsInFrustum(const glm::vec3& position, float radius) {
    // Simple frustum culling check
    for (int i = 0; i < 6; ++i) {
        if (glm::dot(glm::vec3(frustumPlanes[i]), position) + frustumPlanes[i].w < -radius) {
            return false;
        }
    }
    return true;
}

void Renderer::UpdateFrustum(const Camera& camera) {
    // Calculate frustum planes from camera
    glm::mat4 viewProjection = camera.GetProjectionMatrix() * camera.GetViewMatrix();
    
    // Extract frustum planes
    // Left plane
    frustumPlanes[0].x = viewProjection[0][3] + viewProjection[0][0];
    frustumPlanes[0].y = viewProjection[1][3] + viewProjection[1][0];
    frustumPlanes[0].z = viewProjection[2][3] + viewProjection[2][0];
    frustumPlanes[0].w = viewProjection[3][3] + viewProjection[3][0];
    
    // Right plane
    frustumPlanes[1].x = viewProjection[0][3] - viewProjection[0][0];
    frustumPlanes[1].y = viewProjection[1][3] - viewProjection[1][0];
    frustumPlanes[1].z = viewProjection[2][3] - viewProjection[2][0];
    frustumPlanes[1].w = viewProjection[3][3] - viewProjection[3][0];
    
    // Bottom plane
    frustumPlanes[2].x = viewProjection[0][3] + viewProjection[0][1];
    frustumPlanes[2].y = viewProjection[1][3] + viewProjection[1][1];
    frustumPlanes[2].z = viewProjection[2][3] + viewProjection[2][1];
    frustumPlanes[2].w = viewProjection[3][3] + viewProjection[3][1];
    
    // Top plane
    frustumPlanes[3].x = viewProjection[0][3] - viewProjection[0][1];
    frustumPlanes[3].y = viewProjection[1][3] - viewProjection[1][1];
    frustumPlanes[3].z = viewProjection[2][3] - viewProjection[2][1];
    frustumPlanes[3].w = viewProjection[3][3] - viewProjection[3][1];
    
    // Near plane
    frustumPlanes[4].x = viewProjection[0][3] + viewProjection[0][2];
    frustumPlanes[4].y = viewProjection[1][3] + viewProjection[1][2];
    frustumPlanes[4].z = viewProjection[2][3] + viewProjection[2][2];
    frustumPlanes[4].w = viewProjection[3][3] + viewProjection[3][2];
    
    // Far plane
    frustumPlanes[5].x = viewProjection[0][3] - viewProjection[0][2];
    frustumPlanes[5].y = viewProjection[1][3] - viewProjection[1][2];
    frustumPlanes[5].z = viewProjection[2][3] - viewProjection[2][2];
    frustumPlanes[5].w = viewProjection[3][3] - viewProjection[3][2];
    
    // Normalize planes
    for (int i = 0; i < 6; ++i) {
        float length = glm::length(glm::vec3(frustumPlanes[i]));
        frustumPlanes[i] /= length;
    }
}
