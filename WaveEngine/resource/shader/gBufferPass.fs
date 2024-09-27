#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gSpecular;
// pbr
layout (location = 4) out vec4 gAlbedo;
layout (location = 5) out vec4 gMetallic;
layout (location = 6) out vec4 gRoughness;
layout (location = 7) out vec4 gAo;

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
    gPosition = vec4(fs_in.fragWorldPos, 1.0);
    gNormal = vec4(normalize(fs_in.fragWorldNormal), 1.0);
    gDiffuse = vec4(texture(diffuse_map, fs_in.fragUV).rgb, 1.0);
    gSpecular = vec4(texture(specular_map, fs_in.fragUV).rgb, 1.0);

    gAlbedo = vec4(albedo, 1.0);
    gMetallic = vec4(vec3(metallic), 1.0);
    gRoughness = vec4(vec3(roughness), 1.0);
    gAo = vec4(vec3(ao), 1.0);
}  