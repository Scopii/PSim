#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vColor[];
in float vScale[];
in float vRotation[];
out vec4 fragColor;

uniform mat4 view;
uniform mat4 projection;

// Define quad corners
const vec2 corners[4] = vec2[4](
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5)
);

void main() {
    vec3 position = gl_in[0].gl_Position.xyz;
    float particleSize = vScale[0];
    float rotation = vRotation[0];

    // Camera right and up vectors (for billboarding)
    vec3 cameraRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 cameraUp = vec3(view[0][1], view[1][1], view[2][1]);

    // Create rotation matrix
    mat2 rotMat = mat2(
        cos(rotation), -sin(rotation),
        sin(rotation), cos(rotation)
    );

    // Emit vertices
    for (int i = 0; i < 4; i++) {
        // Rotate the corner
        vec2 rotatedCorner = rotMat * corners[i];

        // Apply billboarding and scale
        vec3 offset = (cameraRight * rotatedCorner.x + cameraUp * rotatedCorner.y) * particleSize;
        vec3 worldPos = position + offset;

        gl_Position = projection * view * vec4(worldPos, 1.0);
        fragColor = vColor[0];
        EmitVertex();
    }

    EndPrimitive();
}