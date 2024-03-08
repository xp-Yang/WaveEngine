#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

out VS_OUT {
    mat4 vp;
    vec3 fragPos;          //世界坐标
    vec2 fragUV;
    vec3 fragNormal;       //局部坐标
    vec4 fragPosLightSpace; //裁剪坐标
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.vp = projection * view;
    vs_out.fragUV = vertex_uv;
    vs_out.fragPos = vec3(model * vec4(vertex_pos, 1.0));
    vs_out.fragNormal = mat3(model) * vertex_normal;   

    gl_Position =  projection * view * vec4(vs_out.fragPos, 1.0);
}