#include "UIManager.h"
#include "renderer/Renderer.h"

int id = 0;

UIManager::UIManager() : m_components{}, m_componentsToPush{}, m_componentsToRemove{} {};

void UIManager::init() {
    auto& renderer = Renderer::get();
    auto& instance = UIManager::get();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(renderer.m_window, renderer.m_glContext);
    // TODO: make this adapt to the available glsl version?
    ImGui_ImplOpenGL3_Init("#version 150");
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

bool UIManager::processInput(SDL_Event *event) {
    ImGui_ImplSDL2_ProcessEvent(event);

    return ImGui::GetIO().WantCaptureMouse;
}

void UIManager::render() {
    auto& instance = UIManager::get();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Render components
    for(auto const& [id, component] : instance.m_components) {
        component->render();
        component->postRender();

        if (!component->m_isOpen) {
            instance.m_componentsToRemove.push_back(id);
        }
    }

    // Remove closed components
    for (auto const& componentId : instance.m_componentsToRemove) {
        std::cout << "Removing component with id: " << componentId << std::endl;
        if (instance.m_components.contains(componentId)) {
            instance.m_components.at(componentId)->onClose();
            instance.m_components.erase(componentId);
        }
    }
    instance.m_componentsToRemove.clear();

    // Push new components
    for(auto& component : instance.m_componentsToPush) {
        std::cout << "Adding component with id: " << component->m_id << std::endl;
        component->onOpen();
        instance.m_components.insert({component->m_id, std::move(component)});
    }
    instance.m_componentsToPush.clear();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

std::string UIManager::createUIComponent(std::unique_ptr<UIComponent> component) {
    auto& instance = UIManager::get();
    auto componentId = id;
    id++;

    component->m_id = std::to_string(componentId);
    instance.m_componentsToPush.push_back(std::move(component));

    return std::to_string(componentId);
}

void UIManager::closeUIComponent(const std::string& componentId) {
    auto& instance = UIManager::get();

    if (instance.m_components.contains(componentId)) {
        instance.m_components.at(componentId)->m_externallyClosed = true;
        instance.m_componentsToRemove.push_back(componentId);
    }
}
