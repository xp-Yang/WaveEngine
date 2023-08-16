#version 330 core
in vec3 pass_color;
in vec2 pass_uv;
in vec3 pass_normal;
in vec3 pass_pos;
in vec3 pass_view_pos;

struct Light {
    vec3 position;
    vec3 color;
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
uniform Light light;
uniform vec3 viewpos;

//uniform sampler2D Texture;

out vec4 fragment_color;

void main()
{
    vec3 ambient_light = light.color * material.ambient * vec3(texture(material.diffuse_map, pass_uv));

    vec3 normal = normalize(pass_normal);
    vec3 light_direction = normalize(light.position - pass_pos);
    float diffuse_coef = max(dot(light_direction, normal), 0.0);
    vec3 diffuse_light = light.color * diffuse_coef * vec3(texture(material.diffuse_map, pass_uv));

    vec3 view_direction = normalize(viewpos - pass_pos);
    vec3 reflect_direction = reflect(-light_direction, normal);
    float spec_coef = pow(max(dot(view_direction, reflect_direction), 0.001), material.shininess);
    vec3 specular_light = light.color * spec_coef * vec3(texture(material.specular_map, pass_uv));

    fragment_color = vec4(pass_color * (ambient_light + diffuse_light + specular_light), 1.0);
	
    //fragment_color = texture(Texture, pass_uv);

    //debug
    //fragment_color = vec4(abs(spec_coef),abs(spec_coef),abs(spec_coef), 1.0f);
    //fragment_color = vec4(abs(view_direction), 1.0f);
}