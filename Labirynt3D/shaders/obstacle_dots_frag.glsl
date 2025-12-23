#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;

void main() {
    vec3 patternPos = fract(FragPos * 120.0); // Scale factor for dot density
    float dotSize = 0.1;
    
    // Calculate distance to center of each "cell" in the pattern
    vec3 distToCenter = abs(patternPos - 0.5);
    float maxDist = max(max(distToCenter.x, distToCenter.y), distToCenter.z);
    
    // Create dots - if we're close to center of any cell, make it black
    float dotPattern = 1.0 - step(dotSize, maxDist);
    
    // Mix between object color and black based on dot pattern
    vec3 baseColor = objectColor;
    vec3 dotColor = mix(baseColor, vec3(0.0, 0.0, 0.0), dotPattern);
    
    FragColor = vec4(dotColor, 1.0);
}