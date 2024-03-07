#version 330 core

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

uniform sampler2D shadow_map;

uniform mat4 lightSpaceMatrix;

struct DirectionalLight
{
    vec3 direction;
    vec4 color;
};

struct PointLight
{
    vec3  position;
	vec4  color;
    float radius;
};

uniform DirectionalLight directionalLight;
// TODO shader里怎么用动态数组
const int MAX_POINT_LIGHTS_COUNT = 16;
uniform int point_lights_size = 5;
uniform PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

uniform vec3 view_pos;


float ShadowCalculation(vec4 LightSpacePos)
{
    // 1.还在裁剪空间，执行透视除法，变换到NDC空间
    vec3 projCoords = LightSpacePos.xyz / LightSpacePos.w;
    // 2.变换到[0,1]的范围, 便于采样和比较
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;

    float bias = 0.005;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    // 检查当前片段是否在阴影中
    float shadow = projCoords.z - closestDepth > bias ? 1.0 : 0.0;

    return shadow;
}


// blinn-phong
vec3 LightCalculation(vec3 light_color, vec3 n, vec3 v, vec3 l, vec3 diffuse_coef, vec3 specular_coef)
{
    vec3 diffuse_light = light_color * max(dot(l, n), 0.0) * diffuse_coef;
    
    vec3 h = normalize(v + l);
    vec3 specular_light = light_color * pow(max(dot(n, h), 0.0), 128.0) * specular_coef;

    // phong
    //vec3 reflect_dir = reflect(-l, n);
    //vec3 specular_light = light_color * pow(max(dot(v, reflect_dir), 0.0), 128.0) * specular_coef;

    return diffuse_light + specular_light;
}

void main()
{             
    // retrieve data from gbuffer
    vec3 Position = texture(gPosition, uv).rgb;
    vec3 Normal = texture(gNormal, uv).rgb;
    vec3 Diffuse = texture(gDiffuse, uv).rgb;
    vec3 Specular = texture(gSpecular, uv).rgb;

    vec3 viewDir = normalize(view_pos - Position);

    // TODO 如果采样到GBuffer的空白区域可以直接return

    // Directional Light Source:
	vec3 lightDir = directionalLight.direction;
	vec3 lightingByDirectionalLight = LightCalculation(directionalLight.color.xyz, Normal, viewDir, -lightDir, Diffuse, Specular);
	
	// Point Light Source:
    vec3 lightingByPointLight = vec3(0);
	float k_quadratic = 0.5;
    for(int i = 0; i < point_lights_size; i++){
        vec3 lightDir = normalize(Position - pointLights[i].position);
		float distance = length(Position - pointLights[i].position);
		float attenuation = min((1.0 / ( 1.0 + k_quadratic * distance* distance )) * ( 1.0 - distance * distance / pointLights[i].radius * pointLights[i].radius), 0);
        lightingByPointLight += LightCalculation(pointLights[i].color.xyz * attenuation, Normal, viewDir, -lightDir, Diffuse, Specular);
    }

	// Shadow:
    vec4 LightSpacePos = lightSpaceMatrix * vec4(Position, 1.0);
    float shadow = ShadowCalculation(LightSpacePos);

    gl_FragColor = vec4((1.0 - shadow) * lightingByDirectionalLight + lightingByPointLight, 1.0);
}