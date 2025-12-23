#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (location = 2) in vec3 aInstancePos;
layout (location = 3) in float aInstanceScale;
layout (location = 4) in vec3 aInstanceColor;
layout (location = 5) in float aInstanceAlpha;

out vec3 FragPos;
out vec3 Normal;
out vec3 InstanceColor;
out float InstanceAlpha;

uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    vec3 worldPos = aInstancePos + aPos * aInstanceScale;
    FragPos = worldPos;
    Normal = aNormal; 
    InstanceColor = aInstanceColor;
    InstanceAlpha = aInstanceAlpha;
    
    gl_Position = u_Proj * u_View * vec4(worldPos, 1.0);
}