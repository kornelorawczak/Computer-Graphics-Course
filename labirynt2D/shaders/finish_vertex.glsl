#version 330 core
layout (location = 0) in vec3 aPos;

uniform float time;

void main() {
    // Rotate around the triangle's center
    float rotationSpeed = 0.45;
    float angle = time * rotationSpeed;
    float sin = sin(angle);
    float cos = cos(angle);

    // Rotate around center
    vec2 pos = vec2(aPos.x * cos - aPos.y * sin, aPos.x * sin + aPos.y * cos);
    pos += vec2(0.9, 0.9);

    gl_Position = vec4(pos, aPos.z, 1.0);
}