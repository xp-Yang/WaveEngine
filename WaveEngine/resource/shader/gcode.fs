#version 330 core

in float vertexID;
in vec3 worldPos;
in vec3 gouraudColor;
in vec3 averageColor;

uniform mat4 view;

uniform float layerHeight;

out vec4 outColor;

void main()
{
    // remove the moire pattern
    outColor.a = 1.0;
    vec3 repeating_dir = normalize(vec3(dFdx(vertexID), dFdy(vertexID), 0.0));
    vec3 world_repeating_dir = mat3(inverse(view)) * repeating_dir;
    //float lod = 1 - smoothstep(0., layerHeight, fwidth(dot(world_repeating_dir, worldPos)));
    float lod = 1 - clamp(length(fwidth(worldPos)) * (1./layerHeight), 0., 1.);
    outColor.xyz = mix(averageColor, gouraudColor, lod);
    //outColor.xyz = mix(vec3(1,1,0), gouraudColor, lod);
}