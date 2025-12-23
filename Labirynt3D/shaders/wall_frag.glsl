#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;  

uniform sampler2D wallTexture;  

void main() {
    vec4 texColor = texture(wallTexture, TexCoords * 5.0);  
    
    vec3 gridPos = fract(FragPos * 10.0);
    vec3 grid = step(0.95, gridPos) + step(0.95, 1.0 - gridPos);
    float gridLine = max(max(grid.x, grid.y), grid.z);
    
    vec3 baseColor = texColor.rgb * 0.7;  
    vec3 gridColor = vec3(0.8, 0.8, 0.8);
    
    vec3 finalColor = mix(baseColor, gridColor, gridLine * 0.5);
    
    FragColor = vec4(finalColor * 0.7, 1.0);
}