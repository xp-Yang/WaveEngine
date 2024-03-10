#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 fragWorldPos;
    vec3 fragWorldNormal;
    vec2 fragUV;
} vs_out;

void main()
{
    vs_out.fragWorldPos = vec3(model * vec4(vertex_pos, 1.0));
    vs_out.fragWorldNormal = transpose(inverse(mat3(model))) * vertex_normal;
    vs_out.fragUV = vertex_uv;

    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}