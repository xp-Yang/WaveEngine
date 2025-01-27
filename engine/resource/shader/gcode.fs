#version 330 core

in float vertexID;
in vec3 worldPos;
in vec3 gouraudColor;
in vec3 averageColor;

uniform mat4 view;
uniform mat4 projection;

uniform float layerHeight;

out vec4 outColor;

void main()
{
    // remove the moire pattern
    outColor.a = 1.0;
    vec2 repeating_dir = vec2(dFdx(vertexID), dFdy(vertexID));
    vec4 ndc_repeating_dir = vec4(repeating_dir, -1.0, 1.0);
    vec4 world_repeating_dir = inverse(projection * view) * ndc_repeating_dir;
    world_repeating_dir /= world_repeating_dir.w;
    world_repeating_dir.xyz = normalize(world_repeating_dir.xyz);
    float lod = 1 - smoothstep(0., layerHeight, fwidth(dot(world_repeating_dir.xyz, worldPos)));
    //float lod = 1 - clamp(length(fwidth(worldPos)) * (1./layerHeight), 0., 1.);
    outColor.xyz = mix(averageColor, gouraudColor, lod);
}