#version 330 core

#include "common.h"

in vec2 fragUV;

uniform sampler2D Texture;
//uniform sampler2DMS Texture;
uniform sampler2D bloomMap;

out vec4 FragColor;

void main()
{
    vec3 hdrColor = texture(Texture, fragUV).rgb;      

    vec3 bloomColor = texture(bloomMap, fragUV).rgb;
    hdrColor += bloomColor; // additive blending

    // tone mapping
    vec3 result = ToneMapping(hdrColor, 1.0);
    // gamma correct
    result = GammaCorrection(result);
    FragColor = vec4(result, 1.0);
}