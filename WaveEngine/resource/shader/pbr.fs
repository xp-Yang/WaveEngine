#version 330 core
out vec4 FragColor;

in VS_OUT {
    mat4 vp;
    vec3 fragPos;          //世界坐标
    vec2 fragUV;
    vec3 fragNormal;       //局部坐标
    vec4 fragPosLightSpace; //裁剪坐标
} fs_in;

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
    vec3  intensity;
};

uniform DirectionalLight directionalLight;
const int MAX_POINT_LIGHTS_COUNT = 128;
uniform int point_lights_size;
uniform PointLight pointLights[MAX_POINT_LIGHTS_COUNT];

// TODO should be map
// struct Material {
// };
// uniform Material material;
// material parameters
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 cameraPos;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 BRDF(vec3  L,
          vec3  V,
          vec3  N,
          vec3  F0,
          vec3  basecolor,
          float metallic,
          float roughness)
{
	vec3 H = normalize(V + L);
		  
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);   
	float G   = GeometrySmith(N, V, L, roughness);      
	vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
		
	vec3 numerator    = NDF * G * F; 
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;
	
	// kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metallic;	
	
	// scale light by NdotL
	float NdotL = max(dot(N, L), 0.0);
	//vec3 color = (kD * basecolor / PI + (1.0 - kD) * specular);
    vec3 color = (kD * basecolor / PI + specular) * NdotL;

    return color;
}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 N = normalize(fs_in.fragNormal);
    vec3 V = normalize(cameraPos - fs_in.fragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < point_lights_size; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(pointLights[i].position - fs_in.fragPos);
        vec3 H = normalize(V + L);
        float distance = length(pointLights[i].position - fs_in.fragPos);
        float attenuation = 1.0;//1.0 / (distance * distance);
        vec3 radiance = pointLights[i].color.xyz * attenuation;  

        // add to outgoing radiance Lo
        Lo += radiance * BRDF(L, V, N, F0, radiance, metallic, roughness);  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // // gamma correct
    // color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
