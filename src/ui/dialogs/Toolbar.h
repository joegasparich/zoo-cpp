#pragma once

#include "common.h"
#include "ui/Window.h"
#include "tools/ToolManager.h"

class Toolbar : public Window {
public:
    explicit Toolbar(ToolManager *toolManager);

    void doWindowContents() override;

private:
    ToolManager* toolManager{};

    std::string debugPanelId;
    std::string profilerPanelId;
};
