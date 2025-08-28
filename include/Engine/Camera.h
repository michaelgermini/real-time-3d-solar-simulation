#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
    enum class CameraType {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    Camera();
    Camera(const glm::vec3& position, const glm::vec3& target, float fov = 45.0f);

    // Camera movement
    enum class CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    
    void Move(const glm::vec3& offset);
    void Rotate(float yaw, float pitch);
    void Zoom(float factor);
    void SetPosition(const glm::vec3& position);
    void SetTarget(const glm::vec3& target);
    void SetUp(const glm::vec3& up);
    
    // Input processing
    void ProcessKeyboard(CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

    // Matrix generation
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewProjectionMatrix() const;

    // Getters
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetFront() const { return front; }
    glm::vec3 GetRight() const { return right; }
    glm::vec3 GetUp() const { return up; }
    glm::vec3 GetTarget() const { return target; }
    float GetFOV() const { return fov; }
    float GetNearPlane() const { return nearPlane; }
    float GetFarPlane() const { return farPlane; }

    // Setters
    void SetFOV(float fov);
    void SetNearPlane(float near);
    void SetFarPlane(float far);
    void SetAspectRatio(float aspect);
    void SetCameraType(CameraType type);

    // Frustum
    void UpdateFrustum();
    bool IsPointInFrustum(const glm::vec3& point) const;
    bool IsSphereInFrustum(const glm::vec3& center, float radius) const;

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;

    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    CameraType type;
    
    // Camera settings
    float cameraSpeed;
    float mouseSensitivity;
    float yaw;
    float pitch;
    float zoom;

    // Frustum planes
    glm::vec4 frustumPlanes[6];

    void UpdateCameraVectors();
    void CalculateFrustumPlanes();
};
