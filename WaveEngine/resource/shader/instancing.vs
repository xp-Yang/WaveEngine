#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;
layout (location = 3) in mat4 instance_matrix;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * instance_matrix * vec4(vertex_pos, 1.0f); 
}