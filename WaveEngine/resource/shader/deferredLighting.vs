#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

out vec2 fragUV;

void main()
{
    fragUV = vertex_uv;
    gl_Position = vec4(vertex_pos, 1.0);
}