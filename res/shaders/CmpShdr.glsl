#version 450 core
#extension GL_NV_gpu_shader5 : enable

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

const int LINEAR = 0;
const int FAST_START = 1;
const int FAST_END = 2;

struct chunkA{
    float16_t launchTime;
    float16_t deathTime;

    float16_t startAccel;
    float16_t endAccel;

    float16_t startScale;
    float16_t endScale;

    uint16_t rotation;
    float16_t spin;
};

struct chunkB{
    vec3 velocity;
    float _empty;
};

struct chunkC{
    vec3 position;
    float _empty;
};

struct chunkD{
    vec3 gravity;
    float _empty;
};

struct chunkE{
    uint startColor;
    uint endColor;
};

layout(std430, binding = 0) buffer buffer0 {
    chunkA chunk0[];
};

layout(std430, binding = 1) buffer buffer1 {
    chunkB chunk1[];
};

layout(std430, binding = 2) buffer buffer2 {
    chunkC chunk2[];
};

layout(std430, binding = 3) buffer buffer3 {
    chunkD chunk3[];
};

layout(std430, binding = 4) buffer buffer4 {
    chunkE chunk4[];
};

layout(std430, binding = 5) buffer ResultScaleBuffer {
    float resultScales[];
};

layout(std430, binding = 6) buffer ResultRotationBuffer {
    float resultRotations[];
};

layout(std430, binding = 7) buffer ResultColorBuffer {
    vec4 resultColors[];
};


uniform int alphaType = FAST_END;
uniform int fadeType = LINEAR;
uniform int scaleType = FAST_END;
uniform int accelType = FAST_START;

float easeOutQuad(float t) {
    return 1.0 - (1.0 - t) * (1.0 - t);
}

float easeInQuad(float t) {
    return t * t;
}

float getProgression(int type, float lifeProgress){
    if (type == LINEAR) return lifeProgress; // Linear transition
    else if (type == FAST_START) return easeOutQuad(lifeProgress); // Ease out (faster at start)
    else if (type == FAST_END) return easeInQuad(lifeProgress); // Ease in (faster at end)
}

vec4 unpackColor(uint packedColor) {
    return vec4(float((packedColor >> 24) & 0xFF) / 255.0,
                float((packedColor >> 16) & 0xFF) / 255.0,
                float((packedColor >> 8) & 0xFF) / 255.0,
                float(packedColor & 0xFF) / 255.0);
}

uniform float deltaTime;
uniform float totalTime;

void main() {
    uint id = gl_GlobalInvocationID.x;
    if (id >= chunk0.length()) return;

    //Respawn
    if (totalTime >= chunk0[id].deathTime) {
        chunk2[id].position = vec3(0, 0, 0);
        float16_t totalTimeSF = float16_t(totalTime);
        chunk0[id].deathTime = totalTimeSF + 1.0;
        chunk0[id].launchTime = totalTimeSF;
    }

    // Life Progress Calculation
    float lifeTime = totalTime - chunk0[id].launchTime;
    float maxLifeTime = chunk0[id].deathTime - chunk0[id].launchTime;
    float lifeProgress = lifeTime / maxLifeTime;
    // Applying Calc Types
    float alphaProgress = getProgression(alphaType, lifeProgress);
    float fadeProgress = getProgression(fadeType, lifeProgress);
    float scaleProgress = getProgression(scaleType, lifeProgress);
    float accelProgress = getProgression(accelType, lifeProgress);

    // Motion
    vec3 positionBonus = chunk3[id].gravity * deltaTime;
    positionBonus += chunk1[id].velocity * deltaTime * mix(chunk0[id].startAccel, chunk0[id].endAccel, accelProgress);
    chunk2[id].position += positionBonus;

    //Scale
    resultScales[id] = mix(chunk0[id].startScale, chunk0[id].endScale, scaleProgress);

    // Convert rotation from unsigned short to radians
    float rotationRadians = (float(chunk0[id].rotation) / 65535.0) * 2.0 * 3.14159;
    rotationRadians += chunk0[id].spin * mod(totalTime,1); // Spin already in radians per second
    rotationRadians = mod(rotationRadians, 2.0 * 3.14159);
    resultRotations[id] = rotationRadians;

    vec4 startColorUnpacked = unpackColor(chunk4[id].startColor);
    vec4 endColorUnpacked = unpackColor(chunk4[id].endColor);

    resultColors[id] = vec4(
        mix(startColorUnpacked.r, endColorUnpacked.r, fadeProgress),
        mix(startColorUnpacked.g, endColorUnpacked.g, fadeProgress),
        mix(startColorUnpacked.b, endColorUnpacked.b, fadeProgress),
        mix(startColorUnpacked.a, endColorUnpacked.a, alphaProgress)
    );

}