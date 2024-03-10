#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 uv;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(position, 1.0f);
}