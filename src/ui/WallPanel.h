#pragma once

#include "imgui/imgui.h"

#include "UIComponent.h"
#include "tools/ToolManager.h"
#include "tools/WallTool.h"

class WallPanel : public UIComponent {
public:
    WallPanel(ToolManager& toolManager, WallTool& wallTool);
    ~WallPanel() override;

    void render() override;
    void onOpen() override;
    void onClose() override;

private:
    ToolManager& m_toolManager;
    WallTool& m_wallTool;
    std::vector<WallData *> m_allWalls;
};
