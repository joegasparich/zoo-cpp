#include "ElevationPanel.h"

ElevationPanel::ElevationPanel(ToolManager &toolManager, ElevationTool &elevationTool) :
    UIComponent("Elevation"),
    m_toolManager(toolManager),
    m_elevationTool(elevationTool) {
    m_showHeader = true;
    m_isDraggable = true;
    m_isResizeable = false;
    m_isScrollable = false;
}

ElevationPanel::~ElevationPanel() = default;

void ElevationPanel::render() {
    UIComponent::render();

    ImGui::SetWindowSize({200, 200});

    auto currentElevation = m_elevationTool.getCurrentElevation();

    if (ImGui::Button("Hill", {32, 32})) {
        m_elevationTool.setCurrentElevation(Elevation::Hill);
    }
    ImGui::SameLine();
    if (ImGui::Button("Flat", {32, 32})) {
        m_elevationTool.setCurrentElevation(Elevation::Flat);
    }
    ImGui::SameLine();
    if (ImGui::Button("Water", {32, 32})) {
        m_elevationTool.setCurrentElevation(Elevation::Water);
    }

    ImGui::Text("Current Elevation: %d", currentElevation);
}

void ElevationPanel::onOpen() {}

void ElevationPanel::onClose() {
    if (!m_externallyClosed) {
        m_toolManager.setTool(ToolType::None);
    }
}
