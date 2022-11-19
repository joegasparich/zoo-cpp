#pragma once

#include <Registry.h>
#include "world/World.h"
#include "Tool.h"
#include "ToolGhost.h"

class DoorTool : public Tool {
public:
    DoorTool(ToolManager &toolManager);
    ~DoorTool();

    void set() override;
    void update() override;

    std::string getName() override;
    ToolType getType() override;

    void onInput(InputEvent* event) override;

private:
    void updateGhostSprite(ToolGhost& ghost, Cell tilePos, Side quadrant);
};
