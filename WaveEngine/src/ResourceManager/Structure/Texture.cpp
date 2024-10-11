#include "Texture.hpp"

Texture::Texture(TextureType type, const std::string& filepath, bool gamma);
	: texture_type(type)
	, texture_filepath(filepath)
	, gamma(gamma)
{
        const auto& rhi = RenderSourceData::rhi;

        std::string filepath = texture_asset.texture_filepath;
        data = stbi_load(filepath.c_str(), &width, &height, &channel_count, 0);

        if (data)
        {
            RhiTexture::Format format;
            if (nrComponents == 1)
                format = RhiTexture::Format::R8;
            else if (nrComponents == 3)
                format = RhiTexture::Format::RGB8;
            else if (nrComponents == 4)
                format = RhiTexture::Format::RGBA8;

            stbi_image_free(data);
        }
        else
        {
            assert(false);
            stbi_image_free(data);
        }
}

CubeTexture::CubeTexture(
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