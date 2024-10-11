#ifndef Shader_hpp
#define Shader_hpp

#include "Core/Math/Math.hpp"

enum class ShaderType {
	None,
	PristineGridShader,
	GBufferShader,
	GBufferPhongShader,
	DeferredLightingShader,
	DeferredLightingPhongShader,
	OneColorShader,
	PBRShader,
	SkyboxShader,
	NormalShader,
	WireframeShader,
	CheckerboardShader,
	RayTracingShader,
	CombineShader,
	CubeMapShader,
	ExtractBrightShader,
	GaussianBlur,
	OutlineShader,
	InstancingShader,
};

struct Shader {
	Shader(const std::string& vs_filepath, const std::string& fs_filepath);
	Shader(const std::string& vs_filepath, const std::string& fs_filepath, const std::string& gs_filepath);

	std::string vsCode;
	std::string fsCode;
	std::string gsCode;
};

#endif

