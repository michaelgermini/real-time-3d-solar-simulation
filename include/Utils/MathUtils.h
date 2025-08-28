#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <random>

class MathUtils {
public:
    // Constants
    static constexpr float PI = 3.14159265359f;
    static constexpr float TWO_PI = 6.28318530718f;
    static constexpr float HALF_PI = 1.57079632679f;
    static constexpr float DEG_TO_RAD = PI / 180.0f;
    static constexpr float RAD_TO_DEG = 180.0f / PI;
    
    // Basic math functions
    static float Clamp(float value, float min, float max);
    static float Lerp(float a, float b, float t);
    static float SmoothStep(float edge0, float edge1, float x);
    static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed = INFINITY);
    static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed = INFINITY);
    
    // Vector utilities
    static glm::vec3 Lerp(const glm::vec3& a, const glm::vec3& b, float t);
    static glm::vec3 SmoothDamp(const glm::vec3& current, const glm::vec3& target, glm::vec3& currentVelocity, float smoothTime, float maxSpeed = INFINITY);
    static float Distance(const glm::vec3& a, const glm::vec3& b);
    static float DistanceSquared(const glm::vec3& a, const glm::vec3& b);
    static glm::vec3 Normalize(const glm::vec3& vector);
    static float Length(const glm::vec3& vector);
    static float LengthSquared(const glm::vec3& vector);
    static glm::vec3 Cross(const glm::vec3& a, const glm::vec3& b);
    static float Dot(const glm::vec3& a, const glm::vec3& b);
    
    // Matrix utilities
    static glm::mat4 CreateTransform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
    static glm::mat4 CreateTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
    static glm::mat4 CreateLookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);
    static glm::mat4 CreatePerspective(float fov, float aspect, float near, float far);
    static glm::mat4 CreateOrthographic(float left, float right, float bottom, float top, float near, float far);
    
    // Quaternion utilities
    static glm::quat CreateQuaternion(const glm::vec3& eulerAngles);
    static glm::quat CreateQuaternion(float angle, const glm::vec3& axis);
    static glm::quat Lerp(const glm::quat& a, const glm::quat& b, float t);
    static glm::quat Slerp(const glm::quat& a, const glm::quat& b, float t);
    static glm::vec3 RotateVector(const glm::quat& quaternion, const glm::vec3& vector);
    static glm::vec3 GetEulerAngles(const glm::quat& quaternion);
    
    // Random number generation
    static float RandomFloat(float min = 0.0f, float max = 1.0f);
    static int RandomInt(int min, int max);
    static glm::vec3 RandomVector3(float min = -1.0f, float max = 1.0f);
    static glm::vec3 RandomUnitVector();
    static glm::vec3 RandomPointInSphere(float radius = 1.0f);
    static glm::vec3 RandomPointInCube(float size = 1.0f);
    
    // Noise functions
    static float PerlinNoise(float x, float y);
    static float PerlinNoise(float x, float y, float z);
    static float SimplexNoise(float x, float y);
    static float SimplexNoise(float x, float y, float z);
    static float WorleyNoise(float x, float y, int numPoints = 16);
    
    // Interpolation
    static float EaseInQuad(float t);
    static float EaseOutQuad(float t);
    static float EaseInOutQuad(float t);
    static float EaseInCubic(float t);
    static float EaseOutCubic(float t);
    static float EaseInOutCubic(float t);
    static float EaseInSine(float t);
    static float EaseOutSine(float t);
    static float EaseInOutSine(float t);
    
    // Geometry utilities
    static bool PointInTriangle(const glm::vec3& point, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    static bool RayTriangleIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, 
                                       const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, 
                                       float& t, glm::vec3& barycentric);
    static bool RaySphereIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                                     const glm::vec3& sphereCenter, float sphereRadius,
                                     float& t1, float& t2);
    static bool RayAABBIntersection(const glm::vec3& rayOrigin, const glm::vec3& rayDirection,
                                   const glm::vec3& min, const glm::vec3& max,
                                   float& tMin, float& tMax);
    
    // Bounding box utilities
    static glm::vec3 GetBoundingBoxCenter(const glm::vec3& min, const glm::vec3& max);
    static glm::vec3 GetBoundingBoxSize(const glm::vec3& min, const glm::vec3& max);
    static float GetBoundingSphereRadius(const glm::vec3& min, const glm::vec3& max);
    static bool AABBIntersection(const glm::vec3& min1, const glm::vec3& max1,
                                const glm::vec3& min2, const glm::vec3& max2);
    static bool SphereAABBIntersection(const glm::vec3& sphereCenter, float sphereRadius,
                                      const glm::vec3& min, const glm::vec3& max);
    
    // Frustum utilities
    static void ExtractFrustumPlanes(const glm::mat4& viewProjection, glm::vec4 planes[6]);
    static bool PointInFrustum(const glm::vec3& point, const glm::vec4 planes[6]);
    static bool SphereInFrustum(const glm::vec3& center, float radius, const glm::vec4 planes[6]);
    static bool AABBInFrustum(const glm::vec3& min, const glm::vec3& max, const glm::vec4 planes[6]);
    
    // Color utilities
    static glm::vec3 RGBToHSV(const glm::vec3& rgb);
    static glm::vec3 HSVToRGB(const glm::vec3& hsv);
    static glm::vec3 RGBToHSL(const glm::vec3& rgb);
    static glm::vec3 HSLToRGB(const glm::vec3& hsl);
    static float Luminance(const glm::vec3& color);
    static glm::vec3 GammaCorrect(const glm::vec3& color, float gamma = 2.2f);
    static glm::vec3 GammaUncorrect(const glm::vec3& color, float gamma = 2.2f);

private:
    static std::random_device randomDevice;
    static std::mt19937 randomGenerator;
    static std::uniform_real_distribution<float> floatDistribution;
    static std::uniform_int_distribution<int> intDistribution;
    
    static void InitializeRandom();
    static float Fade(float t);
    static float Grad(int hash, float x, float y, float z);
    static int Permutation[512];
    static bool permutationInitialized;
    static void InitializePermutation();
};
