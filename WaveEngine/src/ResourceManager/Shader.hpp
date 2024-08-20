#ifndef Shader_hpp
#define Shader_hpp

#include "Core/Math/Math.hpp"

namespace Asset {

enum class ShaderType {
	None,
	GBufferShader,
	DeferredLightingShader,
	PBRShader,
	SkyboxShader,
	BorderShader,
	NormalShader,
	WireframeShader,
	CheckerboardShader,
	PickingShader,
	RayTracingShader,
	QuadShader,
	DepthShader,
	CubeMapShader,
	GaussianBlur,
	EdgeDetection,
	CustomShader,
};

struct Shader {
	ShaderType shader_type{ ShaderType::None };
	std::string vshader_filepath;
	std::string fshader_filepath;
	std::string gshader_filepath;
};

}

#endif

