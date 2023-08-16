#version 330 core
layout (location = 0) in vec3 vertex_pos;
//layout (location = 1) in vec3 vertex_clr;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_uv;

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 pass_color;
out vec2 pass_uv;
out vec3 pass_pos;
out vec3 pass_normal;
out vec3 pass_view_pos;

void main()
{
    //pass_color = vec4(color.x * vertex_clr.x, color.y * vertex_clr.y, color.z * vertex_clr.z, 1.0f);
    pass_color = color;
	pass_uv = vertex_uv;
    pass_pos = vec3(model * vec4(vertex_pos, 1.0));
    pass_normal = vec3(model * vec4(vertex_normal, 0.0));
    //Normal = mat3(transpose(inverse(model))) * aNormal;  
    pass_view_pos = view[3].xyz;

    gl_Position = projection * view * model * vec4(vertex_pos, 1.0);
}