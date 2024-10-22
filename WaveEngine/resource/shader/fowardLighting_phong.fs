#version 330 core

#include "common.h"
#include "BlinnPhong.h"

in VS_OUT {
    vec3 fragWorldPos;          //世界坐标
    vec3 fragWorldNormal;       //世界坐标
    vec2 fragUV;
} fs_in;

struct Material {
    float ambient;
    sampler2D diffuse_map;
    sampler2D specular_map;
    sampler2D normal_map;
    sampler2D height_map;

    vec3 diffuse;
    vec3 specular;
};

uniform Material material;

uniform vec3 cameraPos;

out vec4 outColor;

void main()
{
    vec3 normal = normalize(fs_in.fragWorldNormal);
    vec3 view_direction = normalize(cameraPos - fs_in.fragWorldPos);
    vec3 diffuse_coef = vec3(texture(material.diffuse_map, fs_in.fragUV));
    //if (diffuse_coef.xyz == vec3(1.0))
        diffuse_coef = material.diffuse;
    vec3 specular_coef = vec3(texture(material.specular_map, fs_in.fragUV));
    //if (specular_coef.xyz == vec3(1.0))
        specular_coef = material.specular;

    vec3 ambient_light = vec3(material.ambient);
	
    // Directional Light Source:
	vec3 lightDir = normalize(directionalLight.direction);
	vec3 lightingByDirectionalLight = BlinnPhong(directionalLight.color.xyz, normal, view_direction, -lightDir, diffuse_coef, specular_coef);
	
	// Point Light Source:
    vec3 lightingByPointLight = vec3(0);
    for(int i = 0; i < point_lights_size; i++){
        vec3 lightDir = normalize(fs_in.fragWorldPos - pointLights[i].position);
        lightingByPointLight += BlinnPhong(pointLights[i].color.xyz, normal, view_direction, -lightDir, diffuse_coef, specular_coef);
    }

	// Shadow:
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fs_in.fragWorldPos, 1.0);
    float shadowFactor = ShadowCalculation(fragPosLightSpace, shadow_map);       
    
    vec3 result = ambient_light + shadowFactor * lightingByDirectionalLight + lightingByPointLight;
    outColor = vec4(result, 1.0);
}