#version 330 core

#include "common.h"
#include "BlinnPhong.h"

in vec2 fragUV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

uniform vec3 cameraPos;

layout (location = 0) out vec4 FragColor;

void main()
{             
    // retrieve data from gbuffer
    vec3 Position = texture(gPosition, fragUV).rgb;
    vec3 Normal = texture(gNormal, fragUV).rgb;
    vec3 Diffuse = texture(gDiffuse, fragUV).rgb;
    vec3 Specular = texture(gSpecular, fragUV).rgb;

    if (Normal.xyz == vec3(0.0)){
        // return if sample the blank area in GBuffer
        FragColor = vec4(0.046, 0.046, 0.046, 1.0);
        return;
    }
    
    vec3 viewDir = normalize(cameraPos - Position);

    // Directional Light Source:
    vec3 lightDir = directionalLight.direction;
    vec3 lightingByDirectionalLight = BlinnPhong(directionalLight.color.xyz * 0.0157, Normal, viewDir, -lightDir, Diffuse, Specular);
    // Directional Light Shadow:
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);
    float shadowFactor = ShadowCalculation(fragPosLightSpace, shadow_map);
    lightingByDirectionalLight *= shadowFactor;

    // Point Light Source:
    vec3 lightingByPointLight = vec3(0);
    vec3 lightingByPointLights[MAX_POINT_LIGHTS_COUNT];
    for(int i = 0; i < point_lights_size; i++){
        vec3 lightDir = normalize(Position - pointLights[i].position);
        float distance = length(Position - pointLights[i].position);
        float attenuation = PointLightAttenuation(distance, pointLights[i].radius);
        lightingByPointLights[i] = BlinnPhong(pointLights[i].color.xyz * attenuation, Normal, viewDir, -lightDir, Diffuse, Specular);
        // Point Light Shadow:
        float pointShadowFactor = OmnidirectionalShadowCalculation(Position - pointLights[i].position, cube_shadow_maps[i], pointLights[i].radius);
        lightingByPointLights[i] *= pointShadowFactor;
        lightingByPointLight += lightingByPointLights[i];
    }

    FragColor = vec4(lightingByDirectionalLight + lightingByPointLight, 1.0);
}