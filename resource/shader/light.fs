#version 330 core
in vec4 pass_color;

out vec4 fragment_color;

void main()
{
    fragment_color = pass_color;
}