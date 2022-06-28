#pragma once

#include "imgui/imgui.h"

#include "UIComponent.h"
#include "tools/ToolManager.h"
#include "tools/PathTool.h"


class PathPanel : public UIComponent {
public:
    PathPanel(ToolManager& toolManager, PathTool& pathTool);
    ~PathPanel() override;

    void render() override;
    void onOpen() override;
    void onClose() override;

private:
    ToolManager& m_toolManager;
    PathTool& m_pathTool;
    std::vector<PathData *> m_allPaths;
};