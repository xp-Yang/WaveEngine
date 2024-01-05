#version 330 core
in vec4 pass_color;

void main()
{
    gl_FragColor = pass_color;
}