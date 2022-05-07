#include <renderer/Renderer.h>
#include "DebugInfo.h"

DebugInfo::DebugInfo() :
    UIComponent("Debug") {
    m_showHeader = false;
    m_isDraggable = false;
    m_isResizeable = false;
    m_isScrollable = false;
    m_showBackground = false;
}

DebugInfo::~DebugInfo() = default;

void DebugInfo::render() {
    UIComponent::render();

    ImGui::SetWindowSize({WINDOW_WIDTH, WINDOW_HEIGHT});

    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    ImGui::Text("%s", m_text.c_str());
    m_text = "";
}

void DebugInfo::onOpen() {}
void DebugInfo::onClose() {}