struct DirectionalLight
{
    vec3 direction;
    vec4 color;
};

struct PointLight
{
    vec3  position;
	vec4  color;
    float radius;
};

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadow_map)
{
    // 1.还在裁剪空间，执行透视除法，变换到NDC空间
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 2.变换到[0,1]的范围, 便于采样
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 1.0;

    float bias = 0.005;
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float closestDepth = texture(shadow_map, projCoords.xy).r;
    // 检查当前片段是否在阴影中
    float shadowFactor = projCoords.z - closestDepth > bias ? 0.3 : 1.0;

    return shadowFactor;
}

//in VS_OUT {
//    vec3 fragWorldPos;          //世界坐标
//    vec3 fragWorldNormal;       //世界坐标
//    vec2 fragUV;
//} fs_in;