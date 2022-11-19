#pragma once

#include "Registry.h"
#include "world/World.h"
#include "Tool.h"

class WallTool : public Tool {
public:
    WallTool(ToolManager &toolManager);
    ~WallTool();

    void set() override;
    void update() override;
    void render() override;

    void setWall(WallData* wall);
    WallData& getWall();

    std::string getName() override;
    ToolType getType() override;

    void onGUI() override;

    void onInput(InputEvent* event) override;

private:
    void updateGhostSprite(ToolGhost& ghost, Cell tilePos, Side quadrant);

    WallData* currentWall;
    std::vector<WallData*> allWalls;

    std::vector<std::unique_ptr<ToolGhost>> ghosts;
    bool isDragging;
    Cell dragTile;
    Side dragQuadrant;
};
