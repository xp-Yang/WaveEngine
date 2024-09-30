#version 330 core

#include "common.h"

in vec2 fragUV;

uniform sampler2D Texture;
//uniform sampler2DMS Texture;
uniform sampler2D normalMap;
uniform sampler2D bloomMap;
uniform sampler2D outlineMap;

out vec4 FragColor;

void main()
{
    vec3 hdrColor = texture(Texture, fragUV).rgb;      

    vec3 bloomColor = texture(bloomMap, fragUV).rgb;
    hdrColor += bloomColor; // additive blending

    vec3 outlineColor = texture(outlineMap, fragUV).rgb;
    hdrColor += outlineColor;

    vec3 normalColor = texture(normalMap, fragUV).rgb;
    hdrColor += normalColor;

    // tone mapping
    vec3 result = ToneMapping(hdrColor, 1.0);
    // gamma correct
    result = GammaCorrection(result);
    FragColor = vec4(result, 1.0);
}