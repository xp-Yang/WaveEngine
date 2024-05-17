#include "ImGuiEditor.hpp"

#include "Core/Utils/Utils.hpp"
#include "Core/Logger/Logger.hpp"
#include "Platform/OS/FileDialog.hpp"

ImGuiEditor::ImGuiEditor()
{
    configUIStyle();
    m_view_manager = std::make_unique<ViewRectManager>();

    m_gui_creator["TransformComponent"] = [this](const std::string& name, void* value_ptr) -> void {
        auto DrawVecControl = [](const std::string & label, Vec3 & values, float resetValue = 0.0f, float columnWidth = 100.0f)
        {
            ImGui::PushID(label.c_str());

            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, columnWidth);
            ImGui::Text("%s", label.c_str());
            ImGui::NextColumn();

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float  lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            if (ImGui::Button("X", buttonSize))
                values.x = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.55f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.45f, 0.2f, 1.0f });
            if (ImGui::Button("Y", buttonSize))
                values.y = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            if (ImGui::Button("Z", buttonSize))
                values.z = resetValue;
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();

            ImGui::Columns(1);
            ImGui::PopID();
        };

        {
            TransformComponent* trans_ptr = static_cast<TransformComponent*>(value_ptr);

            DrawVecControl("Position", trans_ptr->translation);
            DrawVecControl("Rotation", trans_ptr->rotation);
            DrawVecControl("Scale", trans_ptr->scale);
        }
    };
    m_gui_creator["Vec3"] = [this](const std::string& name, void* value_ptr) -> void {
        Vec3* vec_ptr = static_cast<Vec3*>(value_ptr);
        float    val[3] = { vec_ptr->x, vec_ptr->y, vec_ptr->z };

        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::DragFloat3(full_label.c_str(), val);
        }
        vec_ptr->x = val[0];
        vec_ptr->y = val[1];
        vec_ptr->z = val[2];
    };
    m_gui_creator["bool"] = [this](const std::string& name, void* value_ptr)  -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", name.c_str());
            ImGui::Checkbox(full_label.c_str(), static_cast<bool*>(value_ptr));
        }
    };
    m_gui_creator["int"] = [this](const std::string& name, void* value_ptr) -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::InputInt(full_label.c_str(), static_cast<int*>(value_ptr));
        }
    };
    m_gui_creator["float"] = [this](const std::string& name, void* value_ptr) -> void {
        {
            std::string full_label = "##" + name;
            ImGui::Text("%s", (name + ":").c_str());
            ImGui::InputFloat(full_label.c_str(), static_cast<float*>(value_ptr));
        }
    };
}

void ImGuiEditor::init(RenderSystem* render_system)
{
    m_ref_render_system = render_system;
}

void ImGuiEditor::onUpdate()
{
    renderGlobalMenu();
    renderEmptyMainDockerSpaceWindow();

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

    // TODO
    //ImGui::Text("Add/Delete point light:");
    //auto scene_hierarchy = Application::GetApp().getScene();
    //int point_light_count = scene_hierarchy->pointLightCount();
    //float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
    //ImGui::PushButtonRepeat(true);
    //if (ImGui::Button((std::string("+") + "##pointLight").c_str())) {
    //    scene_hierarchy->addPointLight();
    //}
    //ImGui::SameLine(0.0f, spacing);
    //if (ImGui::Button((std::string("-") + "##pointLight").c_str())) {
    //    int last_point_entity_id = -1;
    //    auto it = world.entityView<ecs::PointLightComponent>().begin();
    //    while(it != world.entityView<ecs::PointLightComponent>().end())
    //    {
    //        last_point_entity_id = (*it).getId();
    //        it++;
    //    }
    //    if (last_point_entity_id != -1)
    //        scene_hierarchy->removeObject(ecs::Entity(last_point_entity_id));
    //}
    //ImGui::PopButtonRepeat();
    //ImGui::SameLine();
    //ImGui::Text("point light number: %d", point_light_count);

    //ImGui::Dummy(dummy);
    //ImGui::Separator();
    //ImGui::Dummy(dummy);

    //ImGui::Text("Add/Delete sphere:");
    //int sphere_count = scene_hierarchy->sphereCount();
    //ImGui::PushButtonRepeat(true);
    //if (ImGui::Button((std::string("+") + "##sphere").c_str())) {
    //    scene_hierarchy->addSphere();
    //}
    //ImGui::SameLine(0.0f, spacing);
    //if (ImGui::Button((std::string("-") + "##sphere").c_str())) {
    //    scene_hierarchy->removeSphere();
    //}
    //ImGui::PopButtonRepeat();
    //ImGui::SameLine();
    //ImGui::Text("sphere number: %d", sphere_count);

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
                    // TODO Application::GetApp().getScene()->loadModel(filepath);
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

void ImGuiEditor::renderEmptyMainDockerSpaceWindow()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
        ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::SetNextWindowPos(ImVec2(0, 18.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT), ImGuiCond_Always);
    ImGui::Begin("Main Dock Space Window", nullptr, window_flags);
    ImGuiID main_docking_id = ImGui::GetID("Main Docking");
    ImGui::DockSpace(main_docking_id);
    ImGui::End();

    ImGui::PopStyleVar(3);
}

