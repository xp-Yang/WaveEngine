#version 330 core

#include "BlinnPhong.h"

layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;

uniform mat4 model;
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
const float LIGHT_TOP_CONTRIBUTION = 0.5;
const float LIGHT_FRONT_CONTRIBUTION = 0.25;
const float EMISSIVE_CONTRIBUTION = 0.25;

out float vertexID;
out vec3 worldPos;
out vec3 gouraudColor;
out vec3 averageColor;

void main()
{
    vec3 vertexViewPos = vec3(view * model * vec4(vertex_pos, 1.0));
    vec3 vertexViewNormal = normalize(mat3(view) * mat3(model) * vertex_normal);
    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);

    worldPos = (model * vec4(vertex_pos, 1.0)).xyz;

    vec3 normal = vertexViewNormal;
    vec3 view_direction = -vertexViewPos;
    vec3 diffuse_coef = material.diffuse;
    vec3 specular_coef = material.specular;
    vec3 ambient_light = vec3(material.ambient);
	
    // Directional Light Source:
	vec3 lightDir = normalize(LIGHT_TOP_DIR);
	vec3 lightingByDirectionalLight = LIGHT_TOP_CONTRIBUTION * BlinnPhong(vec3(1.0), normal, view_direction, -lightDir, diffuse_coef, specular_coef);

    lightDir = normalize(LIGHT_FRONT_DIR);
	lightingByDirectionalLight += LIGHT_FRONT_CONTRIBUTION * BlinnPhong(vec3(1.0), normal, view_direction, -lightDir, diffuse_coef, specular_coef);

    lightingByDirectionalLight += EMISSIVE_CONTRIBUTION * diffuse_coef;

    vec3 result = ambient_light + lightingByDirectionalLight;
    gouraudColor = result;

    averageColor = LIGHT_TOP_CONTRIBUTION * diffuse_coef / 2.0 + 
        LIGHT_FRONT_CONTRIBUTION * diffuse_coef / 2.0 + 0.1 * diffuse_coef + ambient_light;

    vertexID = float(gl_VertexID);
}