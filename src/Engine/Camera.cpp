#include "Engine/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, float fov)
    : position(position), target(target), up(0.0f, 1.0f, 0.0f),
      fov(fov), nearPlane(0.1f), farPlane(1000.0f), aspectRatio(16.0f/9.0f),
      yaw(-90.0f), pitch(0.0f), sensitivity(0.1f), zoom(1.0f) {
    
    UpdateCameraVectors();
}

Camera::~Camera() {
}

void Camera::SetPosition(const glm::vec3& pos) {
    position = pos;
    UpdateCameraVectors();
}

void Camera::SetTarget(const glm::vec3& tgt) {
    target = tgt;
    UpdateCameraVectors();
}

void Camera::SetFOV(float fieldOfView) {
    fov = fieldOfView;
}

void Camera::SetNearPlane(float near) {
    nearPlane = near;
}

void Camera::SetFarPlane(float far) {
    farPlane = far;
}

void Camera::SetAspectRatio(float aspect) {
    aspectRatio = aspect;
}

void Camera::Move(const glm::vec3& offset) {
    position += offset;
    target += offset;
}

void Camera::Rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw * sensitivity;
    pitch += deltaPitch * sensitivity;
    
    // Constrain pitch
    pitch = std::clamp(pitch, -89.0f, 89.0f);
    
    UpdateCameraVectors();
}

void Camera::Zoom(float factor) {
    zoom *= factor;
    zoom = std::clamp(zoom, 0.1f, 10.0f);
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix() const {
    return glm::perspective(glm::radians(fov * zoom), aspectRatio, nearPlane, farPlane);
}

glm::vec3 Camera::GetPosition() const {
    return position;
}

glm::vec3 Camera::GetTarget() const {
    return target;
}

glm::vec3 Camera::GetFront() const {
    return front;
}

glm::vec3 Camera::GetRight() const {
    return right;
}

glm::vec3 Camera::GetUp() const {
    return up;
}

float Camera::GetYaw() const {
    return yaw;
}

float Camera::GetPitch() const {
    return pitch;
}

void Camera::UpdateCameraVectors() {
    // Calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    
    // Recalculate the right and up vector
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

// First-person camera controls
void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = cameraSpeed * deltaTime;
    
    switch (direction) {
        case CameraMovement::FORWARD:
            position += front * velocity;
            break;
        case CameraMovement::BACKWARD:
            position -= front * velocity;
            break;
        case CameraMovement::LEFT:
            position -= right * velocity;
            break;
        case CameraMovement::RIGHT:
            position += right * velocity;
            break;
        case CameraMovement::UP:
            position += up * velocity;
            break;
        case CameraMovement::DOWN:
            position -= up * velocity;
            break;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        pitch = std::clamp(pitch, -89.0f, 89.0f);
    }
    
    // Update front, right and up vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    zoom -= yoffset;
    zoom = std::clamp(zoom, 0.1f, 10.0f);
}

// Orbital camera controls
void Camera::SetOrbitalTarget(const glm::vec3& center) {
    orbitalCenter = center;
    UpdateOrbitalPosition();
}

void Camera::OrbitalRotate(float deltaYaw, float deltaPitch) {
    orbitalYaw += deltaYaw;
    orbitalPitch += deltaPitch;
    orbitalPitch = std::clamp(orbitalPitch, -89.0f, 89.0f);
    
    UpdateOrbitalPosition();
}

void Camera::OrbitalZoom(float factor) {
    orbitalDistance *= factor;
    orbitalDistance = std::clamp(orbitalDistance, 1.0f, 100.0f);
    UpdateOrbitalPosition();
}

void Camera::UpdateOrbitalPosition() {
    float x = orbitalDistance * cos(glm::radians(orbitalYaw)) * cos(glm::radians(orbitalPitch));
    float y = orbitalDistance * sin(glm::radians(orbitalPitch));
    float z = orbitalDistance * sin(glm::radians(orbitalYaw)) * cos(glm::radians(orbitalPitch));
    
    position = orbitalCenter + glm::vec3(x, y, z);
    target = orbitalCenter;
    UpdateCameraVectors();
}

// Frustum culling
Frustum Camera::GetFrustum() const {
    Frustum frustum;
    
    glm::mat4 viewProjection = GetProjectionMatrix() * GetViewMatrix();
    
    // Extract frustum planes
    // Left plane
    frustum.planes[0].x = viewProjection[0][3] + viewProjection[0][0];
    frustum.planes[0].y = viewProjection[1][3] + viewProjection[1][0];
    frustum.planes[0].z = viewProjection[2][3] + viewProjection[2][0];
    frustum.planes[0].w = viewProjection[3][3] + viewProjection[3][0];
    
    // Right plane
    frustum.planes[1].x = viewProjection[0][3] - viewProjection[0][0];
    frustum.planes[1].y = viewProjection[1][3] - viewProjection[1][0];
    frustum.planes[1].z = viewProjection[2][3] - viewProjection[2][0];
    frustum.planes[1].w = viewProjection[3][3] - viewProjection[3][0];
    
    // Bottom plane
    frustum.planes[2].x = viewProjection[0][3] + viewProjection[0][1];
    frustum.planes[2].y = viewProjection[1][3] + viewProjection[1][1];
    frustum.planes[2].z = viewProjection[2][3] + viewProjection[2][1];
    frustum.planes[2].w = viewProjection[3][3] + viewProjection[3][1];
    
    // Top plane
    frustum.planes[3].x = viewProjection[0][3] - viewProjection[0][1];
    frustum.planes[3].y = viewProjection[1][3] - viewProjection[1][1];
    frustum.planes[3].z = viewProjection[2][3] - viewProjection[2][1];
    frustum.planes[3].w = viewProjection[3][3] - viewProjection[3][1];
    
    // Near plane
    frustum.planes[4].x = viewProjection[0][3] + viewProjection[0][2];
    frustum.planes[4].y = viewProjection[1][3] + viewProjection[1][2];
    frustum.planes[4].z = viewProjection[2][3] + viewProjection[2][2];
    frustum.planes[4].w = viewProjection[3][3] + viewProjection[3][2];
    
    // Far plane
    frustum.planes[5].x = viewProjection[0][3] - viewProjection[0][2];
    frustum.planes[5].y = viewProjection[1][3] - viewProjection[1][2];
    frustum.planes[5].z = viewProjection[2][3] - viewProjection[2][2];
    frustum.planes[5].w = viewProjection[3][3] - viewProjection[3][2];
    
    // Normalize planes
    for (int i = 0; i < 6; ++i) {
        float length = glm::length(glm::vec3(frustum.planes[i]));
        frustum.planes[i] /= length;
    }
    
    return frustum;
}
