#ifndef Shader_hpp
#define Shader_hpp

#include "Core/Math/Math.hpp"

namespace Asset {

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
	ShaderType shader_type{ ShaderType::None };
	std::string vshader_filepath;
	std::string fshader_filepath;
	std::string gshader_filepath;
};

}

#endif

