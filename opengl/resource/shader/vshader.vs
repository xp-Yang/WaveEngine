#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_clr;
layout (location = 2) in vec2 in_texCoord;

uniform float time_var;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 pass_color;
out vec2 pass_texCoord;

void main()
{
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
    pass_color = vec4(in_clr.x * time_var, in_clr.y * time_var, in_clr.z * time_var, 1.0);
	pass_texCoord = in_texCoord;
}