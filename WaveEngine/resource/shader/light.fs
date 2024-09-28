#version 330 core

uniform vec4 lightColor;

void main()
{
    gl_FragColor = lightColor;
}