#include "Camera.h"

#include "ComputeShader.h"


Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane, float radius, const vec3& target)
    : fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane),
    target(target), up(vec3(0.0f, 1.0f, 0.0f)), radius(radius), azimuth(0.0f), elevation(0.0f)
{
    updatePosition();
}

Camera::~Camera() {}

void Camera::updatePosition() {
    float x = radius * cos(elevation) * cos(azimuth);
    float y = radius * sin(elevation);
    float z = radius * cos(elevation) * sin(azimuth);
    position = target + vec3(x, y, z);
}

mat4 Camera::getCalculateViewMatrix() const {
    return glm::lookAt(position, target, up);
}

mat4 Camera::getCalculateProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::bind(BaseShader& shader) const {
    shader.Bind();
    shader.SetUniform("view", getCalculateViewMatrix());
    shader.SetUniform("projection", getCalculateProjectionMatrix());
    //shader.SetUniform("camPos", position);
}

void Camera::bind(ComputeShader& shader) const {
    shader.Bind();
    shader.SetUniform("camPos", position);
}

void Camera::rotateHorizontal(float angle) {
    azimuth += glm::radians(angle);
    updatePosition();
}

void Camera::rotateVertical(float angle) {
    elevation += glm::radians(angle);
    elevation = clamp(elevation, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
    updatePosition();
}

void Camera::zoom(float amount) {
    radius += amount;
    radius = std::max(radius, 0.1f); // Prevent zooming through the target
    updatePosition();
}