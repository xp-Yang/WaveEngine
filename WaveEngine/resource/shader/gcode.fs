#version 330 core

in float vertexID;
in vec3 worldPos;
in vec3 gouraudColor;
in vec3 averageColor;

uniform float layerHeight;

out vec4 outColor;

void main()
{
    outColor.a = 1.0;
    // remove the moire pattern
    float lod = 1 - clamp(length(fwidth(worldPos)) * (2./layerHeight), 0., 1.);
    outColor.xyz = mix(averageColor, gouraudColor, lod);
    //float lod = 1 - clamp(fwidth(vertexID), 0., 1.);
    //outColor.xyz = mix(vec3(1,1,0), gouraudColor, lod);
}