#version 450 core
#extension GL_NV_gpu_shader5 : enable

precision highp float;
precision highp int;

const int LINEAR = 0;
const int FAST_START = 1;
const int FAST_END = 2;
const float M_PI = 3.14159; //3.1415927
const float TWO_PI = 2 * M_PI;

float easeOutQuad(float t) {
    return 1.0 - (1.0 - t) * (1.0 - t);
}

float easeInQuad(float t) {
    return t * t;
}

float getProgress(int type, float lifeProgress){
    if (type == LINEAR) return lifeProgress; // Linear transition
    else if (type == FAST_START) return easeOutQuad(lifeProgress); // Ease out (faster at start)
    else if (type == FAST_END) return easeInQuad(lifeProgress); // Ease in (faster at end)
}

vec4 unpackColor(uint packedColor) {
    return vec4(
        float((packedColor >> 24) & 0xFF) / 255.0,
        float((packedColor >> 16) & 0xFF) / 255.0,
        float((packedColor >> 8) & 0xFF) / 255.0,
        float(packedColor & 0xFF) / 255.0
    );
}

vec3 rotateColor(vec3 color, float rotation) {
    vec3 k = vec3(0.57735, 0.57735, 0.57735);
    float cosAngle = cos(rotation);
    return color * cosAngle + cross(k, color) * sin(rotation) + k * dot(k, color) * (1.0 - cosAngle);
}

uint hash(uint x) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Generate a random float between 0 and 1
float random(uint seed) {
    return float(hash(seed)) / 4294967295.0; // 2^32 - 1
}
// Generate a random float between min and max
float randomRange(uint seed, float min, float max) {
    return min + (max - min) * random(seed);
}

float getGradientProgress(float attribStart, float attribEnd, float lowerLimit, float upperLimit, float lifeProgress, int gradientType) {
    float normalizedLife = (lifeProgress - attribStart) / (attribEnd - attribStart);
    float clampedLife = clamp(normalizedLife, 0.0, 1.0);
    return mix(lowerLimit, upperLimit, getProgress(gradientType, clampedLife));
}

struct GradientData{
    int first;
    int second;
    float zoneSize;
    float actualProgress;
};

GradientData getGradientData(int sizesLength, float limitedGradientProgress){
    int maxSizeIndex = sizesLength - 1;
    float zoneSize = 1.0 / float(maxSizeIndex);
    int firstIndex = int(limitedGradientProgress / zoneSize);
    int secondIndex = min(firstIndex + 1, maxSizeIndex);
    float actualProgress = (limitedGradientProgress - float(firstIndex) * zoneSize) / zoneSize;

    return GradientData(firstIndex, secondIndex, zoneSize, actualProgress);
}

// Random Number Generator
uint rng_state = 1u;

void rng_seed(uint seed) {
    rng_state = seed;
}

uint rng_next() {
    rng_state = rng_state * 1664525u + 1013904223u;
    return rng_state;
}

float rng_float(float min, float max) {
    return min + (max - min) * (float(rng_next()) / 4294967295.0);
}

uint rng_int(uint min, uint max) {
    return min + uint(rng_next() % uint(max - min + 1));
}

vec3 VectorFromRadius(float minRadius, float maxRadius, vec3 origin) {
    float radius = rng_float(minRadius, maxRadius);
    float u = rng_float(0.0, 1.0);
    float v = rng_float(0.0, 1.0);

    float theta = 2.0 * 3.14159265359 * u;
    float phi = acos(2.0 * v - 1.0);

    float x = radius * sin(phi) * cos(theta);
    float y = radius * sin(phi) * sin(theta);
    float z = radius * cos(phi);
    return vec3(x, y, z) + origin;
}

