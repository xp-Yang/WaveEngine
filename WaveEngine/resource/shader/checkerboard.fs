#version 330 core

#include "common.h"

in vec3 fragScaledModelPos;

uniform sampler2D shadow_map;
uniform mat4 lightSpaceMatrix;

void main()
{
    vec3 integer = floor(fragScaledModelPos);
    if((mod(integer.x, 2) == 0 && mod(integer.z, 2) == 0) ||
        (mod(integer.x, 2) == 1 && mod(integer.z, 2) == 1))
    {
        if(mod(integer.y, 2) == 0)
            gl_FragColor = vec4(0.25, 0.25, 0.25, 1.0);
        else
            gl_FragColor = vec4(0.75, 0.75, 0.75, 1.0);
    }
    else{
        if(mod(integer.y, 2) == 0)
            gl_FragColor = vec4(0.75, 0.75, 0.75, 1.0);
        else
            gl_FragColor = vec4(0.25, 0.25, 0.25, 1.0);
    }
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragScaledModelPos, 1.0);
    float shadowFactor = ShadowCalculation(fragPosLightSpace, shadow_map);
    gl_FragColor = shadowFactor * gl_FragColor;
}