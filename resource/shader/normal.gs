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
    //ÿ���������������������һ��ͼԪ�Ķ��壬����������6�������ŵ���һ�Σ������Ļ�6�����γ�һ��line_strip
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // ��һ�����㷨��
    GenerateLine(1); // �ڶ������㷨��
    GenerateLine(2); // ���������㷨��
}