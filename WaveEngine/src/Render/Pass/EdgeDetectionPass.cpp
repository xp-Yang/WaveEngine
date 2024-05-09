#include "EdgeDetectionPass.hpp"
#include "Logical/Framework/ECS/Components.hpp"
#include "Platform/RHI/rhi.hpp"

EdgeDetectionPass::EdgeDetectionPass()
{
}

void EdgeDetectionPass::init()
{
    m_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_framebuffer->create({ AttachmentType::RGB16F });

    m_source_framebuffer = std::make_unique<FrameBuffer>(WINDOW_WIDTH, WINDOW_HEIGHT);
    m_source_framebuffer->create({ AttachmentType::RGB16F, AttachmentType::DEPTH });
}

void EdgeDetectionPass::prepare(FrameBuffer* framebuffer)
{
    m_source_framebuffer->bind();
    m_source_framebuffer->clear();

    auto& world = ecs::World::get();
    if (world.getPickedEntities().empty())
        return;
    for (auto picked_entity : world.getPickedEntities()) {
        // render the picked one
        // TODO 需要先把实际的深度贴图拷贝到当前深度缓冲中
        glEnable(GL_DEPTH_TEST);

        Shader* picking_shader = Shader::getShader(ShaderType::PickingShader);
        picking_shader->start_using();
        ecs::CameraComponent& camera = *world.getMainCameraComponent();
        picking_shader->setMatrix("view", 1, camera.view);
        picking_shader->setMatrix("projection", 1, camera.projection);
        if (world.getComponent<ecs::RenderableComponent>(picked_entity)) {
            auto& renderable = *world.getComponent<ecs::RenderableComponent>(picked_entity);
            auto& model_matrix = *world.getComponent<ecs::TransformComponent>(picked_entity);
            picking_shader->setMatrix("model", 1, model_matrix.transform());
            for (int i = 0; i < renderable.sub_meshes.size(); i++) {
                auto& mesh = renderable.sub_meshes[i].mesh;
                int id = picked_entity.getId() * 50000;// for debugging
                int r = (id & 0x000000FF) >> 0;
                int g = (id & 0x0000FF00) >> 8;
                int b = (id & 0x00FF0000) >> 16;
                Color4 color(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
                picking_shader->setFloat4("picking_color", color);
                Renderer::drawIndex(*picking_shader, mesh.get_VAO(), mesh.get_indices_count());
            }
        }
    }
    m_source_map = m_source_framebuffer->getFirstAttachmentOf(AttachmentType::RGB16F).getMap();
}

void EdgeDetectionPass::draw()
{
    m_framebuffer->bind();
    m_framebuffer->clear();

    Shader* edge_shader = Shader::getShader(ShaderType::EdgeDetection);
    edge_shader->start_using();
    edge_shader->setTexture("Texture", 0, m_source_map);
    Renderer::drawIndex(*edge_shader, m_screen_quad->getVAO(), m_screen_quad->indicesCount());
}

FrameBuffer* EdgeDetectionPass::getFrameBuffer()
{
    return m_framebuffer.get();
}
