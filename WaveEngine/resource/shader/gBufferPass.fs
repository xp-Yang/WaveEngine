#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gDiffuse;
layout (location = 3) out vec3 gSpecular;
// pbr
layout (location = 4) out vec3 gAlbedo;
layout (location = 5) out vec3 gMetallic;
layout (location = 6) out vec3 gRoughness;
layout (location = 7) out vec3 gAo;

in VS_OUT{
    vec3 fragWorldPos;
    vec3 fragWorldNormal;
    vec2 fragUV;
} fs_in;

uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

void main()
{    
    gPosition = fs_in.fragWorldPos;
    gNormal = normalize(fs_in.fragWorldNormal);
    gDiffuse.rgb = texture(diffuse_map, fs_in.fragUV).rgb;
    gSpecular.rgb = texture(specular_map, fs_in.fragUV).rgb;

    gAlbedo = albedo;
    gMetallic = vec3(metallic);
    gRoughness = vec3(roughness);
    gAo = vec3(ao);
}  