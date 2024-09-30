#include "ImGuiEditor.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuizmo.h>

#include <GLFW/glfw3.h>

#include "GUI/FileDialog.hpp"
#include "Render/RenderSystem.hpp"
#include "Logical/FrameWork/Scene.hpp"

#include "EngineAPI.hpp"

ImGuiEditor::ImGuiEditor()
    : m_main_canvas(std::make_unique<MainCanvas>())
    , m_context_menu(std::make_unique<ImGuiContextMenu>())
    , m_scene_hierarchy_window(std::make_unique<ImGuiSceneHierarchy>())
    , m_global_console_window(std::make_unique<ImGuiGlobalConsole>())
    , m_debug_window(std::make_unique<ImGuiDebugWindow>())
{
    if (!ImGui::GetCurrentContext() || !ImGui::GetCurrentContext()->Initialized) {
        // setup imgui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
        ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)GetApp().window()->getNativeWindowHandle(), true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }
}

ImGuiEditor::~ImGuiEditor()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiEditor::init()
{
    configUIStyle();

    m_scene_hierarchy_window->init();
}

void ImGuiEditor::onUpdate()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    renderEmptyMainDockerSpaceWindow();
    if (m_show_debug)
        m_debug_window->render(); // render first, the debug window need to be docked
    m_main_canvas->render();
    ImGui::PopStyleVar(3);
    m_context_menu->render();
    m_scene_hierarchy_window->render();
    m_global_console_window->render();
}

void ImGuiEditor::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiEditor::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

Viewport ImGuiEditor::getMainViewport() const
{
    return m_main_canvas->getViewport();
}

void ImGuiEditor::popUpMenu()
{
    ContextType context = GetApp().scene()->getPickedObjects().empty() ? ContextType::Void : ContextType::Object;
    m_context_menu->popUp(context);
}

void ImGuiEditor::dismissMenu()
{
    m_context_menu->dismiss();
}

void ImGuiEditor::renderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                FileDialog* file_dlg = FileDialog::create();
                auto filepath = file_dlg->OpenFile("");
                if (!filepath.empty()) {
                    GetApp().scene()->loadModel(filepath);
                }
            }
            if (ImGui::MenuItem("Save As..", "Ctrl+S")) {
                FileDialog* file_dlg = FileDialog::create();
                auto filepath = file_dlg->SaveFile("");
                // TODO
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            //if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            //if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            //ImGui::Separator();
            //if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            //if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            //if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("Debug Window", "", &m_show_debug)) {}
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void ImGuiEditor::renderEmptyMainDockerSpaceWindow()
{
#if NO_TITLE_BAR
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_MenuBar;
    ImGui::SetNextWindowSize(ImVec2(1920, 1080), ImGuiCond_Appearing);
    static bool show = true;
    ImGui::Begin("Engine", &show, window_flags);
    ImGuiID main_dock_id = ImGui::GetID("Main Dock");
    ImGui::DockSpace(main_dock_id);
    renderMenuBar();
    ImGui::End();
    if (!show) {
        glfwSetWindowShouldClose((GLFWwindow*)GetApp().window()->getNativeWindowHandle(), true);
    }
#else
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar |
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoBackground | ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;
    // When enabling ImGuiConfigFlags_ViewportsEnable, the coordinate system changes,
    // e.g. (0,0) generally becomes the top-left corner of primary monitor.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::Begin("Engine", nullptr, window_flags);
    ImGuiID main_dock_id = ImGui::GetID("Main Dock");
    ImGui::DockSpace(main_dock_id);
    renderMenuBar();
    ImGui::End();
#endif
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
