#include "ImGuiEditor.hpp"
#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuizmo.h>
#include "GamePlay/Framework/ECS/Components.hpp"
#include "Core/Logger.hpp"
#include "Platform/OS/FileDialog.hpp"
#include "GamePlay/FrameWork/SceneHierarchy.hpp"

ImGuiEditor::ImGuiEditor()
{
    motion = true;
    global_ambient_strength = 0.1f;
    icosphere_accuracy = 8;

    // TODO: editor依赖system，能操控渲染管线，能操控运动，能编辑transform，能编辑mesh，能编辑material...
}

void ImGuiEditor::init(RenderSystem* render_system)
{
    m_ref_render_system = render_system;
}

void ImGuiEditor::render()
{
   m_ref_render_system->onUpdate();

   ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
   renderGlobalController();
   renderSceneHierarchy();
   renderCameraController();

   renderMainViewWindow();
   renderPickingViewWindow();
   renderShadowViewWindow();
   renderRayTracingViewWindow();
   updateRenderParams();
}

void ImGuiEditor::renderMainViewWindow()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(1280, 720 + 20), ImGuiCond_Appearing);
    if (ImGui::Begin("MainView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y + 20, (int)window_size.x, (int)window_size.y - 20, Viewport::Coordinates::ScreenCoordinates };
        ImGuizmo::SetOrthographic(true);
        ImGuizmo::SetDrawlist();
        // ImGuizmo的绘制范围是main viewport
        ImGuizmo::SetRect(viewport.x, viewport.y, viewport.width, viewport.height);
        Application::GetApp().getWindow()->setMainViewport(viewport.transToGLCoordinates());
        renderGizmos();
    }
    ImGui::End();
}

void ImGuiEditor::renderPickingViewWindow()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(400, 225), ImGuiCond_Appearing);
    if (ImGui::Begin("PickingView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = ImGui::GetIO().WantPassThroughMouse || hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y, Viewport::Coordinates::ScreenCoordinates };
        Application::GetApp().getWindow()->setViewport(ViewportType::Pick, viewport.transToGLCoordinates());
    }
    ImGui::End();
}

void ImGuiEditor::renderShadowViewWindow()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(400, 225), ImGuiCond_Appearing);
    if (ImGui::Begin("ShadowView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y, Viewport::Coordinates::ScreenCoordinates };
        Application::GetApp().getWindow()->setViewport(ViewportType::Shadow, viewport.transToGLCoordinates());
    }
    ImGui::End();
}

void ImGuiEditor::renderRayTracingViewWindow()
{
    static ImGuiWindowFlags window_flags = 0;
    ImGui::SetNextWindowSize(ImVec2(800, 450), ImGuiCond_Appearing);
    if (ImGui::Begin("RayTracingView", nullptr, window_flags | ImGuiWindowFlags_NoBackground)) {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        bool hovered_window = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);
        window_flags = hovered_window ? ImGuiWindowFlags_NoMove : 0;
        ImGui::GetIO().WantPassThroughMouse = ImGui::GetIO().WantPassThroughMouse || hovered_window && !ImGuizmo::IsUsing();
        ImVec2 window_pos = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();
        Viewport viewport = { (int)window_pos.x, (int)window_pos.y, (int)window_size.x, (int)window_size.y, Viewport::Coordinates::ScreenCoordinates };
        Application::GetApp().getWindow()->setViewport(ViewportType::RayTracing, viewport.transToGLCoordinates());
    }
    ImGui::End();
}

void ImGuiEditor::renderGlobalController() {
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                FileDialog* file_dlg = FileDialog::create();
                file_dlg->OpenFile("");
            }
            if (ImGui::MenuItem("Save As..", "Ctrl+S")) {
                FileDialog* file_dlg = FileDialog::create();
                file_dlg->SaveFile("");
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Global Controller", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    static unsigned int pipeline_type_option = (int)m_render_params.pipeline_type;
    if (ImGui::BeginCombo("Render Pipeline Type", pipeline_type_option == 0 ? "Forward" : "Deferred")) {
        for (int i = 0; i < 2; i++) {
            bool selected = pipeline_type_option == i;
            std::string label = i == 0 ? "Forward" : "Deferred";
            if (ImGui::Selectable(label.c_str(), selected)) {
                pipeline_type_option = i;
                m_render_params.pipeline_type = (PIPELINE_TYPE)pipeline_type_option;
            }
        }
        ImGui::EndCombo();
    }
    
    static unsigned int curr_item = 1;
    if (ImGui::BeginCombo("MSAA", (std::to_string((int)std::pow(4, curr_item)) + "x").c_str())) {
        for (int i = 0; i < 3; i++) {
            bool selected = curr_item == i;
            std::string label = std::to_string((int)std::pow(4, i)) + "x";
            if (ImGui::Selectable(label.c_str(), selected)) {
                curr_item = i;
                m_render_params.msaa_sample_count = (int)std::pow(4, i);
            }
        }
        ImGui::EndCombo();
    }
    static unsigned int shadow_curr_item = 0;
    if (ImGui::BeginCombo("Shadow Map Resolution", (std::to_string((int)std::pow(4, shadow_curr_item)) + "x").c_str())) {
        for (int i = 0; i < 3; i++) {
            bool selected = shadow_curr_item == i;
            std::string label = std::to_string((int)std::pow(4, i)) + "x";
            if (ImGui::Selectable(label.c_str(), selected)) {
                shadow_curr_item = i;
                m_render_params.shadow_map_sample_count = (int)std::pow(4, i);
            }
        }
        ImGui::EndCombo();
    }
    ImGui::Checkbox("motion", &motion);
    ImGui::Checkbox("shadow", &m_render_params.shadow);
    ImGui::Checkbox("reflection", &m_render_params.reflection);
    ImGui::Checkbox("normal", &m_render_params.normal_debug);
    ImGui::Checkbox("wireframe", &m_render_params.wireframe);
    //ImGui::SliderInt("pixel style", &m_render_params.pixelate_level, 1, 16);
    
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}

