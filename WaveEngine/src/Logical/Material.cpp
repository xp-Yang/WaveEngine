#include "Material.hpp"

std::shared_ptr<Material> Material::create_default_material()
{
	auto default_material = std::make_shared<Material>();
	return default_material;
}
