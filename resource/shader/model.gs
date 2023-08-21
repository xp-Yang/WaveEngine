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

vec4 explode(vec4 position, vec3 dir)
{
    vec3 displacement = dir * magnitude; 
    return position + vec4(displacement, 0.0);
}

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

void main() {    
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    gs_out.pass_uv = gs_in[0].pass_uv;
    gs_out.pass_color = gs_in[0].pass_color;
    gs_out.pass_normal = gs_in[0].pass_normal;
    gs_out.pass_pos = gs_in[0].pass_pos;
    gs_out.FragPosLightSpace = gs_in[0].FragPosLightSpace;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    gs_out.pass_uv = gs_in[1].pass_uv;
    gs_out.pass_color = gs_in[1].pass_color;
    gs_out.pass_normal = gs_in[1].pass_normal;
    gs_out.pass_pos = gs_in[1].pass_pos;
    gs_out.FragPosLightSpace = gs_in[1].FragPosLightSpace;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    gs_out.pass_uv = gs_in[2].pass_uv;
    gs_out.pass_color = gs_in[2].pass_color;
    gs_out.pass_normal = gs_in[2].pass_normal;
    gs_out.pass_pos = gs_in[2].pass_pos;
    gs_out.FragPosLightSpace = gs_in[2].FragPosLightSpace;
    EmitVertex();
    EndPrimitive();
}