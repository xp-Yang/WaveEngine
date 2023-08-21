#version 330 core

in vec2 pass_uv;
uniform sampler2D Texture;
out vec4 fragment_color;
void main()
{
    //fragment_color = texture(Texture, pass_uv);
    float depthValue = texture(Texture, pass_uv).r;
    fragment_color = vec4(vec3(depthValue), 1.0);
    //fragment_color = vec4(0.2, 1.0, 0.0, 1.0);
}