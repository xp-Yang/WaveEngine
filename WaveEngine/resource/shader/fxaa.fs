#version 330

in vec2 fragUV;
out vec4 outColor;

uniform sampler2D mainTexture;


float calcLuminance(vec4 color) {
	return dot(vec3(color), vec3(0.213, 0.715, 0.072));
}

float calcContrast(float N_luma, float W_luma, float S_luma, float E_luma, float M_luma) {
	float max_luminance = max(N_luma,
	 max(W_luma,
	  max(S_luma,
	   max(E_luma, M_luma))));
	float min_luminance = min(N_luma,
	 min(W_luma,
	  min(S_luma,
	   min(E_luma, M_luma)))); 
	float contrast = max_luminance - min_luminance;
	return contrast;
}

vec2 calcEdgeNormal(float N_luma, float W_luma, float S_luma, float E_luma, float M_luma)
{
	float gradHorizontal = (N_luma - M_luma) - (M_luma - S_luma);
	float gradVertical = (E_luma - M_luma) - (M_luma - W_luma);
	bool isHorizontal = abs(gradHorizontal) > abs(gradVertical);
	
	// myIsEdgeGradHorizontal
	//gradHorizontal = (N_luma - M_luma) + (M_luma - S_luma);
	//gradVertical = (E_luma - M_luma) + (M_luma - W_luma);
	//isHorizontal = abs(gradHorizontal) > abs(gradVertical);
	
	vec2 normal = vec2(0.0);
	if (isHorizontal)
		normal.y = sign(abs(N_luma - M_luma) - abs(M_luma - S_luma));
	else
		normal.x = sign(abs(E_luma - M_luma) - abs(M_luma - W_luma));
	return normal;
}


void main() {
	vec2 tex_offset = 1.0 / textureSize(mainTexture, 0);
	vec2 st = fragUV;
	vec4 N = texture(mainTexture, vec2(st.x, st.y + tex_offset.y));
	vec4 S = texture(mainTexture, vec2(st.x, st.y - tex_offset.y));
	vec4 W = texture(mainTexture, vec2(st.x - tex_offset.x, st.y));
	vec4 E = texture(mainTexture, vec2(st.x + tex_offset.x, st.y));
	vec4 M = texture(mainTexture, st);
	float N_luma = calcLuminance(N);
	float W_luma = calcLuminance(W);
	float S_luma = calcLuminance(S);
	float E_luma = calcLuminance(E);
	float M_luma = calcLuminance(M);
	
	float contrast = calcContrast(N_luma, W_luma, S_luma, E_luma, M_luma);
	if (contrast < 0.15) {
		discard;
	}

	vec2 normal = calcEdgeNormal(N_luma, W_luma, S_luma, E_luma, M_luma);
	vec4 color = vec4(0.0);	
	float expect_luma = (N_luma + W_luma + S_luma + M_luma) * 0.25;
	float luma_delta = abs(M_luma - expect_luma);
	float blend = luma_delta / contrast;
	//color = texture(mainTexture, st + normal * blend * tex_offset);
	if (normal.x > 0)
		color = mix(M,E,blend);
	if (normal.x < 0)
		color = mix(M,W,blend);
	if (normal.y > 0)
		color = mix(M,N,blend);
	if (normal.y < 0)
		color = mix(M,S,blend);
	outColor = color;
	outColor = (N + S + W + E + M) / 5.0;
	//outColor = M;
	//outColor = vec4(vec3(contrast), 1.0);
}