vec3 VectorFromDirecAngle(vec3 direction, float minRadius, float maxRadius, float spreadAngleDegrees, float xMult, float yMult, float zMult) {
    float radius = rng_float(minRadius, maxRadius);
    float u = rng_float(0.0, 1.0);
    float v = rng_float(0.0, 1.0);

    float theta = 2.0 * 3.14159265359 * u;
    float maxPhi = radians(spreadAngleDegrees) / 2.0;
    float phi = acos(1.0 - v * (1.0 - cos(maxPhi)));

    vec3 spherePoint = vec3(
        sin(phi) * cos(theta) * xMult,
        sin(phi) * sin(theta) * yMult,
        cos(phi) * zMult
    );

    vec3 zAxis = vec3(0.0, 0.0, 1.0);
    vec3 rotationAxis = cross(zAxis, normalize(direction));
    float rotationAngle = acos(dot(zAxis, normalize(direction)));

    vec3 rotatedPoint = spherePoint * cos(rotationAngle) +
    cross(rotationAxis, spherePoint) * sin(rotationAngle) +
    rotationAxis * dot(rotationAxis, spherePoint) * (1.0 - cos(rotationAngle));

    return rotatedPoint * radius;
}

// Gradients
layout(std430, binding = 1) buffer ColorGradient {
    vec4 colors[];
};
layout(std430, binding = 2) buffer SizeGradient {
    float sizes[];
};
layout(std430, binding = 3) buffer AccelGradient {
    float accels[];
};
layout(std430, binding = 4) buffer AlphaGradient {
    float alphas[];
};

// Attribute (UBO) related
struct TimeData
{
    float launchMin;
    float launchMax;
    float deathMin;
    float deathMax;
};

layout(std140, binding = 0) uniform TimeSettings {
    TimeData timeUBO;
};

struct PositionData {
    float posRadiusMin;
    float posRadiusMax;
    vec4 posOrigin;
};

layout(std140, binding = 1) uniform PositionSettings {
    PositionData posUBO;
};

struct SpinData {
    float rotMin;
    float rotMax;
    float spinMin;
    float spinMax;
};

layout(std140, binding = 2) uniform SpinSettings {
    SpinData spinUBO;
};

