#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    mat4 projectionView;
    vec3 fragWorldPos;          //��������
    vec3 fragWorldNormal;       //��������
    vec2 fragUV;
    vec4 fragPosLightSpace;     //�ü�����
} gs_in[];

void main()
{
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}