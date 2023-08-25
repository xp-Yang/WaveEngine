#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    mat4 vp;
    vec3 pass_color;
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} gs_in[];

uniform float magnitude;

uniform mat4 projection;

void GenerateLine(int index)
{
    const float MAGNITUDE = 0.1;
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = (gl_in[index].gl_Position + gs_in[index].vp * vec4(gs_in[index].pass_normal, 0.0) * MAGNITUDE);
    EmitVertex();
    //每发射两个顶点在这里结束一个图元的定义，而不是生成6个顶点后才调用一次，那样的话6个点形成一个line_strip
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}