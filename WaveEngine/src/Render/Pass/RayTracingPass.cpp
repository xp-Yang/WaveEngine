#include "RayTracingPass.hpp"
#include "Logical/Framework/ECS/Components.hpp"
#include "Core/Math.hpp"

void RayTracingPass::init()
{
	m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_framebuffer->create({ AttachmentType::RGBA });
}

void RayTracingPass::prepare(FrameBuffer* framebuffer)
{
}

void RayTracingPass::draw()
{
	m_framebuffer->bind();
	m_framebuffer->clear();

	static RenderShaderObject* rt_shader = RenderShaderObject::getShaderObject(Asset::ShaderType::RayTracingShader);
	auto& world = ecs::World::get();
	auto camera = world.getMainCameraComponent();

	Viewport rt_viewport = Application::GetApp().getWindow()->getViewport(ViewportType::RayTracing).value_or(Viewport());

	{
		rt_shader->start_using();
		// camera
		rt_shader->setFloat3("camera.pos", camera->pos);
		rt_shader->setFloat("camera.distance", 1.0f/*camera->focal_length*/);
		rt_shader->setFloat("camera.fov", camera->fov);
		rt_shader->setFloat("camera.aspect_ratio", 16.0f / 9.0f); //TODO ��Ҫһ�ַ���viewport��Сֻ����������ʾ��Χ��������������Ҳ��Ӱ��fov�Ϳ��߱�

		rt_shader->setFloat3("camera.front", camera->direction);
		rt_shader->setFloat3("camera.right", camera->getRightDirection());
		rt_shader->setFloat3("camera.up", camera->getUpDirection());

		// debug
		//float width = tan(camera->fov / 2.0f) * camera->focal_length * 2.0;
		//float height = width / (16.0f / 9.0f);
		//Vec3 origin = camera->pos + camera->focal_length * camera->direction;
		//Vec3 leftbottom = camera->pos + camera->focal_length * camera->direction - width / 2.0f * camera->getRightDirection() - height / 2.0f * camera->getUpDirection();

		// random
		rt_shader->setFloat("randOrigin", 674764.0f * (Math::randomUnit() + 1.0f));
		// render to m_framebuffer
		Renderer::drawIndex(m_screen_quad->getVAO(), m_screen_quad->indicesCount());
	}
}

FrameBuffer* RayTracingPass::getFrameBuffer()
{
	return m_framebuffer.get();
}