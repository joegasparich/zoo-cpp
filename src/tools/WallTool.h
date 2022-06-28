#pragma once

#include <Registry.h>
#include "world/World.h"
#include "Tool.h"

class WallTool : public Tool {
public:
    WallTool(ToolManager &toolManager);
    ~WallTool();

    void set() override;
    void unset() override;
    void update() override;
    void postUpdate() override;
    void render() override;

    void setWall(WallData* wall);
    WallData& getWall();

    std::string getName() override;
    ToolType getType() override;

private:
    void updateGhostSprite(ToolGhost& ghost, glm::ivec2 tilePos, Side quadrant);

    std::string m_panelId;
    WallData* m_currentWall;

    std::vector<std::unique_ptr<ToolGhost>> m_ghosts;
    bool m_isDragging;
    glm::vec2 m_dragTile;
    Side m_dragQuadrant;
};
