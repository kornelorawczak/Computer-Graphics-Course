#version 330 core
out vec4 FragColor;

in vec2 FragCoord;

uniform float progress; 
uniform vec2 resolution;
uniform vec3 color;

void main() {
    vec2 uv = (FragCoord + 1.0) * 0.5;
    
    vec2 center = vec2(0.5, 0.5);
    vec2 dir = uv - center;
    dir.x *= resolution.x / resolution.y; 
    float dist = length(dir);
    
    float circle = smoothstep(progress - 0.1, progress + 0.1, dist);
    float alpha = 1.0 - circle;
    
    FragColor = vec4(color, alpha * progress);
}