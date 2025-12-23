#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;

void main() {
    vec3 goldColor = vec3(1.0, 0.84, 0.0); 
    FragColor = vec4(goldColor, 1.0);
}