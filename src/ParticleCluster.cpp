#include "ParticleCluster.h"
using namespace ParticleUtils;

PCGRNG rng(2341);

ParticleCluster::ParticleCluster(size_t initCount) //: particles(initCount)
{
    count = initCount;
    //SetDefaults();

    for (int i = 0; i < count; ++i)
    {
        //particles[i].launch = floatToHalf(rng.Float(0.0f, 0.0f));
        //particles[i].death = floatToHalf(rng.Float(0.0f, 0.0f));
    }
}

void ParticleCluster::SetOnCPU()
{
    for (int i = 0; i < count; ++i)
    {
        particles[i].launch = floatToHalf(rng.Float(0.0f, 0.0f));
        particles[i].death = floatToHalf(rng.Float(2.0f, 2.0f));

        particles[i].vel.vec = DirectionAngleVector(vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0001f, 360.0f, 1, 1, 1);
        particles[i].pos.vec = vec3(0, 0, 0);
        particles[i].grav.vec = vec3(DirectionAngleVector(vec3(0.0f, 1.0f, 0.0f), 3.0f, .3f, 360.0f, 1, 1, 1));

        particles[i].rot = floatToHalf(rng.Float(0.0f, TWO_PI));
        particles[i].spin = floatToHalf(rng.Float(-TWO_PI, TWO_PI)); // Full rotation per second when spin is 2π

        // Size
        particles[i].sizeData.start = floatToHalf(rng.Float(0.4,0.4));
        particles[i].sizeData.end = floatToHalf(rng.Float(0.65,0.65));
        particles[i].sizeData.lowLimit = floatToHalf(rng.Float(0.1,0));
        particles[i].sizeData.upLimit = floatToHalf(rng.Float(0.9,1));
        particles[i].sizeData.startRot = floatToHalf(rng.Float(0.0,0.1));
        particles[i].sizeData.endRot = floatToHalf(rng.Float(0.0,0.1));
        particles[i].sizeData.scale = floatToHalf(rng.Float(1.0,1.0));
        particles[i].sizeData.loop = static_cast<unsigned short>(rng.Int(1,1));

        // Acceleration
        particles[i].accelData.start = floatToHalf(rng.Float(0.0,0.0));
        particles[i].accelData.end = floatToHalf(rng.Float(0.9,1.0));
        particles[i].accelData.lowLimit = floatToHalf(rng.Float(0.1,0));
        particles[i].accelData.upLimit = floatToHalf(rng.Float(0.9,1));
        particles[i].accelData.startRot = floatToHalf(rng.Float(0.0,0.2));
        particles[i].accelData.endRot = floatToHalf(rng.Float(0.0,0.2));
        particles[i].accelData.scale = floatToHalf(rng.Float(0.5,0.1));
        particles[i].accelData.loop = static_cast<unsigned short>(rng.Int(1,5));

        // Color
        particles[i].colorData.start = floatToHalf(rng.Float(0.0,0.2));
        particles[i].colorData.end = floatToHalf(rng.Float(0.8,1.0));
        particles[i].colorData.lowLimit = floatToHalf(rng.Float(0.2,0));
        particles[i].colorData.upLimit = floatToHalf(rng.Float(0.8,1));
        particles[i].colorData.startRot = floatToHalf(rng.Float(-0.1,0.1));
        particles[i].colorData.endRot = floatToHalf(rng.Float(-0.5,-0.1));
        particles[i].colorData.scale = floatToHalf(rng.Float(1.0,1.0));
        particles[i].colorData.loop = static_cast<unsigned short>(rng.Int(1,1));

        // Alpha
        particles[i].alphaData.start = floatToHalf(rng.Float(0.0,0.2));
        particles[i].alphaData.end = floatToHalf(rng.Float(0.8,1.0));
        particles[i].alphaData.lowLimit = floatToHalf(rng.Float(0.2,0));
        particles[i].alphaData.upLimit = floatToHalf(rng.Float(0.8,1));
        particles[i].alphaData.startRot = floatToHalf(rng.Float(0.0,0.1));
        particles[i].alphaData.endRot = floatToHalf(rng.Float(0.0,0.0));
        particles[i].alphaData.scale = floatToHalf(rng.Float(0.8,1.0));
        particles[i].alphaData.loop = static_cast<unsigned short>(rng.Int(1,3));
    }
}

vec3 ParticleCluster::VectorFromRadius(float radiusMin, float radiusMax, vec3 origin)
{
    float radius = rng.Float(2.0f, 2.0f);
    float u = rng.Float(0.0f, 1.0f);
    float v = rng.Float(0.0f, 1.0f);

    float theta = glm::two_pi<float>() * u;
    float phi = std::acos(2.0f * v - 1.0f);

    float x = radius * std::sin(phi) * std::cos(theta);
    float y = radius * std::sin(phi) * std::sin(theta);
    float z = radius * std::cos(phi);
    return vec3(x, y, z) + origin;
}

vec3 ParticleCluster::DirectionAngleVector(vec3 direction, float minRadius, float maxRadius, float spreadAngleDegrees, float xMult, float yMult, float zMult)
{
    float radius = rng.Float(minRadius, maxRadius);
    float u = rng.Float(0.0f, 1.0f);
    float v = rng.Float(0.0f, 1.0f);
    // Calculate theta and phi, but limit phi based on the spread angle
    float theta = glm::two_pi<float>() * u;
    float maxPhi = glm::radians(spreadAngleDegrees) / 2.0f; // Half the spread angle
    float phi = std::acos(1.0f - v * (1.0f - std::cos(maxPhi))); // This maps v to [0, maxPhi]

    // Generate a point on a sphere
    glm::vec3 spherePoint(
        std::sin(phi) * std::cos(theta) * xMult,
        std::sin(phi) * std::sin(theta) * yMult,
        std::cos(phi) * zMult
    );

    // Create a rotation from the z-axis to our direction vector
    vec3 zAxis(0, 0, 1);
    vec3 rotationAxis = cross(zAxis, normalize(direction));
    float rotationAngle = std::acos(dot(zAxis, normalize(direction)));
    // Create and apply the rotation
    quat rotation = angleAxis(rotationAngle, normalize(rotationAxis));

    return rotation * (spherePoint * radius);
}

ParticleCluster::~ParticleCluster()
{
}


void ParticleCluster::SetDefaults()
{
    //Setting default values
}
