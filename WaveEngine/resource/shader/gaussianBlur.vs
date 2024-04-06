#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 uv;

out vec2 TexCoords;

void main()
{
    TexCoords = uv;
    gl_Position = vec4(vertex_pos, 1.0);
}