#include "ImGuiEditor.hpp"

#include "Core/Logger.hpp"
#include "Platform/OS/FileDialog.hpp"

static std::string matrix_str(const Mat4 mat)
{
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].x, mat[1].x, mat[2].x, mat[3].x);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].y, mat[1].y, mat[2].y, mat[3].y);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].z, mat[1].z, mat[2].z, mat[3].z);
    result += buf;
    sprintf_s(buf, "%.3f %.3f %.3f %.3f \n", mat[0].w, mat[1].w, mat[2].w, mat[3].w);
    result += buf;
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
    //printf("%.3f %.3f %.3f %.3f \n", mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

static std::string vec3_str(const Vec3 vec) {
    std::string result;
    char buf[1024];
    sprintf_s(buf, "%.3f %.3f %.3f \n", vec.x, vec.y, vec.z);
    result += buf;
    sprintf_s(buf, "\n");
    result += buf;
    return result;
}

ImGuiEditor::ImGuiEditor()
    : world(ecs::World::get())
{
    m_motion = false;
    m_view_manager = std::make_unique<ViewRectManager>();
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

   renderGlobalMenu();
   renderSceneHierarchy();
   renderGlobalConsole();
   renderCameraController();

   m_view_manager->render();
   renderGizmos();

   updateRenderParams();
}

void ImGuiEditor::renderGlobalConsole() {
    ImGui::Begin("Console", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImVec2 dummy = ImGui::CalcTextSize("A");

    ImGui::Dummy(dummy);
    ImGui::PushItemWidth(150.0f);
    int path_type_option = (int)m_ref_render_system->getRenderPathType();
    std::array<std::string, 3> combo_strs = { "Forward", "Deferred", "RayTracing" };
    if (ImGui::BeginCombo("Render Path", combo_strs[path_type_option].c_str())) {
        for (int i = 0; i < combo_strs.size(); i++) {
            bool selected = path_type_option == i;
            if (ImGui::Selectable(combo_strs[i].c_str(), selected)) {
                m_ref_render_system->setRenderPathType(RenderPath::Type(i));
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();

    ImGui::Dummy(dummy);
    ImGui::Separator();
    ImGui::Dummy(dummy);

    ImGui::Text("Params:");
    if (m_ref_render_system->getRenderPathType() == RenderPath::Type::Forward) {
        ImGui::PushItemWidth(50.0f);
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
        ImGui::PopItemWidth();
    }

    ImGui::Checkbox("motion", &m_motion);
    ImGui::Checkbox("skybox", &m_render_params.skybox);
    ImGui::Checkbox("shadow", &m_render_params.shadow);
    //ImGui::Checkbox("reflection", &m_render_params.reflection);
    ImGui::Checkbox("normal", &m_render_params.normal_debug);

    ImGui::Dummy(dummy);
    ImGui::Separator();
    ImGui::Dummy(dummy);

    ImGui::Text("Choose Shader:");
    if (ImGui::RadioButton("Default", !m_render_params.wireframe && !m_render_params.checkerboard && !m_render_params.pbr)) {
        m_render_params.pbr = false;
        m_render_params.wireframe = false;
        m_render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("PBR", m_render_params.pbr)) {
        m_render_params.pbr = true;
        m_render_params.wireframe = false;
        m_render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Wireframe", m_render_params.wireframe)) {
        m_render_params.pbr = false;
        m_render_params.wireframe = true;
        m_render_params.checkerboard = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Checkerboard", m_render_params.checkerboard)) {
        m_render_params.pbr = false;
        m_render_params.wireframe = false;
        m_render_params.checkerboard = true;
    }
    //ImGui::SliderInt("pixel style", &m_render_params.pixelate_level, 1, 16);
    
    ImGui::Dummy(dummy);
    ImGui::Separator();
    ImGui::Dummy(dummy);

    ImGui::Text("Add/Delete point light:");
    auto scene_hierarchy = Application::GetApp().getSceneHierarchy();
    int point_light_count = scene_hierarchy->pointLightCount();
    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button((std::string("+") + "##pointLight").c_str())) {
        scene_hierarchy->addPointLight();
    }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::Button((std::string("-") + "##pointLight").c_str())) {
        int last_point_entity_id = -1;
        auto it = world.entityView<ecs::PointLightComponent>().begin();
        while(it != world.entityView<ecs::PointLightComponent>().end())
        {
            last_point_entity_id = (*it).getId();
            it++;
        }
        if (last_point_entity_id != -1)
            scene_hierarchy->removeObject(ecs::Entity(last_point_entity_id));
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("point light number: %d", point_light_count);

    ImGui::Dummy(dummy);
    ImGui::Separator();
    ImGui::Dummy(dummy);

    ImGui::Text("Add/Delete sphere:");
    int sphere_count = scene_hierarchy->sphereCount();
    ImGui::PushButtonRepeat(true);
    if (ImGui::Button((std::string("+") + "##sphere").c_str())) {
        scene_hierarchy->addSphere();
    }
    ImGui::SameLine(0.0f, spacing);
    if (ImGui::Button((std::string("-") + "##sphere").c_str())) {
        scene_hierarchy->removeSphere();
    }
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    ImGui::Text("sphere number: %d", sphere_count);

    ImGui::Dummy(dummy);
    ImGui::Separator();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::End();
}

void ImGuiEditor::renderGlobalMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                FileDialog* file_dlg = FileDialog::create();
                auto filepath = file_dlg->OpenFile("");
                if (!filepath.empty()) {
                    Application::GetApp().getSceneHierarchy()->loadModel(filepath);
                }
            }
            if (ImGui::MenuItem("Save As..", "Ctrl+S")) {
                FileDialog* file_dlg = FileDialog::create();
                file_dlg->SaveFile("");
            }
            ImGui::EndMenu();
        }
        //if (ImGui::BeginMenu("Edit"))
        //{
        //    if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
        //    if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
        //    ImGui::Separator();
        //    if (ImGui::MenuItem("Cut", "CTRL+X")) {}
        //    if (ImGui::MenuItem("Copy", "CTRL+C")) {}
        //    if (ImGui::MenuItem("Paste", "CTRL+V")) {}
        //    ImGui::EndMenu();
        //}
        ImGui::EndMainMenuBar();
    }
}

