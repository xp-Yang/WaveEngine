#include "RenderPipeline.hpp"
#include "MainCameraPass.hpp"
#include "ShadowPass.hpp"
#include "PickingPass.hpp"
#include "ScreenPass.hpp"
#include "UIPass.hpp"
#include "../../../imgui/imgui.h"

void RenderPipeline::init()
{
    m_main_camera_pass = std::make_shared<MainCameraPass>();
    m_shadow_pass = std::make_shared<ShadowPass>();
    m_picking_pass = std::make_shared<PickingPass>();
    m_screen_pass = std::make_shared<ScreenPass>();
    m_ui_pass = std::make_shared<UIPass>();
    m_main_camera_pass->init();
    m_shadow_pass->init();
    m_picking_pass->init();
    m_screen_pass->init();
    m_ui_pass->init();
}

void RenderPipeline::render()
{
    m_shadow_pass->prepare_data();
    m_shadow_pass->draw();
    if (!ImGui::GetIO().WantCaptureMouse) {
        m_picking_pass->prepare_data();
        m_picking_pass->draw();
    }
    m_main_camera_pass->prepare_data(m_shadow_pass->get_fbo(), m_shadow_pass->get_map());
    m_main_camera_pass->draw();
    m_screen_pass->prepare_data(m_main_camera_pass->get_fbo(), m_main_camera_pass->get_map());
    m_screen_pass->draw();
    m_ui_pass->draw();
}
