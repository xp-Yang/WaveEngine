#version 330 core

in vec3 pass_uv;

uniform samplerCube skybox;

out vec4 out_color;

void main()
{    
    out_color = texture(skybox, pass_uv);
}