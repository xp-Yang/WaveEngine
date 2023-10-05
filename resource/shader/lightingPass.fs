#version 330 core
out vec4 FragColor;

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

vec3 LightCalculation(Light light, vec3 normal, vec3 view_dir, vec3 position, vec3 diffuse, vec3 specular)
{
    vec3 incidence_dir = normalize(light.position - position);

    float diff_coef = max(dot(incidence_dir, normal), 0.0);
    vec3 diffuse_light = light.color.xyz * diff_coef * diffuse;
    
    vec3 reflect_dir = reflect(-incidence_dir, normal);
    float spec_coef = pow(max(dot(view_dir, reflect_dir), 0.001), 16.0);
    vec3 specular_light = light.color.xyz * spec_coef * specular;

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
        lighting += LightCalculation(lights[i], Normal, viewDir, Position, Diffuse, Specular);
    }
    FragColor = vec4(lighting, 1.0);
}