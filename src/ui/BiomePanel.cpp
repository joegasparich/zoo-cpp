#include "BiomePanel.h"

BiomePanel::BiomePanel(ToolManager& toolManager, BiomeTool& biomeTool) :
    UIComponent("Biome"),
    m_toolManager{toolManager},
    m_biomeTool{biomeTool} {
    m_showHeader = true;
    m_isDraggable = true;
    m_isResizeable = false;
    m_isScrollable = false;
}
BiomePanel::~BiomePanel() {}

void BiomePanel::render() {
    UIComponent::render();

    ImGui::SetWindowSize({200, 200});

    auto currentBiome = getBiomeInfo(m_biomeTool.getBiome());

    for(auto i = 0; i < NUM_BIOMES; i++) {
        auto biome = (Biome)i;
        auto [name, colour] = getBiomeInfo(biome);

        if (ImGui::ColorButton(name.c_str(), ImColor{colour.r, colour.g, colour.b}, ImGuiColorEditFlags_NoTooltip, {32, 32})) {
            m_biomeTool.setBiome(biome);
        }
        ImGui::SameLine();
    }
    ImGui::NewLine();

    ImGui::Text("Current Biome: %s", currentBiome.name.c_str());
}

void BiomePanel::onOpen() {}

void BiomePanel::onClose() {
    if (!m_externallyClosed) {
        m_toolManager.setTool(ToolType::None);
    }
}
