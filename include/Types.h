#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <vector>

#include <cstdint>
#include <limits>
#include <glm/glm.hpp>

#include "PCGRNG.cpp"
// Define a half-float type
using half = uint16_t;

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int NUM_PARTICLES = 10000000; //10*100*100*100
const int WORK_GROUP_SIZE = 256;

const float TWO_PI = 2.0f * M_PI;

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat quat;


template<typename T>
using vectorArray = std::vector<T>;

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

enum GLBufferUsage {
    STATIC = GL_STATIC_DRAW,
    DYNAMIC = GL_DYNAMIC_DRAW,
    STREAM = GL_STREAM_DRAW
};

struct Vertex {
    vec3 position;
    vec4 color;
    // Add more attributes here
};

struct alignas(16) alignedVec3{
    vec3 vec;
};

struct RandAttributeGradient
{
    float startMin;
    float startMax;
    float endMin;
    float endMax;
    float lowLimitMin;
    float lowLimitMax;
    float upLimitMin;
    float upLimitMax;
    float startRotMin;
    float startRotMax;
    float endRotMin;
    float endRotMax;
    float scaleMin;
    float scaleMax;
    uint32_t loopMin;
    uint32_t loopMax;
};

struct TimeData
{
    float launchMin;
    float launchMax;
    float deathMin;
    float deathMax;
};

struct SpinData
{
    float rotMin;
    float rotMax;
    float spinMin;
    float spinMax;
};

struct DirecAngleVecData
{
    alignedVec3 direction;
    float minRadius;
    float maxRadius;
    float angleDegrees;
    float xMult;
    float yMult;
    float zMult;
};

struct RadiusVecData
{
    float posRadiusMin;
    float posRadiusMax;
    alignedVec3 posOrigin;
};

struct v4 {
    float x, y, z, w;
    v4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

struct ParticleRuleset
{

};