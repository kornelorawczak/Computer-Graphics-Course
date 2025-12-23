#version 330 core
out vec4 FragColor;

uniform vec3 u_Color;

void main()
{
    // Simple solid color for minimap objects
    FragColor = vec4(u_Color, 1.0);
}