#include <functional>
#include "../lib/imgui/imgui.h"

#include "Toolbar.h"

Toolbar::Toolbar(ToolManager *toolManager) : UIComponent("Toolbar"), m_toolManager{toolManager} {
    m_showHeader = false;
    m_isDraggable = false;
    m_isResizeable = false;
    m_isScrollable = false;
}
Toolbar::~Toolbar() {}

void renderButton(std::string label, bool isActive, std::function<void()> onClick) {
    if (isActive) {
        ImGui::PushStyleColor(ImGuiCol_Button, {1, 1, 1, 1});
        ImGui::PushStyleColor(ImGuiCol_Text, {0, 0, 0, 1});
    }
    if (ImGui::Button(label.c_str())) {
        onClick();
    };
    ImGui::SameLine();
    if (isActive) {
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
}

void Toolbar::render() {
    UIComponent::render();

    ImGui::SetWindowSize({1000, 20});
    ImGui::SetWindowPos({0, 0});

    // TODO: Can we loop this
    renderButton("Biome", m_toolManager->getActiveTool().getType() == ToolType::Biome, [this]() {
        m_toolManager->setTool(ToolType::Biome);
    });
    renderButton("Elevation", m_toolManager->getActiveTool().getType() == ToolType::Elevation, [this]() {
        m_toolManager->setTool(ToolType::Elevation);
    });
}

void Toolbar::onOpen() {}

void Toolbar::onClose() {}
