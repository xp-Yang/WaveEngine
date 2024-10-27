#version 330 core

in vec4 gouraudColor;

out vec4 outColor;

void main()
{
    outColor = gouraudColor;
}