#version 450 core
layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform float deltaTime;
uniform float totalTime;

void main() {
    uint gid = gl_GlobalInvocationID.x;
    if (gid >= deathtimes.length()) return;


}