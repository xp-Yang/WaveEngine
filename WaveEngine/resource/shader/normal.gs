#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 fragWorldPos;          //��������
    vec3 fragWorldNormal;       //��������
    vec2 fragUV;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;

void GenerateLine(int index)
{
    const float MAGNITUDE = 0.2;
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    // TODO ��2�ֵ�����
    gl_Position = gl_in[index].gl_Position + vec4(mat3(projection) * mat3(view) * gs_in[index].fragWorldNormal, 0.0) * MAGNITUDE; // projection��ʧ�����У��൱��������һ����͸�ӳ���
    // �൱��vs��gl_Position���˱任���󣬵�gs���ȼ���normal����һ�����pvm����
    gl_Position = gl_in[index].gl_Position + projection * view * vec4(gs_in[index].fragWorldNormal * MAGNITUDE, 0.0); // vec4(gs_in[index].fragWorldNormal, 0.0) == normalize(model * vec4(gs_in[index].modelNormal, 0.0))
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}