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
    void postUpdate() override;
    void render() override;
    void unset() override;

    std::string getName() override;
    ToolType getType() override;

private:
    void updateGhostSprite(ToolGhost& ghost, glm::ivec2 tilePos, Side quadrant);
};
