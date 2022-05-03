#include "UIComponent.h"

#include <utility>

#include "../lib/imgui/imgui.h"

UIComponent::UIComponent(std::string name) : m_name(std::move(name)) {}
UIComponent::~UIComponent() {}

void UIComponent::render() {
    ImGui::Begin(
            m_name.c_str(),
            &m_isOpen,
            (!m_showHeader ? ImGuiWindowFlags_NoTitleBar : ImGuiWindowFlags_None) |
            (!m_isResizeable ? ImGuiWindowFlags_NoResize : ImGuiWindowFlags_None) |
            (!m_isDraggable ? ImGuiWindowFlags_NoMove : ImGuiWindowFlags_None) |
            (!m_isScrollable ? ImGuiWindowFlags_NoScrollbar : ImGuiWindowFlags_None));
}

void UIComponent::postRender() {
    ImGui::End();
}

void UIComponent::close() {
    m_isOpen = false;
}
