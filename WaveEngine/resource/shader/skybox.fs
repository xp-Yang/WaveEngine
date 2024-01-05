#version 330 core

in vec3 pass_uv;

uniform samplerCube skybox;

void main()
{    
    gl_FragColor = texture(skybox, pass_uv);
}