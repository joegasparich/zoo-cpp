#pragma once

#include <memory>

#include "Tool.h"

class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    void update();
    void postUpdate();

    void setTool(ToolType type);
    Tool& getActiveTool();

private:
    std::unique_ptr<Tool> m_activeTool;
};
