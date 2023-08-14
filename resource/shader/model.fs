#version 330 core
out vec4 fragment_color;

in vec2 pass_uv;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

void main()
{    
    fragment_color = texture(texture_diffuse1, pass_uv);
//    fragment_color = texture(texture_specular1, pass_uv);
//    fragment_color = texture(texture_height1, pass_uv);
}