void ImGuiEditor::renderSceneHierarchy()
{
    ImGui::Begin(("Scene Hierarchy"), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGuiWindow* scene_hierarchy_window = ImGui::GetCurrentWindow();
    float scene_hierarchy_window_width = ImGui::GetWindowWidth();

    auto& world = ecs::World::get();
    auto scene_hierarchy = Application::GetApp().getSceneHierarchy();
    auto scene_root = scene_hierarchy->rootObject();
    static const ecs::Entity* picked_entity = nullptr;
    for (int i = 0; i < scene_root->children().size(); i++)
    {
        auto child = scene_root->children()[i];
        const auto& entity = child->entity();
        auto object_name = world.getComponent<ecs::NameComponent>(entity)->name;

        static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, object_name.c_str(), i);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            if (picked_entity && picked_entity == &entity) {
                picked_entity = nullptr;
            }
            else {
                for (const auto& picked_entity : world.getPickedEntities()) {
                    world.removeComponent<ecs::PickedComponent>(picked_entity);
                }
                world.addComponent<ecs::PickedComponent>(entity);
                picked_entity = &entity;
            }
        }
        if (node_open)
        {
            // TODO display all components name_str belong to this entity
            if (world.hasComponent<ecs::NameComponent>(entity))
                ImGui::Text("<NameComponent>");
            if (world.hasComponent<ecs::TransformComponent>(entity))
                ImGui::Text("<TransformComponent>");
            if (world.hasComponent<ecs::RenderableComponent>(entity))
                ImGui::Text("<RenderableComponent>");
            if (world.hasComponent<ecs::ExplosionComponent>(entity))
                ImGui::Text("<ExplosionComponent>");
            if (world.hasComponent<ecs::SkyboxComponent>(entity))
                ImGui::Text("<SkyboxComponent>");
            if (world.hasComponent<ecs::PointLightComponent>(entity))
                ImGui::Text("<PointLightComponent>");
            if (world.hasComponent<ecs::DirectionalLightComponent>(entity))
                ImGui::Text("<DirectionalLightComponent>");
            if (world.hasComponent<ecs::BaseGridGroundComponent>(entity))
                ImGui::Text("<BaseGridGroundComponent>");
            ImGui::TreePop();
        }
    }

    ImGui::End();

    if (picked_entity) {
        ImVec2 pos = ImVec2(scene_hierarchy_window->Pos.x + scene_hierarchy_window_width, scene_hierarchy_window->Pos.y);
        renderPickedEntityController(pos, { *picked_entity });
    }
}

