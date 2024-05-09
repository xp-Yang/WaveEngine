#include "Scene.hpp"

#include "Logical/Framework/Component/CameraComponent.hpp"
#include "Logical/Framework/Component/MeshComponent.hpp"
#include "Logical/Framework/Component/TransformComponent.hpp"

#include "ResourceManager/AssetManager.hpp"

static const std::string resource_dir = RESOURCE_DIR;

void Scene::load()
{
}

void Scene::save()
{
}

void Scene::init()
{
	//m_skybox;

	//size_t cubes_count = 36;
	//for (int i = 0; i < cubes_count; i++) {
	//	GObject* cube_obj = new GObject();
	//	MeshComponent& mesh = cube_obj->addComponent<MeshComponent>();
	//	SubMesh cube_sub_mesh;
	//	std::shared_ptr<Asset::Material> cube_material = std::make_shared<Asset::Material>();
	//	cube_material->set_diffuse_map(resource_dir + "/images/pure_white_map.png");
	//	cube_material->set_specular_map(resource_dir + "/images/cube_specular.png");
	//	cube_material->shader = Shader::getShader(ShaderType::PBRShader);
	//	cube_material->albedo = Vec3(1.0f);
	//	cube_material->metallic = 1.0;
	//	cube_material->roughness = 0.5;
	//	cube_material->ao = 0.01;
	//	cube_sub_mesh.mesh_ref = std::make_shared<Asset::Mesh>();
	//	*cube_sub_mesh.mesh_ref = Asset::Mesh::create_cube_mesh();
	//	cube_sub_mesh.transform = Mat4(1.0f);
	//	mesh.sub_meshes.push_back(cube_sub_mesh);

	//	TransformComponent& transform = cube_obj->addComponent<TransformComponent>();
	//	transform.translation = { 1.5f * (i % 6), 0.5f + 1.5f * (i / 6), -10.0f };
	//	m_objects.push_back(std::shared_ptr<GObject>(cube_obj));
	//}


	//GObject* plane_obj = new GObject();
	//MeshComponent& plane_mesh = plane_obj->addComponent<MeshComponent>();
	//SubMesh plane_sub_mesh;
	//std::shared_ptr<Asset::Material> plane_material = std::make_shared<Asset::Material>();
	//plane_material->shader = Shader::getShader(ShaderType::PBRShader);
	//plane_material->albedo = Vec3(1.0f, 1.0f, 1.0f);
	//plane_material->metallic = 0.0f;
	//plane_material->roughness = 1.0f;
	//plane_material->ao = 0.01;
	//plane_material->set_diffuse_map(resource_dir + "/images/pure_white_map.png");
	//plane_material->set_specular_map(resource_dir + "/images/pure_white_map.png");
	//plane_sub_mesh.material_ref = plane_material;
	//plane_sub_mesh.mesh_ref = std::make_shared<Asset::Mesh>();
	//*plane_sub_mesh.mesh_ref = Asset::Mesh::create_ground_mesh(Vec2(60.0f));
	//plane_sub_mesh.transform = Mat4(1.0f);
	//plane_mesh.sub_meshes.push_back(plane_sub_mesh);

	//TransformComponent& plane_transform = plane_obj->addComponent<TransformComponent>();

	//m_objects.push_back(std::shared_ptr<GObject>(plane_obj));
}

void Scene::tick(float delta_time)
{
	for (const auto& obj : m_objects) {
		obj->tick(delta_time);
	}
}
