#version 330 core
in vec3 pass_color;

out vec4 fragment_color;

void main()
{
    fragment_color = vec4(pass_color.xyz, 1.0);
}