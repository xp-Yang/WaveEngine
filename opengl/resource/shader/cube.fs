#version 330 core
in vec3 pass_color;
in vec2 pass_uv;
in vec3 pass_normal;
in vec3 pass_pos;
in vec3 pass_view_pos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    float ambient;
    float diffuse;
    float specular;
    float shininess;
};

uniform vec3 light_color;
uniform vec3 light_pos;

uniform Material material;

uniform sampler2D Texture;

out vec4 fragment_color;

void main()
{
    vec3 ambient_light = light_color * material.ambient;

    vec3 normal = normalize(pass_normal);
    vec3 light_direction = normalize(light_pos - pass_pos);
    float diffuse_coef = max(dot(light_direction, normal), 0.0);
    vec3 diffuse_light = light_color * diffuse_coef * material.diffuse;

    vec3 view_direction = normalize(pass_view_pos - pass_pos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec_coef = pow(max(dot(view_direction, reflect_direction), 0.001), material.shininess);
    vec3 specular_light = light_color * spec_coef * material.specular;

    fragment_color = vec4(pass_color * (ambient_light + diffuse_light + specular_light), 1.0);
	
    //fragment_color = texture(Texture, pass_uv);

    //debug
    //fragment_color = vec4(pass_pos, 1.0f);
}