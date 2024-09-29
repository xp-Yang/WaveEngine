#version 330 core

#include "common.h"

in vec2 fragUV;

uniform sampler2D Texture;
//uniform sampler2DMS Texture;
uniform sampler2D bloomBlurMap;
uniform sampler2D borderMap;
uniform sampler2D normalMap;
uniform bool bloom;
uniform bool border;
uniform bool normal;
//uniform float exposure;

out vec4 FragColor;

void main()
{
    vec3 hdrColor = texture(Texture, fragUV).rgb;      
    if(bloom) {
	    vec3 bloomColor = texture(bloomBlurMap, fragUV).rgb;
        hdrColor += bloomColor; // additive blending
	}
    if(border) {
	    vec3 borderColor = texture(borderMap, fragUV).rgb;
        hdrColor += borderColor;
	}
    if(normal) {
        vec3 normalColor = texture(normalMap, fragUV).rgb;
        hdrColor += normalColor;
    }
    // tone mapping
    vec3 result = ToneMapping(hdrColor, 1.0);
    // gamma correct
    result = GammaCorrection(result);
    FragColor = vec4(result, 1.0);
}