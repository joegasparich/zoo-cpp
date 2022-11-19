#pragma once

#include "Tool.h"
#include "Registry.h"
#include "ToolGhost.h"

class PathTool : public Tool {
public:
    PathTool(ToolManager& toolManager);
    ~PathTool();

    void set() override;
    void onInput(InputEvent* event) override;
    void update() override;
    void onGUI() override;
    void render() override;

    std::string getName() override;
    ToolType getType() override;

    void setPath(PathData* path);
    PathData* getPath();

private:
    void updateGhostSprite(ToolGhost& ghost);

    PathData* currentPath;
    std::vector<PathData*> allPaths;

    std::vector<std::unique_ptr<ToolGhost>> ghosts;
    bool isDragging;
    Cell dragTile;
};
