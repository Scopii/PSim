#pragma once
#include "Types.h"
#include "BaseShader.h"
#include "ComputeShader.h"

class Camera
{
private:
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    vec3 position;
    vec3 target;
    vec3 up;

    float radius;
    float azimuth;   // Horizontal angle
    float elevation; // Vertical angle

    void updatePosition();

    // Helper function for clamping if std::clamp is not available
    template<class T>
    static constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
        return (v < lo) ? lo : (hi < v) ? hi : v;
    }

public:
    Camera(
        float fov = 90.0f,
        float aspectRatio = 1.0f,
        float nearPlane = 0.001f,
        float farPlane = 100.0f,
        float radius = 10.0f,
        const vec3& target = vec3(0.0f)
    );
    ~Camera();

    mat4 getCalculateViewMatrix() const;
    mat4 getCalculateProjectionMatrix() const;
    void bind(BaseShader& shader) const;
    void bind(ComputeShader& shader) const;

    void rotateHorizontal(float angle);
    void rotateVertical(float angle);
    void zoom(float amount);

    // Getters
    vec3 getPosition() const { return position; }
    vec3 getTarget() const { return target; }
    vec3 getUp() const { return up; }
    float getRadius() const { return radius; }
};