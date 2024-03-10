#version 330 core

#include "common.h"
#include "BRDF.h"

in VS_OUT {
    vec3 fragWorldPos;          //世界坐标
    vec3 fragWorldNormal;       //局部坐标
    vec2 fragUV;
} fs_in;

uniform DirectionalLight directionalLight;
const int MAX_POINT_LIGHTS_COUNT = 128;
uniform int point_lights_size;
uniform PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

// TODO should be map
// struct Material {
// };
// uniform Material material;
// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 cameraPos;

out vec4 FragColor;

void main()
{		
    vec3 N = normalize(fs_in.fragWorldNormal);
    vec3 V = normalize(cameraPos - fs_in.fragWorldPos);

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
        vec3 L = normalize(pointLights[i].position - fs_in.fragWorldPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - fs_in.fragWorldPos);
        float k_quadratic = 0.2 / pointLights[i].radius;
		float attenuation = step(0, (pointLights[i].radius - distance)) * (1.0 / (1.0 + k_quadratic * distance* distance));
        vec3 radiance = pointLights[i].color.xyz * attenuation;  

        // add to outgoing radiance Lo
        Lo += radiance * BRDF(L, V, N, F0, radiance, metallic, roughness);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = 0.03 * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
