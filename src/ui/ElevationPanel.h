#pragma once

#include "../lib/imgui/imgui.h"

#include "UIComponent.h"
#include "../tools/ToolManager.h"
#include "../tools/ElevationTool.h"

class ElevationPanel : public UIComponent {
public:
    ElevationPanel(ToolManager &toolManager, ElevationTool &biomeTool);
    ~ElevationPanel() override;

    void render() override;
    void onOpen() override;
    void onClose() override;

private:
    ToolManager& m_toolManager;
    ElevationTool& m_elevationTool;
};
