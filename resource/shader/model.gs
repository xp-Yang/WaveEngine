#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    mat4 vp;
    vec4 pass_color;
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} gs_in[];

out GS_OUT{
    vec4 pass_color;
    vec3 pass_pos;
    vec2 pass_uv;
    vec3 pass_normal;
    vec4 FragPosLightSpace;
} gs_out;

uniform float explosionRatio;

vec4 explode(vec3 position, vec3 dir)
{
    vec3 displacement = dir * explosionRatio; 
    return vec4(position + displacement, 1.0);
}

vec3 GetNormal()
{
   vec3 a = vec3(gs_in[0].pass_pos) - vec3(gs_in[1].pass_pos);
   vec3 b = vec3(gs_in[2].pass_pos) - vec3(gs_in[1].pass_pos);
   return normalize(cross(b, a));
}

void main() {    
    //vec3 normal = GetNormal();
    vec3 normal = (gs_in[0].pass_normal + gs_in[1].pass_normal + gs_in[2].pass_normal) / 3;

    //输入的gs_in[0].pass_pos为世界坐标
    //输入和输出的gl_Position为裁剪空间坐标
    gl_Position = gs_in[0].vp * explode(gs_in[0].pass_pos, normal);
    gs_out.pass_uv = gs_in[0].pass_uv;
    gs_out.pass_color = gs_in[0].pass_color;
    gs_out.pass_normal = gs_in[0].pass_normal;
    gs_out.pass_pos = gs_in[0].pass_pos;
    gs_out.FragPosLightSpace = gs_in[0].FragPosLightSpace;
    EmitVertex();
    gl_Position = gs_in[1].vp * explode(gs_in[1].pass_pos, normal);
    gs_out.pass_uv = gs_in[1].pass_uv;
    gs_out.pass_color = gs_in[1].pass_color;
    gs_out.pass_normal = gs_in[1].pass_normal;
    gs_out.pass_pos = gs_in[1].pass_pos;
    gs_out.FragPosLightSpace = gs_in[1].FragPosLightSpace;
    EmitVertex();
    gl_Position = gs_in[2].vp * explode(gs_in[2].pass_pos, normal);
    gs_out.pass_uv = gs_in[2].pass_uv;
    gs_out.pass_color = gs_in[2].pass_color;
    gs_out.pass_normal = gs_in[2].pass_normal;
    gs_out.pass_pos = gs_in[2].pass_pos;
    gs_out.FragPosLightSpace = gs_in[2].FragPosLightSpace;
    EmitVertex();
    EndPrimitive();
}