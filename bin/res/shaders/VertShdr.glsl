#version 450 core
#extension GL_NV_gpu_shader5 : enable

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

out vec4 vColor;
out float vScale;
out float vRotation;

uniform mat4 view;
uniform mat4 projection;
uniform float deltaTime;
uniform float totalTime;
uniform vec3 camPos;

const vec2 quadVertices[4] = vec2[4](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2(-0.5,  0.5),
    vec2( 0.5,  0.5)
);

void main()
{
    uint particleId = gl_VertexID / 4;
    uint cornerIdx = gl_VertexID % 4;

    //Respawn
    if (totalTime >= chunk0[particleId].deathTime) {
        chunk2[particleId].position = vec3(0, 0, 0);
        chunk0[particleId].deathTime = totalTime + 1.0;
        chunk0[particleId].launchTime = totalTime;
    }

    // Life Progress Calculation
    float lifeTime = totalTime - chunk0[particleId].launchTime;
    float maxLifeTime = chunk0[particleId].deathTime - chunk0[particleId].launchTime;
    float lifeProgress = lifeTime / maxLifeTime;

    // Applying Calc Types
    float alphaProgress = getProgression(alphaType, lifeProgress);
    float fadeProgress = getProgression(fadeType, lifeProgress);
    float scaleProgress = getProgression(scaleType, lifeProgress);
    float accelProgress = getProgression(accelType, lifeProgress);

    // Motion
    vec3 positionBonus = chunk3[particleId].gravity * deltaTime;
    positionBonus += chunk1[particleId].velocity * deltaTime * mix(chunk0[particleId].startAccel, chunk0[particleId].endAccel, accelProgress);
    chunk2[particleId].position += positionBonus;

    //Scale
    float scale = mix(chunk0[particleId].startScale, chunk0[particleId].endScale, scaleProgress);

    // Rotation
    float rotationRadians = (float(chunk0[particleId].rotation) / 65535.0) * 2.0 * 3.14159;
    rotationRadians += chunk0[particleId].spin * mod(totalTime, 1);
    mat2 rotMat = mat2(cos(rotationRadians), -sin(rotationRadians),
                       sin(rotationRadians), cos(rotationRadians));

    // Apply transformation to quad vertex
    vec2 rotatedPos = rotMat * (quadVertices[cornerIdx] * scale);
    vec3 finalPosition = chunk2[particleId].position + vec3(rotatedPos, 0.0);

    gl_Position = projection * view * vec4(finalPosition, 1.0);

    // Color calculation
    vec4 startColorUnpacked = unpackColor(chunk4[particleId].startColor);
    vec4 endColorUnpacked = unpackColor(chunk4[particleId].endColor);

    vColor = vec4(
        mix(startColorUnpacked.r, endColorUnpacked.r, fadeProgress),
        mix(startColorUnpacked.g, endColorUnpacked.g, fadeProgress),
        mix(startColorUnpacked.b, endColorUnpacked.b, fadeProgress),
        mix(startColorUnpacked.a, endColorUnpacked.a, alphaProgress)
    );
}
