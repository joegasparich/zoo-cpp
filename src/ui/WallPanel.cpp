#include "WallPanel.h"

WallPanel::WallPanel(ToolManager &toolManager, WallTool &wallTool) :
    UIComponent("Wall"),
    m_toolManager{toolManager},
    m_wallTool{wallTool},
    m_allWalls{Registry::getAllWalls()}
{
    m_isResizeable = false;
    m_isScrollable = false;
}
WallPanel::~WallPanel() = default;

void WallPanel::render() {
    UIComponent::render();

    ImGui::SetWindowSize({200, 200});

    auto currentWall = m_wallTool.getCurrentWall();

    for (auto wall : m_allWalls) {
        ImGui::Text(wall->name.c_str());
        auto spriteSheet = AssetManager::getSpriteSheet(wall->spriteSheetPath);
        if (wall->name == currentWall.name) ImGui::PushStyleColor(ImGuiCol_Button, {1, 1, 1, 1});
        ImGui::ImageButton(
            (void*)(intptr_t)AssetManager::loadTexture(spriteSheet->m_image)->getRendererId(),
            {32.0f, 32.0f},
            {0.0, 0.5f},
            {0.125f, 1.0f},
            5.0f
        );
        if (wall->name == currentWall.name) ImGui::PopStyleColor();
    }
}

void WallPanel::onOpen() {}

void WallPanel::onClose() {
    if (!m_externallyClosed) {
        m_toolManager.setTool(ToolType::None);
    }
}
