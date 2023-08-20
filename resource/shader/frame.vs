#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec2 uv;

out vec2 pass_uv;

void main()
{
    gl_Position = vec4(vertex_pos, 1.0);
    pass_uv = uv;
}