#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 InstanceColor;
in float InstanceAlpha;

uniform vec3 u_LightDir;
uniform vec3 u_ViewPos;
uniform vec3 u_LightColor;
uniform float u_Shininess;
uniform float u_SpecularStrength;

uniform vec3 u_FogColor;
uniform float u_FogDensity;

uniform bool u_UseTransparency;

uniform bool u_UseFlashlight;
uniform vec3 u_FlashlightPos;
uniform vec3 u_FlashlightDir;
uniform float u_FlashlightCutoff;
uniform float u_FlashlightOuterCutoff;
uniform vec3 u_FlashlightColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-u_LightDir);
    
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * u_LightColor;
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_LightColor;
    
    vec3 viewDir = normalize(u_ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    float fresnel = pow(1.0 - max(dot(norm, viewDir), 0.0), 2.0);
    vec3 specular = (u_SpecularStrength * spec + fresnel * 0.3) * u_LightColor;

    vec3 flashlight = vec3(0.0);
    vec3 flashlightDir = normalize(FragPos - u_FlashlightPos);
    float theta = dot(flashlightDir, normalize(-u_FlashlightDir));
    float epsilon = u_FlashlightCutoff - u_FlashlightOuterCutoff;
    float intensity = clamp((theta - u_FlashlightOuterCutoff) / epsilon, 0.0, 1.0);
    
    if (theta > u_FlashlightOuterCutoff) {
        float distance = length(u_FlashlightPos - FragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
        
        vec3 flashDir = normalize(-u_FlashlightDir);
        float flashDiff = max(dot(norm, flashDir), 0.0);
        vec3 flashDiffuse = flashDiff * u_FlashlightColor;
        
        vec3 flashReflectDir = reflect(-flashDir, norm);
        float flashSpec = pow(max(dot(viewDir, flashReflectDir), 0.0), u_Shininess);
        vec3 flashSpecular = u_SpecularStrength * flashSpec * u_FlashlightColor;
        
        flashlight = (flashDiffuse + flashSpecular) * intensity * attenuation;
    }
    
    
    vec3 result = (ambient + diffuse + specular +  flashlight) * InstanceColor;
    
    float distance = length(u_ViewPos - FragPos);
    float fogFactor = 1.0 - exp(-u_FogDensity * distance * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 sceneColor = mix(vec4(result, InstanceAlpha), vec4(u_FogColor, InstanceAlpha), fogFactor);
    
    if (u_UseTransparency) {
        FragColor = sceneColor;
    } else {
        FragColor = vec4(sceneColor.rgb, 1.0);
    }
}