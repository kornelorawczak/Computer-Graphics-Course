#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;

void main() {
    FragColor = vec4(objectColor * 0.8, 1.0);
}