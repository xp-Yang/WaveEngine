#version 330 core

#include "common.h"
#include "BlinnPhong.h"

in VS_OUT {
    vec3 fragWorldPos;          //��������
    vec3 fragWorldNormal;       //��������
    vec2 fragUV;
} fs_in;

struct Material {
    float ambient;
    sampler2D diffuse_map;
    sampler2D specular_map;
    sampler2D normal_map;
    sampler2D height_map;
};

uniform Material material;

uniform vec3 cameraPos;

uniform samplerCube skybox;
uniform bool enable_skybox_sample;

void main()
{
    vec3 normal = normalize(fs_in.fragWorldNormal);//TODO normal��Ҫ�任������ռ䣬��Ҫע�ⲻ�ܴ�ƽ��?
    vec3 view_direction = normalize(cameraPos - fs_in.fragWorldPos);
    vec3 diffuse_coef = vec3(texture(material.diffuse_map, fs_in.fragUV));
    vec3 specular_coef = vec3(texture(material.specular_map, fs_in.fragUV));

    vec3 ambient_light = vec3(0);
	
    // Directional Light Source:
	vec3 lightDir = directionalLight.direction;
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
    gl_FragColor = vec4(result, 1.0);

    if(enable_skybox_sample){
        vec3 I = normalize(fs_in.fragWorldPos - cameraPos);
        vec3 R = reflect(I, normalize(normal));
        gl_FragColor = 0.33 * gl_FragColor + 0.66 * vec4(texture(skybox, R).rgb, 1.0);
    }

    //debug
    //gl_FragColor = vec4((ambient_light + diffuse_light + specular_light), 1.0);
}