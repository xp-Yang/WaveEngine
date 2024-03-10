#version 330 core

in vec2 fragUV;

#include "common.h"
#include "BlinnPhong.h"

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

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
    float shadow = ShadowCalculation(fragPosLightSpace, shadow_map);

    gl_FragColor = vec4((1.0 - shadow) * lightingByDirectionalLight + lightingByPointLight, 1.0);
}