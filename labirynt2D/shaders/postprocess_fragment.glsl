#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
uniform bool gameWon;
uniform float winTime;
uniform float animationStartTime;
uniform bool animationStarted;

void main()
{
    vec4 color = texture(screenTexture, TexCoords);
    
    if (!gameWon) {
        FragColor = color;
        return;
    }
    
    if (!animationStarted) {
        FragColor = color;
        return;
    }
    
    float animationTime = time - animationStartTime;
    
    if (animationTime < 2.0) {
        // Phase 1: Fade out everything to background color
        float fadeProgress = animationTime / 2.0;
        float fadeAmount = 1.0 - fadeProgress;
        FragColor = mix(vec4(0.1, 0.1, 0.1, 1.0), color, fadeAmount);
    }
}