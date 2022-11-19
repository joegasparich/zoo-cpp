#pragma once

#include "common.h"

#include "Tool.h"
#include "ToolGhost.h"
#include "ui/Window.h"

class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    void setup();
    void cleanup();
    void update();
    void postUpdate();
    void render();
    void onGUI();
    void onInput(InputEvent* event);

    void setTool(ToolType type);
    Tool& getActiveTool();

    std::unique_ptr<ToolGhost> ghost;
private:
    std::unique_ptr<Tool> activeTool;
    std::string toolbarId;
};
