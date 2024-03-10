#version 330 core

in vec2 fragUV;

#include "common.h"
#include "BlinnPhong.h"
#include "BRDF.h"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;
// pbr
uniform sampler2D gAlbedo;
uniform sampler2D gMetallic;
uniform sampler2D gRoughness;
uniform sampler2D gAo;

uniform bool enablePBR;

uniform sampler2D shadow_map;
uniform mat4 lightSpaceMatrix;

uniform DirectionalLight directionalLight;
// TODO shader里怎么用动态数组
const int MAX_POINT_LIGHTS_COUNT = 256;
uniform int point_lights_size;
uniform PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

uniform vec3 cameraPos;

void main()
{             
    // retrieve data from gbuffer
    vec3 Position = texture(gPosition, fragUV).rgb;
    vec3 Normal = texture(gNormal, fragUV).rgb;
    vec3 Diffuse = texture(gDiffuse, fragUV).rgb;
    vec3 Specular = texture(gSpecular, fragUV).rgb;

if(!enablePBR){
    vec3 viewDir = normalize(cameraPos - Position);

    // TODO 如果采样到GBuffer的空白区域可以直接return

    // Directional Light Source:
	vec3 lightDir = directionalLight.direction;
	vec3 lightingByDirectionalLight = BlinnPhong(directionalLight.color.xyz, Normal, viewDir, -lightDir, Diffuse, Specular);
	
	// Point Light Source:
    vec3 lightingByPointLight = vec3(0);
    for(int i = 0; i < point_lights_size; i++){
        vec3 lightDir = normalize(Position - pointLights[i].position);
		float distance = length(Position - pointLights[i].position);
	    float k_quadratic = 0.2 / pointLights[i].radius;
		float attenuation = step(0, (pointLights[i].radius - distance)) * (1.0 / (1.0 + k_quadratic * distance* distance));
        lightingByPointLight += BlinnPhong(pointLights[i].color.xyz * attenuation, Normal, viewDir, -lightDir, Diffuse, Specular);
    }

	// Shadow:
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);
    float shadowFactor = ShadowCalculation(fragPosLightSpace, shadow_map);

    gl_FragColor = vec4(shadowFactor * lightingByDirectionalLight + lightingByPointLight, 1.0);
}
else{
    vec3 albedo = Diffuse;
    float metallic = texture(gMetallic, fragUV).r;
    float roughness = texture(gRoughness, fragUV).r;
    float ao = texture(gAo, fragUV).r;

    vec3 N = normalize(Normal);
    vec3 V = normalize(cameraPos - Position);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
		
		// Directional Light
        vec3 L = normalize(-directionalLight.direction);
        vec3 H = normalize(V + L);
        vec3 radiance = directionalLight.color.xyz;  
        Lo += radiance * BRDF(L, V, N, F0, radiance, metallic, roughness);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again

    for(int i = 0; i < point_lights_size; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - Position);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - Position);
        float k_quadratic = 0.2 / pointLights[i].radius;
		float attenuation = step(0, (pointLights[i].radius - distance)) * (1.0 / (1.0 + k_quadratic * distance* distance));
        vec3 radiance = pointLights[i].color.xyz * attenuation;  

        // add to outgoing radiance Lo
        Lo += radiance * BRDF(L, V, N, F0, radiance, metallic, roughness);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = 0.3 * albedo * ao;

	// Shadow:
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(Position, 1.0);
    float shadowFactor = ShadowCalculation(fragPosLightSpace, shadow_map);

    vec3 color = ambient + shadowFactor * Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    gl_FragColor = vec4(color, 1.0);
}
}