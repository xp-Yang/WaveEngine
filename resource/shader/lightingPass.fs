#version 330 core

in vec2 uv;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

struct Light {
    vec3 position;
    vec4 color;
    
    //float Linear;
    //float Quadratic;
};
const int LIGHT_COUNT = 256;
uniform Light lights[LIGHT_COUNT];
uniform vec3 view_pos;

// blinn-phong
vec3 LightCalculation(Light light, vec3 n, vec3 v, vec3 l, vec3 diffuse_coef, vec3 specular_coef)
{
    vec3 diffuse_light = light.color.xyz * max(dot(l, n), 0.0) * diffuse_coef;
    
    vec3 h = normalize(v + l);
    vec3 specular_light = light.color.xyz * pow(max(dot(n, h), 0.0), 128.0) * specular_coef;

    // phong
    //vec3 reflect_dir = reflect(-l, n);
    //vec3 specular_light = light.color.xyz * pow(max(dot(v, reflect_dir), 0.0), 128.0) * specular_coef;

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

    // º∆À„π‚’’
    //vec3 ambient_light = vec3(0);
    vec3 lighting = vec3(0);
    for(int i = 0; i < LIGHT_COUNT; i++){
        //ambient_light += lights[i].color.xyz * material.ambient * Diffuse);
        vec3 lightDir = normalize(lights[i].position - Position);
        lighting += LightCalculation(lights[i], Normal, viewDir, lightDir, Diffuse, Specular);
    }
    gl_FragColor = vec4(lighting, 1.0);
}