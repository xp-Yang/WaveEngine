#include "RayTracingPass.hpp"
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"
#include "Core/Math.hpp"
#include "Application_impl.hpp"

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

	Shader* rt_shader = Shader::getShader(ShaderType::RayTracingShader);
	auto& world = ecs::World::get();
	ecs::CameraComponent* camera = world.getMainCameraComponent();

	Viewport rt_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::RayTracing).value_or(Viewport());

	{
		rt_shader->start_using();
		// camera
		rt_shader->setFloat3("camera.pos", camera->pos);
		rt_shader->setFloat("camera.distance", 1.0f/*camera->focal_length*/);
		rt_shader->setFloat("camera.fov", camera->fov);
		rt_shader->setFloat("camera.aspect_ratio", 16.0f / 9.0f); //TODO 需要一种方法viewport大小只调整纹理显示范围，不会拉伸纹理也不影响fov和宽高比

		rt_shader->setFloat3("camera.front", camera->direction);
		rt_shader->setFloat3("camera.right", camera->getRightDirection());
		rt_shader->setFloat3("camera.up", camera->getUpDirection());

		// debug
		//float width = tan(camera->fov / 2.0f) * camera->focal_length * 2.0;
		//float height = width / (16.0f / 9.0f);
		//Vec3 origin = camera->pos + camera->focal_length * camera->direction;
		//Vec3 leftbottom = camera->pos + camera->focal_length * camera->direction - width / 2.0f * camera->getRightDirection() - height / 2.0f * camera->getUpDirection();

		// random
		rt_shader->setFloat("randOrigin", 674764.0f * (Core::randomUnit() + 1.0f));
		// render to m_framebuffer
		Renderer::drawIndex(*rt_shader, m_screen_quad.get_VAO(), m_screen_quad.get_indices_count());
	}
}

FrameBuffer* RayTracingPass::getFrameBuffer()
{
	return m_framebuffer.get();
}
