#pragma once

#include "imgui/imgui.h"

#include <tools/TileObjectTool.h>
#include <tools/ToolManager.h>
#include "UIComponent.h"

class ObjectPanel : public UIComponent {
public:
    ObjectPanel(ToolManager& toolManager, TileObjectTool& wallTool);
    ~ObjectPanel() override;

    void render() override;
    void onOpen() override;
    void onClose() override;

private:
    ToolManager& m_toolManager;
    TileObjectTool& m_tileObjectTool;
    std::vector<ObjectData*> m_allObjects;
};
