#version 330 core

#include "BlinnPhong.h"

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in mat4 instance_matrix;

uniform mat4 view;
uniform mat4 projection;

struct Material {
    float ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material material;

const vec3 LIGHT_TOP_DIR = vec3(0.4574957, -0.7624929, 0.4574957);
const vec3 LIGHT_FRONT_DIR = vec3(-0.6985074, -0.6985074, 0.1397015);

out vec4 gouraudColor;

void main()
{
    vec3 fragViewPos = vec3(view * instance_matrix * vec4(vertex_pos, 1.0));
    vec3 fragWorldNormal = normalize(mat3(view) * mat3(instance_matrix) * vertex_normal);
    gl_Position = projection * view * instance_matrix * vec4(vertex_pos, 1.0);

    vec3 normal = fragWorldNormal;
    vec3 view_direction = -fragViewPos;
    vec3 diffuse_coef = material.diffuse;
    vec3 specular_coef = material.specular;
    vec3 ambient_light = vec3(material.ambient);
	
    // Directional Light Source:
	vec3 lightDir = normalize(LIGHT_TOP_DIR);
	vec3 lightingByDirectionalLight = 0.5 * BlinnPhong(vec3(1.0), normal, view_direction, -lightDir, diffuse_coef, specular_coef);

    lightDir = normalize(LIGHT_FRONT_DIR);
	lightingByDirectionalLight += 0.25 * BlinnPhong(vec3(1.0), normal, view_direction, -lightDir, diffuse_coef, specular_coef);

    lightingByDirectionalLight += 0.25 * diffuse_coef;

    vec3 result = ambient_light + lightingByDirectionalLight;
    gouraudColor = vec4(result, 1.0);
}