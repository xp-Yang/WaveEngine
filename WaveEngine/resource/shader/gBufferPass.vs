#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

out VS_OUT {
    vec3 pos;
    vec3 normal;
    vec2 uv;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.pos = vec3(model * vec4(vertex_pos, 1.0));
    vs_out.normal = transpose(inverse(mat3(model))) * vertex_normal;
    vs_out.uv = vertex_uv;

    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}