#version 330 core
in vec3 pass_color;

uniform sampler2D Texture;

out vec4 fragment_color;

void main()
{
    fragment_color = vec4(pass_color.xyz, 1.0);
}