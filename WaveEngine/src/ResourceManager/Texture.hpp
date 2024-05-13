#ifndef Texture_hpp
#define Texture_hpp

#include <string>
#include <vector>

namespace Asset {

enum class TextureType {
	None,
	Diffuse,
	Specular,
	Normal,
	Height,
	Albedo,
	Metallic,
	Roughness,
	AO,
};

struct Texture {
	Texture() = default;
	Texture(TextureType type, const std::string& filepath, bool gamma)
		: texture_type(type)
		, texture_filepath(filepath)
		, gamma(gamma)
	{}

	TextureType texture_type{ TextureType::None };
	std::string texture_filepath;
	bool gamma{ false };
};

struct CubeTexture {
	CubeTexture() = default;
	CubeTexture(
		const std::string& right_filepath,
		const std::string& left_filepath,
		const std::string& top_filepath,
		const std::string& bottom_filepath,
		const std::string& front_filepath,
		const std::string& back_filepath)
	{
		cube_texture_filepath[0] = right_filepath;
		cube_texture_filepath[1] = left_filepath;
		cube_texture_filepath[2] = top_filepath;
		cube_texture_filepath[3] = bottom_filepath;
		cube_texture_filepath[4] = front_filepath;
		cube_texture_filepath[5] = back_filepath;
	}

	std::array<std::string, 6> cube_texture_filepath;
};

}

#endif
