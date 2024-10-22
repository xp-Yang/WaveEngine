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
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;

uniform mat4 view;

const vec3 LIGHT_TOP_FRONT_DIR = vec3(0.0, -0.7624929, -0.4574957);

out vec4 outColor;

void main()
{
    vec3 normal = mat3(view) * normalize(fs_in.fragWorldNormal);
    vec3 view_direction = normalize(-mat3(view) * fs_in.fragWorldPos);
    vec3 diffuse_coef = material.diffuse;
    vec3 specular_coef = material.specular;
    vec3 ambient_light = vec3(material.ambient);
	
    // Directional Light Source:
	vec3 lightDir = normalize(LIGHT_TOP_FRONT_DIR);
	vec3 lightingByDirectionalLight = BlinnPhong(vec3(1.0), normal, view_direction, -lightDir, diffuse_coef, specular_coef);

    vec3 result = ambient_light + lightingByDirectionalLight;
    outColor = vec4(result, 1.0);
}