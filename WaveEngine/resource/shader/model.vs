#version 330 core
layout (location = 0) in vec3 vertex_pos;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec3 fragWorldPos;          //世界坐标
    vec3 fragWorldNormal;       //世界坐标
    vec2 fragUV;
} vs_out;

void main()
{
    vs_out.fragWorldPos = vec3(model * vec4(vertex_pos, 1.0));
    vs_out.fragWorldNormal = normalize(mat3(model) * vertex_normal);
	vs_out.fragUV = vertex_uv;
    //这里输出的gl_Position为Clip Space，给到gs时还是Clip Space，到fs时已经自动做了 (透视除法) => NDC => (视口变换) => Screen Space，变成了屏幕空间Screen Space
    //裁剪坐标
    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}