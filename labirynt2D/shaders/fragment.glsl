#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

uniform float time;

void main()
{ 
    // Computes the position of color in the diagonal direction
    float diagonal = (gl_FragCoord.x + gl_FragCoord.y) * 0.007 - time;

    // sin wave between 0 and 1
    float wave = 0.5 + 0.5 * sin(diagonal);

    vec3 finalColor = vertexColor + vec3(wave) * 0.4;
    finalColor = clamp(finalColor, 0.0, 0.5);
    FragColor = vec4(finalColor, 1.0);

}
