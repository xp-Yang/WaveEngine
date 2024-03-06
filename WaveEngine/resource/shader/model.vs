#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out VS_OUT {
    mat4 vp;
    vec3 pass_pos;          //世界坐标
    vec2 pass_uv;
    vec3 pass_normal;       //局部坐标
    vec4 FragPosLightSpace; //裁剪坐标
} vs_out;

void main()
{
    vs_out.vp = projection * view;
	vs_out.pass_uv = vertex_uv;
    vs_out.pass_pos = vec3(model * vec4(vertex_pos, 1.0));
    //vs_out.pass_normal = vec3(model * vec4(vertex_normal, 0.0));
    vs_out.pass_normal = vertex_normal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.pass_pos, 1.0);
    //Normal = mat3(transpose(inverse(model))) * aNormal;  
    //这里输出的gl_Position为裁剪空间，给到gs时还是裁剪空间，到fs时已经自动做了透视除法，变成了NDC空间
    //裁剪坐标
    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}