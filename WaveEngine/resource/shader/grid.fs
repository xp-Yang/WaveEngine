#version 330 core

in vec3 fragPos;

uniform sampler2D shadow_map;
uniform mat4 lightSpaceMatrix;

// TODO 把这些公用函数挪到一个公用头文件中，然后include
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 1.还在裁剪空间，执行透视除法，变换到NDC空间
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 2.变换到[0,1]的范围, 便于采样
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;

    float bias = 0.005;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    // 检查当前片段是否在阴影中
    float shadow = projCoords.z - closestDepth > bias ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 integer = floor(fragPos);
    if((mod(integer.x, 2) == 0 && mod(integer.z, 2) == 0) ||
        (mod(integer.x, 2) == 1 && mod(integer.z, 2) == 1))
    {
        gl_FragColor = vec4(0.25, 0.25, 0.25, 1.0);
    }
    else{
        gl_FragColor = vec4(0.75, 0.75, 0.75, 1.0);
    }

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);
    gl_FragColor = (1.0 - shadow) * gl_FragColor;
}