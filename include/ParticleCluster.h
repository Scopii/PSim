#pragma once
#include "Types.h"
#include "ParticleUtils.h"

struct ParticleCluster {

    struct AttribGradient
    {
        half start;
        half end;
        half lowLimit;
        half upLimit;
        half startRot;
        half endRot;
        half scale;
        uint16_t loop;
    };

    struct alignas(16) Particle
    {
        half launch;
        half death;
        alignedVec3 vel;
        alignedVec3 pos;
        alignedVec3 grav;
        half rot;
        half spin;
        AttribGradient sizeData;
        AttribGradient accelData;
        AttribGradient colorData;
        AttribGradient alphaData;
    };

    vectorArray<Particle> particles;
    size_t count = 0;

    ParticleCluster(size_t count);
    ~ParticleCluster();
    void SetDefaults();
    void SetOnCPU();
    vec3 VectorFromRadius(float radiusMin, float radiusMax , vec3 origin = vec3(0,0,0));
    vec3 DirectionAngleVector(vec3 direction, float minRadius, float maxRadius, float spreadAngleDegrees, float xMult, float yMult, float zMult);
};