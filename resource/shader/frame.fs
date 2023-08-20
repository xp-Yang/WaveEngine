#version 330 core

in vec2 pass_uv;
uniform sampler2D Texture;
out vec4 fragment_color;
void main()
{
    fragment_color = texture(Texture, pass_uv);
    //fragment_color = vec4(0.2, 1.0, 0.0, 1.0);
}