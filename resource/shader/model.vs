#version 330 core
layout (location = 0) in vec3 vertex_pos;
//layout (location = 1) in vec3 vertex_clr;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 pass_color;
    vec2 pass_uv;
    vec3 pass_normal;
    vec3 pass_pos;
} vs_out;

void main()
{
    //pass_color = vec4(color.x * vertex_clr.x, color.y * vertex_clr.y, color.z * vertex_clr.z, 1.0f);
    vs_out.pass_color = color;
	vs_out.pass_uv = vertex_uv;
    vs_out.pass_pos = vec3(model * vec4(vertex_pos, 1.0));
    vs_out.pass_normal = vec3(model * vec4(vertex_normal, 0.0));
    //Normal = mat3(transpose(inverse(model))) * aNormal;  

    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}