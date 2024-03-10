#version 330 core

in vec3 fragUV;

uniform samplerCube skybox;

void main()
{    
    gl_FragColor = texture(skybox, fragUV);
}