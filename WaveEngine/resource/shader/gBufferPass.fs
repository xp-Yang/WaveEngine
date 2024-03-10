#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gDiffuse;
layout (location = 3) out vec3 gSpecular;

in VS_OUT{
    vec3 fragWorldPos;
    vec3 fragWorldNormal;
    vec2 fragUV;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

void main()
{    
    gPosition = fs_in.fragWorldPos;
    gNormal = normalize(fs_in.fragWorldNormal);
    gDiffuse.rgb = texture(diffuse_map, fs_in.fragUV).rgb;
    gSpecular.rgb = texture(specular_map, fs_in.fragUV).rgb;
}  