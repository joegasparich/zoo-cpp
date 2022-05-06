#pragma once

#include "pch.h"

#include "Tool.h"
#include "ToolGhost.h"

class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    void setup();
    void reset();
    void update();
    void postUpdate();
    void render();

    void setTool(ToolType type);
    Tool& getActiveTool();

    std::unique_ptr<ToolGhost> m_ghost;
private:
    std::unique_ptr<Tool> m_activeTool;
    std::string m_toolbarHandle;
};
