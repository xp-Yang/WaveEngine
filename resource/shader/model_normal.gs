#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 pass_color;
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} gs_in[];

out GS_OUT{
    vec3 pass_color;
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} gs_out;

uniform float magnitude;

const float MAGNITUDE = 0.4;

uniform mat4 projection;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    gs_out.pass_uv = gs_in[index].pass_uv;
    gs_out.pass_color = gs_in[index].pass_color;
    gs_out.pass_normal = gs_in[index].pass_normal;
    gs_out.pass_pos = gs_in[index].pass_pos;
    gs_out.FragPosLightSpace = gs_in[index].FragPosLightSpace;
    EmitVertex();
    //gl_Position = (gl_in[index].gl_Position + vec4(gs_in[index].pass_normal, 0.0) * MAGNITUDE);
    //EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}