void ImGuiEditor::renderSceneHierarchyNode(GameObject* node)
{
    for (int i = 0; i < node->children().size(); i++)
    {
        auto child_node = node->children()[i];
        const auto& childe_entity = child_node->entity();

        static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        std::string node_name = world.getComponent<ecs::NameComponent>(childe_entity)->name;
        std::string display_text = node_name + " (Entity: " + std::to_string(childe_entity.getId()) + ")";
        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, display_text.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            bool close = false;
            for (const auto& picked_entity : world.getPickedEntities()) {
                if (picked_entity.getId() == childe_entity.getId()) {
                    world.removeComponent<ecs::PickedComponent>(childe_entity);
                    close = true;
                }
            }
            if (!close) {
                for (const auto& picked_entity : world.getPickedEntities()) {
                    world.removeComponent<ecs::PickedComponent>(picked_entity);
                }
                world.addComponent<ecs::PickedComponent>(childe_entity);
            }
        }
        if (node_open)
        {
            if (child_node->children().empty()) {
                // TODO display all components name_str belong to this entity
                ImGui::Text("Entity: %d", childe_entity);
                if (world.hasComponent<ecs::NameComponent>(childe_entity))
                    ImGui::Text("<NameComponent>");
                if (world.hasComponent<ecs::TransformComponent>(childe_entity))
                    ImGui::Text("<TransformComponent>");
                if (world.hasComponent<ecs::RenderableComponent>(childe_entity))
                    ImGui::Text("<RenderableComponent>");
                if (world.hasComponent<ecs::ExplosionComponent>(childe_entity))
                    ImGui::Text("<ExplosionComponent>");
                if (world.hasComponent<ecs::SkyboxComponent>(childe_entity))
                    ImGui::Text("<SkyboxComponent>");
                if (world.hasComponent<ecs::PointLightComponent>(childe_entity))
                    ImGui::Text("<PointLightComponent>");
                if (world.hasComponent<ecs::DirectionalLightComponent>(childe_entity))
                    ImGui::Text("<DirectionalLightComponent>");
                if (world.hasComponent<ecs::GroundComponent>(childe_entity))
                    ImGui::Text("<GroundComponent>");
            }
            else {
                renderSceneHierarchyNode(child_node);
            }
            ImGui::TreePop();
        }
    }
}