void ImGuiEditor::renderSceneHierarchyNode(GObject* node)
{
    //for (int i = 0; i < node->children().size(); i++)
    //{
    //    auto child_node = node->children()[i];
    //    const auto& childe_entity = child_node->entity();

    //    static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    //    for (const auto& picked_entity : world.getPickedEntities()) {
    //        if (picked_entity == childe_entity)
    //            node_flags |= ImGuiTreeNodeFlags_Selected;
    //        else
    //            node_flags &= ~ImGuiTreeNodeFlags_Selected;
    //    }
    //    std::string node_name = world.getComponent<ecs::NameComponent>(childe_entity)->name;
    //    std::string display_text = node_name + " (Entity: " + std::to_string(childe_entity.getId()) + ")";
    //    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, display_text.c_str());
    //    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
    //        bool close = false;
    //        for (const auto& picked_entity : world.getPickedEntities()) {
    //            if (picked_entity.getId() == childe_entity.getId()) {
    //                world.removeComponent<ecs::PickedComponent>(childe_entity);
    //                close = true;
    //            }
    //        }
    //        if (!close) {
    //            for (const auto& picked_entity : world.getPickedEntities()) {
    //                world.removeComponent<ecs::PickedComponent>(picked_entity);
    //            }
    //            world.addComponent<ecs::PickedComponent>(childe_entity);
    //        }
    //    }
    //    if (node_open)
    //    {
    //        if (child_node->children().empty()) {
    //            // TODO display all components name_str belong to this entity
    //            ImGui::Text("Entity: %d", childe_entity);
    //            if (world.hasComponent<ecs::NameComponent>(childe_entity))
    //                ImGui::Text("<NameComponent>");
    //            if (world.hasComponent<ecs::TransformComponent>(childe_entity))
    //                ImGui::Text("<TransformComponent>");
    //            if (world.hasComponent<ecs::RenderableComponent>(childe_entity))
    //                ImGui::Text("<RenderableComponent>");
    //            if (world.hasComponent<ecs::ExplosionComponent>(childe_entity))
    //                ImGui::Text("<ExplosionComponent>");
    //            if (world.hasComponent<ecs::SkyboxComponent>(childe_entity))
    //                ImGui::Text("<SkyboxComponent>");
    //            if (world.hasComponent<ecs::PointLightComponent>(childe_entity))
    //                ImGui::Text("<PointLightComponent>");
    //            if (world.hasComponent<ecs::DirectionalLightComponent>(childe_entity))
    //                ImGui::Text("<DirectionalLightComponent>");
    //        }
    //        else {
    //            renderSceneHierarchyNode(child_node);
    //        }
    //        ImGui::TreePop();
    //    }
    //}

    for (int i = 0; i < node->children().size(); i++)
    {
        auto child_node = node->children()[i];
        GObjectID child_id = child_node->ID();
        std::string child_name = child_node->name();
        std::string display_text = child_name + " (ID: " + std::to_string(child_id.id) + ")";

        static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        //for (const auto& picked_entity : world.getPickedEntities()) {
        //    if (picked_entity == childe_entity)
        //        node_flags |= ImGuiTreeNodeFlags_Selected;
        //    else
        //        node_flags &= ~ImGuiTreeNodeFlags_Selected;
        //}

        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, display_text.c_str());
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            // set gobject selected
        }
        if (node_open)
        {
            if (child_node->children().empty()) {
                for (auto& com : child_node->getComponents()) {
                    // TODO display all components name_str belong to this gobject
                    auto refl = Meta::ReflectionInstance(Meta::MetaObjectOf(com->typeName()), com.get());
                    for (int i = 0; i < refl.fieldCount(); i++) {
                        auto field = refl.field(i);
                        std::string field_type_name = field.field_type_name;
                        std::string field_name = field.field_name;
                        if (m_gui_creator.find(field_type_name) != m_gui_creator.end())
                            m_gui_creator[field_type_name](field_name, refl.getFieldValue(i));
                    }
                }
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

    auto scene_hierarchy = Application::GetApp().getScene();
    auto root_node = scene_hierarchy->rootObject();
    renderSceneHierarchyNode(root_node);

    ImGui::End();

    ImVec2 pos = ImVec2(scene_hierarchy_window->Pos.x + scene_hierarchy_window->Size.x, scene_hierarchy_window->Pos.y);
    //renderPickedEntityController(pos, world.getPickedEntities());
}

void ImGuiEditor::renderCameraController()
{
    ImGui::Begin("Main Camera", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    auto& camera = Application::GetApp().getScene()->getMainCamera();
    ImGui::NewLine();
    ImGui::Text("view matrix:");
    std::string test_view = Utils::mat4ToStr(camera.view);
    ImGui::Text(test_view.c_str());

    ImGui::NewLine();
    ImGui::Text("inverse view matrix:");
    std::string inverse_view = Utils::mat4ToStr(Inverse(camera.view));
    ImGui::Text(inverse_view.c_str());

    ImGui::NewLine();
    ImGui::Text("camera position:");
    std::string test_camera_pos = Utils::vec3ToStr(camera.pos);
    ImGui::Text(test_camera_pos.c_str());

    ImGui::NewLine();
    ImGui::Text("camera direction:");
    std::string test_camera_dir = Utils::vec3ToStr(camera.direction);
    ImGui::Text(test_camera_dir.c_str());
    ImGui::End();
}

void ImGuiEditor::renderPickedEntityController(const ImVec2& pos, const std::vector<ecs::Entity>& picked_entities)
{
    //if (picked_entities.empty())
    //    return;
    //// TODO consider to edit materials in MaterialSystem
    //auto entity = picked_entities[0];
    //if (!world.hasComponent<ecs::NameComponent>(entity))
    //    return;
    //std::string obj_name = world.getComponent<ecs::NameComponent>(entity)->name;
    //ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    //ImGui::Begin((obj_name).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    //ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

    //if (world.hasComponent<ecs::TransformComponent>(entity)) {
    //    auto& transform_component = *world.getComponent<ecs::TransformComponent>(entity);
    //    if (ImGui::RadioButton("Translate", m_toolbar_type == ToolbarType::Translate))
    //        m_toolbar_type = ToolbarType::Translate;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Rotate", m_toolbar_type == ToolbarType::Rotate))
    //        m_toolbar_type = ToolbarType::Rotate;
    //    ImGui::SameLine();
    //    if (ImGui::RadioButton("Scale", m_toolbar_type == ToolbarType::Scale))
    //        m_toolbar_type = ToolbarType::Scale;
    //    ImGui::PushItemWidth(80.0f);
    //    ImGui::SliderFloat((std::string("##x") + "##" + obj_name).c_str(), &transform_component.translation.x, -30.0f, 30.0f);
    //    ImGui::SameLine();
    //    ImGui::SliderFloat((std::string("##y") + "##" + obj_name).c_str(), &transform_component.translation.y, 0.0f, 30.0f);
    //    ImGui::SameLine();
    //    ImGui::SliderFloat((std::string("xyz") + "##" + obj_name).c_str(), &transform_component.translation.z, -30.0f, 30.0f);
    //    ImGui::PopItemWidth();

    //    // log
    //    ImGui::NewLine();
    //    ImGui::Text("model matrix:");
    //    std::string matrix_str_ = Utils::mat4ToStr(transform_component.transform());
    //    ImGui::Text(matrix_str_.c_str());
    //}
    //if (world.hasComponent<ecs::RenderableComponent>(entity)) {
    //    auto renderable = world.getComponent<ecs::RenderableComponent>(entity);
    //    // 编辑对象材质属性
    //    for (int i = 0; i < renderable->sub_meshes.size(); i++) {
    //        auto& material = renderable->sub_meshes[i].material;

    //        ImGui::PushItemWidth(80.0f);
    //        ImGui::SliderFloat((std::string("##albedo.x") + "##" + obj_name).c_str(), &material.albedo.x, 0.0f, 1.0f);
    //        ImGui::SameLine();
    //        ImGui::SliderFloat((std::string("##albedo.y") + "##" + obj_name).c_str(), &material.albedo.y, 0.0f, 1.0f);
    //        ImGui::SameLine();
    //        ImGui::SliderFloat((std::string("albedo") + "##" + obj_name).c_str(), &material.albedo.z, 0.0f, 1.0f);
    //        ImGui::PopItemWidth();

    //        ImGui::SliderFloat((std::string("metallic") + "##" + obj_name).c_str(), &material.metallic, 0.0f, 1.0f);
    //        ImGui::SliderFloat((std::string("roughness") + "##" + obj_name).c_str(), &material.roughness, 0.01f, 1.0f);
    //        ImGui::SliderFloat((std::string("ao") + "##" + obj_name).c_str(), &material.ao, 0.0f, 1.0f);
    //    }
    //}
    //if (world.hasComponent<ecs::ExplosionComponent>(entity)) {
    //    auto& explosion = *world.getComponent<ecs::ExplosionComponent>(entity);
    //    ImGui::SliderFloat((std::string("explosion ratio") + "##" + obj_name).c_str(), &explosion.explosionRatio, 0.0f, 1.0f);
    //}
    //if (world.hasComponent<ecs::SkyboxComponent>(entity))
    //    ImGui::Text("<SkyboxComponent>");
    //if (world.hasComponent<ecs::PointLightComponent>(entity)) {
    //    Vec4& luminousColor = world.getComponent<ecs::PointLightComponent>(entity)->luminousColor;
    //    float* radius = &world.getComponent<ecs::PointLightComponent>(entity)->radius;
    //    ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
    //    ImGui::SliderFloat((std::string("Radius") + "##" + obj_name).c_str(), radius, 5.0f, 50.0f);
    //}
    //if (world.hasComponent<ecs::DirectionalLightComponent>(entity)) {
    //    auto dir_light_component = world.getComponent<ecs::DirectionalLightComponent>(entity);
    //    // TODO dir_light_component->direction = ;
    //    Vec4& luminousColor = dir_light_component->luminousColor;
    //    ImGui::ColorEdit3((std::string("Luminous Color") + "##" + obj_name).c_str(), (float*)&luminousColor);
    //}

    //ImGui::End();
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

    auto camera = Application::GetApp().getScene()->getMainCamera();
    ecs::TransformComponent* transform_component = nullptr;
    //auto picked_entities = world.getPickedEntities();
    //for (auto entity : picked_entities) {
    //    transform_component = world.getComponent<ecs::TransformComponent>(entity);
    //    break;
    //}

    if (transform_component) {
        Mat4 model_matrix = transform_component->transform();
        ImGuizmo::Manipulate((float*)(&camera.view), (float*)(&camera.projection), imguizmo_operation, ImGuizmo::LOCAL, (float*)(&model_matrix), NULL, NULL, NULL, NULL);
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

void ImGuiEditor::configUIStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    /// 0 = FLAT APPEARENCE
    /// 1 = MORE "3D" LOOK
    int is3D = 0;

    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
    colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
    colors[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    style.PopupRounding = 3;

    style.WindowPadding = ImVec2(4, 4);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(6, 2);

    style.ScrollbarSize = 18;

    style.WindowBorderSize = 1;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = is3D;

    style.WindowRounding = 3;
    style.ChildRounding = 3;
    style.FrameRounding = 3;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 3;

#ifdef IMGUI_HAS_DOCK 
    style.TabBorderSize = is3D;
    style.TabRounding = 3;

    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif
}
