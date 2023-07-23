#version 330 core
in vec3 pass_color;
in vec2 pass_uv;
in vec3 pass_normal;
in vec3 pass_pos;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform float ambient_strength;

uniform sampler2D Texture;

out vec4 fragment_color;

void main()
{
    vec3 ambient_light = light_color * ambient_strength;

    vec3 normal = normalize(pass_normal);
    vec3 light_direction = normalize(light_pos - pass_pos);
    float diffuse = max(dot(light_direction, normal), 0.0);
    vec3 diffuse_light = light_color * diffuse;

    fragment_color = vec4(pass_color * (ambient_light + diffuse_light), 1.0);
	
    fragment_color = texture(Texture, pass_uv);
    //fragment_color = vec4(pass_normal, 1.0f);
}