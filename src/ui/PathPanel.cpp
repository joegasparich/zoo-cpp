#include "PathPanel.h"

PathPanel::PathPanel(ToolManager &toolManager, PathTool &pathTool) :
    UIComponent("Path"),
    m_toolManager{toolManager},
    m_pathTool{pathTool},
    m_allPaths{Registry::getAllPaths()}
{
    m_isResizeable = false;
    m_isScrollable = false;
}
PathPanel::~PathPanel() {
    std::cout << "destroyed" << std::endl;
};

void PathPanel::render() {
    UIComponent::render();

    ImGui::SetWindowSize({200, 200});

    auto currentPath = m_pathTool.getPath();

    for (auto path : m_allPaths) {
        ImGui::Text(path->name.c_str());
        auto spriteSheet = AssetManager::getSpriteSheet(path->spriteSheetPath);
        if (path->name == currentPath->name) ImGui::PushStyleColor(ImGuiCol_Button, {1, 1, 1, 1});
        if (ImGui::ImageButton(
                (void*)(intptr_t)AssetManager::loadTexture(spriteSheet->m_image)->getRendererId(),
                {32.0f, 32.0f},
                {0.0, 0.5f},
                {0.125f, 1.0f},
                5.0f
        )) {
            m_pathTool.setPath(path);
        }
        if (path->name == currentPath->name) ImGui::PopStyleColor();
    }
}

void PathPanel::onOpen() {}

void PathPanel::onClose() {
    if (!m_externallyClosed) {
        m_toolManager.setTool(ToolType::None);
    }
}
