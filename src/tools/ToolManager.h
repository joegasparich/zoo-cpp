#pragma once

#include "common.h"

#include "Tool.h"
#include "ToolGhost.h"
#include "ui/Window.h"

struct Action {
    std::string name;
    json data;
    std::function<void(json&)> undo;
};

class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    void setup();
    void cleanup();
    void preUpdate();
    void update();
    void postUpdate();
    void render();
    void onGUI();
    void onInput(InputEvent* event);

    void pushAction(std::unique_ptr<Action> action);
    void undo();

    void setTool(ToolType type);
    Tool& getActiveTool();

    std::unique_ptr<ToolGhost> ghost;
private:
    std::unique_ptr<Tool> activeTool;
    std::string toolbarId;
    // TODO: implement max undos and use list instead
    std::stack<std::unique_ptr<Action>> actionStack;
};
