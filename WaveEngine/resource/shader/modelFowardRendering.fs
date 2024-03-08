#version 330 core

in VS_OUT {
    mat4 vp;
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} fs_in;

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
    vec3  intensity;
};

struct Material {
    float ambient;
    sampler2D diffuse_map;
    sampler2D specular_map;
    sampler2D normal_map;
    sampler2D height_map;
    float shininess;
};

uniform Material material;

uniform DirectionalLight directionalLight;
const int MAX_POINT_LIGHTS_COUNT = 128;
uniform int point_lights_size;
uniform PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

uniform vec3 cameraPos;

uniform sampler2D shadow_map;
uniform samplerCube skybox;
uniform bool enable_skybox_sample;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 1.还在裁剪空间，执行透视除法，变换到NDC空间
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 2.变换到[0,1]的范围, 便于采样
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
    vec3 specular_light = light_color * pow(max(dot(n, h), 0.0), 128.0) * specular_coef; // TODO 光线入射能量不对

    // phong
    //vec3 reflect_dir = reflect(-l, n);
    //vec3 specular_light = light_color * pow(max(dot(v, reflect_dir), 0.0), material.shininess) * specular_coef;

    return diffuse_light + specular_light;
}

void main()
{
    vec3 normal = normalize(fs_in.pass_normal);//TODO normal需要变换成世界空间，但要注意不能带平移
    vec3 view_direction = normalize(cameraPos - fs_in.pass_pos);
    vec3 diffuse_coef = vec3(texture(material.diffuse_map, fs_in.pass_uv));
    vec3 specular_coef = vec3(texture(material.specular_map, fs_in.pass_uv));

    vec3 ambient_light = vec3(0);
	
    // Directional Light Source:
	vec3 lightDir = directionalLight.direction;
	vec3 lightingByDirectionalLight = LightCalculation(directionalLight.color.xyz, normal, view_direction, -lightDir, diffuse_coef, specular_coef);
	
	// Point Light Source:
    vec3 lightingByPointLight = vec3(0);
    for(int i = 0; i < point_lights_size; i++){
        vec3 lightDir = normalize(fs_in.pass_pos - pointLights[i].position);
        lightingByPointLight += LightCalculation(pointLights[i].color.xyz, normal, view_direction, -lightDir, diffuse_coef, specular_coef) / point_lights_size;
    }

	// Shadow:
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    
    vec3 result = ambient_light + (1.0 - shadow) * lightingByDirectionalLight + lightingByPointLight;
    gl_FragColor = vec4(result, 1.0);

    if(enable_skybox_sample){
        vec3 I = normalize(fs_in.pass_pos - cameraPos);
        vec3 R = reflect(I, normalize(normal));
        gl_FragColor = 0.33 * gl_FragColor + 0.66 * vec4(texture(skybox, R).rgb, 1.0);
    }

    //debug
    //gl_FragColor = vec4((ambient_light + diffuse_light + specular_light), 1.0);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(fs_in.pass_uv, 0.0, 1.0);
    //gl_FragColor = vec4(vec3(shadow), 1.0);
    //gl_FragColor = vec4(normal, 1.0);
    //gl_FragColor = vec4(abs(spec_coef),abs(spec_coef),abs(spec_coef), 1.0f);
    //gl_FragColor = vec4(abs(view_direction), 1.0f);
}