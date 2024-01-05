#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gDiffuse;
layout (location = 3) out vec3 gSpecular;

in VS_OUT{
    vec3 pos;
    vec3 normal;
    vec2 uv;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

void main()
{    
    gPosition = fs_in.pos;
    gNormal = normalize(fs_in.normal);
    gDiffuse.rgb = texture(diffuse_map, fs_in.uv).rgb;
    gSpecular.rgb = texture(specular_map, fs_in.uv).rgb;
}  