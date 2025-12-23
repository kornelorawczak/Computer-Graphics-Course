#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    
    vec3 flattenedPos = vec3(worldPos.x, 0.0, worldPos.z);
    
    gl_Position = u_Proj * u_View * vec4(flattenedPos, 1.0);
}