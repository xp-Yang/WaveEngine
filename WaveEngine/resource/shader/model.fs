#version 330 core

in GS_OUT{
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} fs_in;

struct Light {
    vec3 position;
    vec4 color;
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
#define LIGHT_COUNT 256
uniform Light lights[LIGHT_COUNT];
uniform vec3 view_pos;

uniform sampler2D shadow_map;
uniform samplerCube skybox;
uniform bool enable_skybox_sample;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 1.���ڲü��ռ䣬ִ��͸�ӳ������任��NDC�ռ�
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 2.�任��[0,1]�ķ�Χ, ���ڲ���
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;

    float bias = 0.005;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    // ��鵱ǰƬ���Ƿ�����Ӱ��
    float shadow = projCoords.z - closestDepth > bias ? 1.0 : 0.0;

    return shadow;
}

// blinn-phong
vec3 LightCalculation(Light light, vec3 n, vec3 v, vec3 l, vec3 diffuse_coef, vec3 specular_coef)
{
    vec3 diffuse_light = light.color.xyz * max(dot(l, n), 0.0) * diffuse_coef;
    
    vec3 h = normalize(v + l);
    vec3 specular_light = light.color.xyz * pow(max(dot(n, h), 0.0), 128.0) * specular_coef; // ����������������

    // phong
    //vec3 reflect_dir = reflect(-l, n);
    //vec3 specular_light = light.color.xyz * pow(max(dot(v, reflect_dir), 0.0), material.shininess) * specular_coef;

    return diffuse_light + specular_light;
}

void main()
{
    vec3 normal = normalize(fs_in.pass_normal);//TODO normal��Ҫ�任������ռ䣬��Ҫע�ⲻ�ܴ�ƽ��
    vec3 view_direction = normalize(view_pos - fs_in.pass_pos);
    vec3 diffuse_coef = vec3(texture(material.diffuse_map, fs_in.pass_uv));
    vec3 specular_coef = vec3(texture(material.specular_map, fs_in.pass_uv));

    // �������
    vec3 ambient_light = vec3(0);
    vec3 lighting = vec3(0);
    for(int i = 0; i < LIGHT_COUNT; i++){
        //ambient_light += lights[i].color.xyz * material.ambient * diffuse_coef;
        vec3 lightDir = normalize(lights[i].position - fs_in.pass_pos);
        lighting += LightCalculation(lights[i], normal, view_direction, lightDir, diffuse_coef, specular_coef);
    }

    // ������Ӱ
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    
    vec3 result = ambient_light + (1.0 - shadow) * lighting;
    gl_FragColor = vec4(result, 1.0);

    if(enable_skybox_sample){
        vec3 I = normalize(fs_in.pass_pos - view_pos);
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