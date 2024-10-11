#include "Material.hpp"

std::shared_ptr<Material> Material::create_default_material()
{
	return std::make_shared<Material>();
}
