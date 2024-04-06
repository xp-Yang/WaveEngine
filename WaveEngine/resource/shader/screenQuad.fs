#version 330 core

#include "common.h"

in vec2 fragUV;

uniform sampler2D Texture;
//uniform sampler2DMS Texture;
uniform sampler2D bloomBlurMap;
uniform sampler2D borderMap;
uniform bool bloom;
uniform bool border;
//uniform float exposure;

out vec4 FragColor;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(Texture, fragUV).rgb;      
    vec3 bloomColor = texture(bloomBlurMap, fragUV).rgb;
    vec3 borderColor = texture(borderMap, fragUV).rgb;
    if(bloom)
        hdrColor += bloomColor; // additive blending
    if(border)
        hdrColor += borderColor;
    // tone mapping
    vec3 result = ToneMapping(hdrColor, 1.0);
    // gamma correct
    result = GammaCorrection(result);
    FragColor = vec4(result, 1.0);

    // customize anti-aliasing
    //ivec2 texSize = textureSize(Texture);
    //gl_FragColor = texelFetch(Texture, ivec2(fragUV * texSize), 3);

    // debug depth
    //float depthValue = texture(Texture, fragUV).r;
    //float z = depthValue * 2.0 - 1.0; // back to NDC 
    //float near = 0.1; 
    //float far  = 100.0; 
    //float depth = (2.0 * near * far) / (far + near - z * (far - near)) / far;
    //gl_FragColor = vec4(vec3(depth), 1.0);
    
    // debug color
    //gl_FragColor = vec4(0.2, 1.0, 0.0, 1.0);
}