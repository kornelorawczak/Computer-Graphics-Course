#version 330 core
out vec4 FragColor;

in vec2 FragCoord;

uniform float progress; 
uniform vec2 resolution;

void main() {
    // Convert from [-1,1] to [0,1] range
    vec2 uv = (FragCoord + 1.0) * 0.5;
    
    // Calculate distance from center with aspect ratio correction
    vec2 center = vec2(0.5, 0.5);
    vec2 dir = uv - center;
    dir.x *= resolution.x / resolution.y; 
    float dist = length(dir);
    
    // Create radial white 
    float whiteCircle = smoothstep(progress - 0.1, progress + 0.1, dist);
    float alpha = 1.0 - whiteCircle;
    vec3 white = vec3(1.0);
    
    FragColor = vec4(white, alpha * progress);
}