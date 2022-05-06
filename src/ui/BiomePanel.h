#pragma once

#include "lib/imgui/imgui.h"

#include "UIComponent.h"
#include "tools/ToolManager.h"
#include "tools/BiomeTool.h"

class BiomePanel : public UIComponent {
public:
    BiomePanel(ToolManager& toolManager, BiomeTool& biomeTool);
    ~BiomePanel() override;

    void render() override;
    void onOpen() override;
    void onClose() override;

private:
    ToolManager& m_toolManager;
    BiomeTool& m_biomeTool;
};