void ImGuiEditor::renderCameraController()
{
    ImGui::Begin("Main Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    auto& world = ecs::World::get();
    auto& camera = *world.getMainCameraComponent();
    ImGui::NewLine();
    ImGui::Text("view matrix:");
    std::string test_view = matrix_log(camera.view);
    ImGui::Text(test_view.c_str());

    ImGui::NewLine();
    ImGui::Text("inverse view matrix:");
    std::string inverse_view = matrix_log(Inverse(camera.view));
    ImGui::Text(inverse_view.c_str());

    ImGui::NewLine();
    ImGui::Text("camera position:");
    std::string test_camera_pos = vec3_log(camera.pos);
    ImGui::Text(test_camera_pos.c_str());

    ImGui::NewLine();
    ImGui::Text("camera direction:");
    std::string test_camera_dir = vec3_log(camera.direction);
    ImGui::Text(test_camera_dir.c_str());
    ImGui::End();
}

void ImGuiEditor::renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities)
{
    // TODO consider to edit materials in MaterialSystem
    auto& world = ecs::World::get();
    auto entity = picked_entities[0];
    if (!world.hasComponent<ecs::NameComponent>(entity))
        return;
    std::string obj_name = world.getComponent<ecs::NameComponent>(entity)->name;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    ImGui::Begin((obj_name).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    if (world.hasComponent<ecs::TransformComponent>(entity)) {
        auto& transform_component = *world.getComponent<ecs::TransformComponent>(entity);
        if (ImGui::RadioButton("Translate", m_toolbar_type == ToolbarType::Translate))
            m_toolbar_type = ToolbarType::Translate;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", m_toolbar_type == ToolbarType::Rotate))
            m_toolbar_type = ToolbarType::Rotate;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", m_toolbar_type == ToolbarType::Scale))
            m_toolbar_type = ToolbarType::Scale;
        ImGui::PushItemWidth(85.0f);
        ImGui::SliderFloat((std::string("##x") + "##" + obj_name).c_str(), &transform_component.translation.x, -10.0f, 10.0f);
        ImGui::SameLine();
        ImGui::SliderFloat((std::string("##y") + "##" + obj_name).c_str(), &transform_component.translation.y, -10.0f, 10.0f);
        ImGui::SameLine();
        ImGui::SliderFloat((std::string("xyz") + "##" + obj_name).c_str(), &transform_component.translation.z, -10.0f, 10.0f);
        ImGui::PopItemWidth();

        // log
        ImGui::NewLine();
        ImGui::Text("model matrix:");
        std::string matrix_str = matrix_log(transform_component.transform());
        ImGui::Text(matrix_str.c_str());
    }
    if (world.hasComponent<ecs::RenderableComponent>(entity)) {
        auto renderable = world.getComponent<ecs::RenderableComponent>(entity);
        float shininess = renderable->primitives[0].material.shininess;
        ImGui::SliderFloat((std::string("shininess") + "##" + obj_name).c_str(), &shininess, 64.0f, 256.0f);
        // 编辑对象材质属性
        //ecs::MaterialSystem::onUpdate();
        for (int i = 0; i < renderable->primitives.size(); i++) {
            auto& material = renderable->primitives[i].material;
            //material.materials[i].ambient_strength;
            material.shininess = shininess;
            //curr_entity->material(i).set_diffuse_map();
            //curr_entity->material(i).set_specular_map();
        }
    }
    if (world.hasComponent<ecs::ExplosionComponent>(entity)) {
        // 编辑对象的explosion ratio
        auto& explosion = *world.getComponent<ecs::ExplosionComponent>(entity);
        ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion.explosionRatio, 0.0f, 1.0f);
    }
    if (world.hasComponent<ecs::SkyboxComponent>(entity))
        ImGui::Text("<SkyboxComponent>");
    if (world.hasComponent<ecs::PointLightComponent>(entity)) {
        Vec4& luminousColor = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
        ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
    }
    if (world.hasComponent<ecs::DirectionalLightComponent>(entity)) {
        Vec4& luminousColor = world.getComponent<ecs::DirectionalLightComponent>(entity)->luminousColor;
        ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
    }
    if (world.hasComponent<ecs::BaseGridGroundComponent>(entity))
        ;

    ImGui::End();
}

void ImGuiEditor::renderGizmos()
{
    ImGuizmo::OPERATION imguizmo_operation;
    switch (m_toolbar_type)
    {
    case ToolbarType::Translate:
        imguizmo_operation = ImGuizmo::TRANSLATE;
        break;
    case ToolbarType::Rotate:
        imguizmo_operation = ImGuizmo::ROTATE;
        break;
    case ToolbarType::Scale:
        imguizmo_operation = ImGuizmo::SCALE;
        break;
    default:
        break;
    }

    auto& world = ecs::World::get();
    ecs::CameraComponent* camera = world.getMainCameraComponent();
    ecs::TransformComponent* transform_component = nullptr;
    auto picked_entities = world.getPickedEntities();
    for (auto entity : picked_entities) {
        if (!world.hasComponent<ecs::BaseGridGroundComponent>(entity)) {
            transform_component = world.getComponent<ecs::TransformComponent>(entity);
            break;
        }
    }

    if (camera && transform_component) {
        Mat4 model_matrix = transform_component->transform();
        ImGuizmo::Manipulate((float*)(&camera->view), (float*)(&camera->projection), imguizmo_operation, ImGuizmo::LOCAL, (float*)(&model_matrix), NULL, NULL, NULL, NULL);
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents((float*)&model_matrix, matrixTranslation, matrixRotation, matrixScale);
        transform_component->translation = Vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
        transform_component->scale = Vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
        transform_component->rotation = Vec3(matrixRotation[0], matrixRotation[1], matrixRotation[2]);
    }

    if (camera) {
        Viewport viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
        viewport.transToScreenCoordinates();
        ImVec2 air_window_size = ImVec2(128, 128);
        float camDistance = 8.f;
        ImGuizmo::ViewManipulate((float*)(&camera->view), camDistance, ImVec2(viewport.x + viewport.width - air_window_size.x, viewport.y), air_window_size, 0x10101010);
    }
}

void ImGuiEditor::updateRenderParams()
{
    m_ref_render_system->setRenderParams(m_render_params);
}