void ImGuiEditor::renderSceneHierarchy()
{
    ImGui::Begin(("Scene Hierarchy"), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    ImGuiWindow* scene_hierarchy_window = ImGui::GetCurrentWindow();
    float scene_hierarchy_window_width = ImGui::GetWindowWidth();

    auto scene_hierarchy = Application::GetApp().getSceneHierarchy();
    auto root_node = scene_hierarchy->rootObject();

    renderSceneHierarchyNode(root_node);

    ImGui::End();

    ImVec2 pos = ImVec2(scene_hierarchy_window->Pos.x + scene_hierarchy_window_width, scene_hierarchy_window->Pos.y);
    renderPickedEntityController(pos, world.getPickedEntities());
}

void ImGuiEditor::renderCameraController()
{
    ImGui::Begin("Main Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    auto& camera = *world.getMainCameraComponent();
    ImGui::NewLine();
    ImGui::Text("view matrix:");
    std::string test_view = matrix_str(camera.view);
    ImGui::Text(test_view.c_str());

    ImGui::NewLine();
    ImGui::Text("inverse view matrix:");
    std::string inverse_view = matrix_str(Inverse(camera.view));
    ImGui::Text(inverse_view.c_str());

    ImGui::NewLine();
    ImGui::Text("camera position:");
    std::string test_camera_pos = vec3_str(camera.pos);
    ImGui::Text(test_camera_pos.c_str());

    ImGui::NewLine();
    ImGui::Text("camera direction:");
    std::string test_camera_dir = vec3_str(camera.direction);
    ImGui::Text(test_camera_dir.c_str());
    ImGui::End();
}

void ImGuiEditor::renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities)
{
    if (picked_entities.empty())
        return;
    // TODO consider to edit materials in MaterialSystem
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
        ImGui::PushItemWidth(80.0f);
        ImGui::SliderFloat((std::string("##x") + "##" + obj_name).c_str(), &transform_component.translation.x, -30.0f, 30.0f);
        ImGui::SameLine();
        ImGui::SliderFloat((std::string("##y") + "##" + obj_name).c_str(), &transform_component.translation.y, 0.0f, 30.0f);
        ImGui::SameLine();
        ImGui::SliderFloat((std::string("xyz") + "##" + obj_name).c_str(), &transform_component.translation.z, -30.0f, 30.0f);
        ImGui::PopItemWidth();

        // log
        ImGui::NewLine();
        ImGui::Text("model matrix:");
        std::string matrix_str_ = matrix_str(transform_component.transform());
        ImGui::Text(matrix_str_.c_str());
    }
    if (world.hasComponent<ecs::RenderableComponent>(entity)) {
        auto renderable = world.getComponent<ecs::RenderableComponent>(entity);
        // 编辑对象材质属性
        //ecs::MaterialSystem::onUpdate();
        for (int i = 0; i < renderable->primitives.size(); i++) {
            auto& material = renderable->primitives[i].material;
            ImGui::SliderFloat((std::string("shininess") + "##" + obj_name).c_str(), &material.shininess, 64.0f, 256.0f);

            ImGui::PushItemWidth(80.0f);
            ImGui::SliderFloat((std::string("##albedo.x") + "##" + obj_name).c_str(), &material.albedo.x, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::SliderFloat((std::string("##albedo.y") + "##" + obj_name).c_str(), &material.albedo.y, 0.0f, 1.0f);
            ImGui::SameLine();
            ImGui::SliderFloat((std::string("albedo") + "##" + obj_name).c_str(), &material.albedo.z, 0.0f, 1.0f);
            ImGui::PopItemWidth();

            ImGui::SliderFloat((std::string("metallic") + "##" + obj_name).c_str(), &material.metallic, 0.0f, 1.0f);
            ImGui::SliderFloat((std::string("roughness") + "##" + obj_name).c_str(), &material.roughness, 0.01f, 1.0f);
            ImGui::SliderFloat((std::string("ao") + "##" + obj_name).c_str(), &material.ao, 0.0f, 1.0f);
        }
    }
    if (world.hasComponent<ecs::ExplosionComponent>(entity)) {
        auto& explosion = *world.getComponent<ecs::ExplosionComponent>(entity);
        ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion.explosionRatio, 0.0f, 1.0f);
    }
    if (world.hasComponent<ecs::SkyboxComponent>(entity))
        ImGui::Text("<SkyboxComponent>");
    if (world.hasComponent<ecs::PointLightComponent>(entity)) {
        Vec4& luminousColor = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
        float* radius = &world.getComponent<ecs::PointLightComponent>(entity)->radius;
        ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
        ImGui::SliderFloat((std::string("Radius") + "##" + obj_name).c_str(), radius, 5.0f, 50.0f);
    }
    if (world.hasComponent<ecs::DirectionalLightComponent>(entity)) {
        auto dir_light_component = world.getComponent<ecs::DirectionalLightComponent>(entity);
        dir_light_component->direction = -world.getComponent<ecs::TransformComponent>(entity)->translation;
        Vec4& luminousColor = dir_light_component->luminousColor;
        ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
    }
    if (world.hasComponent<ecs::GroundComponent>(entity))
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

    ecs::CameraComponent* camera = world.getMainCameraComponent();
    ecs::TransformComponent* transform_component = nullptr;
    auto picked_entities = world.getPickedEntities();
    for (auto entity : picked_entities) {
        transform_component = world.getComponent<ecs::TransformComponent>(entity);
        break;
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

    //if (camera) {
    //    Viewport viewport = Application::GetApp().getWindow()->getMainViewport().value_or(Viewport());
    //    viewport.transToScreenCoordinates();
    //    ImVec2 air_window_size = ImVec2(128, 128);
    //    float camDistance = 8.f;
    //    ImGuizmo::ViewManipulate((float*)(&camera->view), camDistance, ImVec2(viewport.x + viewport.width - air_window_size.x, viewport.y), air_window_size, 0x10101010);
    //}
}

void ImGuiEditor::updateRenderParams()
{
    m_ref_render_system->setRenderParams(m_render_params);
}
