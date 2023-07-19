#version 330 core
in vec4 pass_color;
in vec2 pass_texCoord;

uniform sampler2D Texture;

out vec4 color;

void main()
{
    //color = pass_color;
	color = texture(Texture, pass_texCoord) * pass_color;
}