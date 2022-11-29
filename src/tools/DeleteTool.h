#pragma once

#include "common.h"
#include "Tool.h"
#include "entities/Entity.h"
#include "world/WallGrid.h"

class DeleteTool : public Tool {
public:
    DeleteTool(ToolManager& toolManager);
    ~DeleteTool();

    void set() override;
    void update() override;
    void onInput(InputEvent* event) override;

    std::string getName() override;
    ToolType getType() override;

private:
    IRect getDragRect();
    std::vector<Entity*> getHighlightedTileObjects();
    std::vector<Cell> getHighlightedPaths();
    std::vector<Wall*> getHighlightedWalls();

    bool isDragging;
    Cell dragStartTile;
};
