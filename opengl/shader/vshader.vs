#version 330 core
layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 in_clr;
uniform mat4 transform;
out vec4 pass_color;
void main()
{
    gl_Position = transform * vec4(in_pos.x, in_pos.y, in_pos.z, 1.0);
    pass_color = vec4(in_clr.x, in_clr.y, in_clr.z, 1.0);
}