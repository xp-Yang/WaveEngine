#include "RayTracingPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Core/Math.hpp"

void RayTracingPass::init()
{
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGB16F });
	m_screen_quad = Mesh::create_screen_mesh();
}

void RayTracingPass::prepare(FrameBuffer* framebuffer)
{
}

void RayTracingPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();

	static Shader* rt_shader = new Shader("resource/shader/RayTracing.vs", "resource/shader/RayTracing.fs");
	auto& world = ecs::World::get();
	ecs::CameraComponent* camera = nullptr;
	for (auto entity : world.entityView<ecs::CameraComponent>()) {
		camera = world.getComponent<ecs::CameraComponent>(entity);
	}

	{
		// ¼¤»î×ÅÉ«Æ÷
		rt_shader->start_using();
		// camera
		rt_shader->setFloat3("camera.pos", camera->pos);
		rt_shader->setFloat3("camera.front", camera->direction);
		rt_shader->setFloat3("camera.right", camera->getRightDirection());
		rt_shader->setFloat3("camera.up", camera->getUpDirection());
		float halfH = 1.0f;
		float halfW = halfH * 16.0f / 9.0f;
		rt_shader->setFloat("camera.halfH", halfH);
		rt_shader->setFloat("camera.halfW", halfW);
		Vec3 LeftBottomCorner = camera->direction - halfW * camera->getRightDirection() - halfH * camera->getUpDirection();
		rt_shader->setFloat3("camera.leftbottom", LeftBottomCorner);
		// random
		rt_shader->setFloat("randOrigin", 674764.0f * (MathUtils::randomUnit() + 1.0f));
		// render to m_framebuffer
		Renderer::drawIndex(*rt_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
	}
}

FrameBuffer* RayTracingPass::getFrameBuffer()
{
	return m_framebuffer.get();
}