struct GradientDataUBO {
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

layout(std140, binding = 3) uniform SizeSettings {
    GradientDataUBO sizeUBO;
};

layout(std140, binding = 4) uniform AccelSettings {
    GradientDataUBO accelUBO;
};

layout(std140, binding = 5) uniform ColorSettings {
    GradientDataUBO colorUBO;
};

layout(std140, binding = 6) uniform AlphaSettings {
    GradientDataUBO alphaUBO;
};

struct DirecAngleVecData {
    vec4 direction;
    float minRadius;
    float maxRadius;
    float angleDegrees;
    float xMult;
    float yMult;
    float zMult;
};

layout(std140, binding = 7) uniform VelocitySettings {
    DirecAngleVecData velUBO;
};

layout(std140, binding = 8) uniform GravitySettings {
    DirecAngleVecData gravUBO;
};


// Particle Related
struct AttribGradient {
    float16_t start;
    float16_t end;
    float16_t lowLimit;
    float16_t upLimit;
    float16_t startRot;
    float16_t endRot;
    float16_t scale;
    uint16_t loop;
};

struct Particle{
    float16_t launch;
    float16_t death;
    vec4 vel;
    vec4 pos;
    vec4 grav;
    float16_t rot;
    float16_t spin;
    AttribGradient sizeData;
    AttribGradient accelData;
    AttribGradient colorData;
    AttribGradient alphaData;
};

layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

AttribGradient updateGradient(GradientDataUBO data){
    AttribGradient temp;
    temp.start = float16_t(rng_float(data.startMin, data.startMax));
    temp.end = float16_t(rng_float(data.endMin, data.endMax));
    temp.lowLimit = float16_t(rng_float(data.lowLimitMin, data.lowLimitMax));
    temp.upLimit = float16_t(rng_float(data.upLimitMin, data.upLimitMax));
    temp.startRot = float16_t(rng_float(data.startRotMin, data.startRotMax));
    temp.endRot = float16_t(rng_float(data.endRotMin, data.endRotMax));
    temp.scale = float16_t(rng_float(data.scaleMin, data.scaleMax));
    temp.loop = uint16_t(rng_int(data.loopMin, data.loopMax));
    return temp;
}

vec4 updateDirecAngleVec(DirecAngleVecData data){
    return vec4(VectorFromDirecAngle(data.direction.xyz, data.minRadius, data.maxRadius, data.angleDegrees, data.xMult, data.yMult, data.zMult), 0);
}

uniform int alphaType = LINEAR;
uniform int sizeType = FAST_END;
uniform int accelType = FAST_START;
uniform int colorType = LINEAR;

out vec4 vColor;
out float vScale;
out float vRotation;

uniform mat4 view;
uniform mat4 projection;
uniform float deltaTime;
uniform float totalTime;
uniform vec3 camPos;

uniform uint globalSeed; // Not used

void main()
{
    uint id = gl_VertexID;
    rng_seed(2341 * gl_VertexID);

    //Respawn
    if (totalTime >= particles[id].death) {
        float16_t totalTimeSF = float16_t(totalTime);

        particles[id].launch = totalTimeSF;
        particles[id].death = totalTimeSF + float16_t(rng_float(timeUBO.deathMin, timeUBO.deathMax));

        particles[id].vel = updateDirecAngleVec(velUBO);
        particles[id].pos.xyz = VectorFromRadius(posUBO.posRadiusMin, posUBO.posRadiusMax, posUBO.posOrigin.xyz);
        particles[id].grav = updateDirecAngleVec(gravUBO);

        particles[id].rot = float16_t(rng_float(spinUBO.rotMin, spinUBO.rotMax));
        particles[id].spin = float16_t(rng_float(spinUBO.spinMin, spinUBO.spinMax));

        particles[id].sizeData = updateGradient(sizeUBO);
        particles[id].accelData = updateGradient(accelUBO);
        particles[id].colorData = updateGradient(colorUBO);
        particles[id].alphaData = updateGradient(alphaUBO);
    }

    Particle p = particles[id];

    vec3 pos = p.pos.xyz;
    vec3 grav = p.grav.xyz;
    vec3 vel = p.vel.xyz;

    // Life Progress Calculation
    float lifeTime = totalTime - p.launch;
    float maxLifeTime = p.death - p.launch;
    float lifeProgress = lifeTime / maxLifeTime;

    // Rotation
    float rotRadians = float(p.rot) * TWO_PI;
    rotRadians += p.spin * totalTime; // Spin is radians per second
    rotRadians = mod(rotRadians, TWO_PI);
    vRotation = rotRadians;

    // Motion
    float curAccel;
    int accelsLength = accels.length();
    float accelGradientProg = getGradientProgress(p.accelData.start, p.accelData.end, p.accelData.lowLimit, p.accelData.upLimit, mod(lifeProgress, 1 / float(p.accelData.loop)), accelType);
    GradientData accelGradientData = getGradientData(accelsLength, accelGradientProg);
    if (accelsLength >= 2){
        float currentRot = mix(p.accelData.startRot, p.accelData.endRot, lifeProgress);
        float currentCos = abs(cos(currentRot * M_PI));
        float centerVal = (accelsLength % 2 == 0) ? accels[accelsLength / 2 -1] + accels[accelsLength / 2] / 2 : accels[int(floor(accelsLength /2.0))];
        centerVal *= (1.0 - currentCos);
        float firstAccel = (currentRot > 0.5) ? currentCos * accels[accelsLength - 1 - accelGradientData.first] : currentCos * accels[accelGradientData.first];
        float secondAccel = (currentRot > 0.5) ? currentCos * accels[accelsLength - 1 - accelGradientData.second] : currentCos * accels[accelGradientData.second];
        curAccel = mix(firstAccel + centerVal, secondAccel + centerVal, accelGradientData.actualProgress) * p.accelData.scale;
    }else curAccel = accels[0] * p.accelData.scale;

    vec3 posBonus = grav * deltaTime;
    posBonus += vel * deltaTime * curAccel;
    particles[id].pos.xyz += posBonus;
    gl_Position = vec4(p.pos.xyz, 1.0);

    // Size
    int sizesLength = sizes.length();
    float sizeGradientProg = getGradientProgress(p.sizeData.start, p.sizeData.end, p.sizeData.lowLimit, p.sizeData.upLimit, mod(lifeProgress, 1 / float(p.sizeData.loop)), sizeType);
    GradientData sizeGradientData = getGradientData(sizesLength, sizeGradientProg);
    if (sizesLength >= 2){
        float currentRot = mix(p.sizeData.startRot, p.sizeData.endRot, lifeProgress);
        float currentCos = abs(cos(currentRot * M_PI));
        float centerVal = (sizesLength % 2 == 0) ? sizes[sizesLength / 2 -1] + sizes[sizesLength / 2] / 2 : sizes[int(floor(sizesLength/2.0))];
        centerVal *= (1.0 - currentCos);
        float firstSize = (currentRot > 0.5) ? currentCos * sizes[sizesLength - 1 - sizeGradientData.first] : currentCos * sizes[sizeGradientData.first];
        float secondSize = (currentRot > 0.5) ? currentCos * sizes[sizesLength - 1 - sizeGradientData.second] : currentCos * sizes[sizeGradientData.second];
        vScale = mix(firstSize + centerVal, secondSize + centerVal, sizeGradientData.actualProgress) * p.sizeData.scale;
    }else vScale = sizes[0] * p.sizeData.scale;

    // Color
    float colorGradientProg = getGradientProgress(p.colorData.start, p.colorData.end, p.colorData.lowLimit, p.colorData.upLimit, mod(lifeProgress, 1 / float(p.colorData.loop)), colorType);
    GradientData colorGradientData = getGradientData(colors.length(), colorGradientProg);
    // Apply gradient rotation !SCALAR UNUSED!
    float gradientRotation = mix(p.colorData.startRot, p.colorData.endRot, lifeProgress);
    vec3 leftColor = rotateColor(colors[colorGradientData.first].rgb, gradientRotation * TWO_PI);
    vec3 rightColor = rotateColor(colors[colorGradientData.second].rgb, gradientRotation * TWO_PI);
    vec3 finalColor = mix(leftColor, rightColor, mod(colorGradientData.actualProgress, 1 / float(p.accelData.loop)));

    // Alpha
    float curAlpha;
    int alphaLength = alphas.length();
    float alphaGradientProg = getGradientProgress(p.alphaData.start, p.alphaData.end, p.alphaData.lowLimit, p.alphaData.upLimit, mod(lifeProgress, 1 / float(p.alphaData.loop)), alphaType);
    GradientData alphaGradientData = getGradientData(alphaLength, alphaGradientProg);
    if (alphaLength >= 2){
        float currentRot = mix(p.alphaData.startRot, p.alphaData.endRot, lifeProgress);
        float currentCos = abs(cos(currentRot * M_PI));
        float centerVal = (alphaLength % 2 == 0) ? alphas[alphaLength / 2 -1] + alphas[alphaLength / 2] / 2 : alphas[int(floor(alphaLength /2.0))];
        centerVal *= (1.0 - currentCos);
        float firstAlpha = (currentRot > 0.5) ? currentCos * alphas[alphaLength - 1 - alphaGradientData.first] : currentCos * alphas[alphaGradientData.first];
        float secondAlpha = (currentRot > 0.5) ? currentCos * alphas[alphaLength - 1 - alphaGradientData.second] : currentCos * alphas[alphaGradientData.second];
        curAlpha = mix(firstAlpha + centerVal, secondAlpha + centerVal, alphaGradientData.actualProgress) * p.alphaData.scale;
    }else curAlpha = alphas[0] * p.alphaData.scale;

    vColor = vec4(finalColor, curAlpha);
}