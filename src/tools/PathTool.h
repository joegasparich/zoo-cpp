#pragma once

#include "Tool.h"
#include "Registry.h"
#include "ToolGhost.h"

class PathTool : public Tool {
public:
    PathTool(ToolManager& toolManager);
    ~PathTool();

    void update() override;
    void set() override;
    void unset() override;

    void render() override;

    std::string getName() override;
    ToolType getType() override;

    void setPath(PathData* path);
    PathData* getPath();

private:
    void updateGhostSprite(ToolGhost& ghost);

    std::string m_panelId;
    PathData* m_path;

    std::vector<std::unique_ptr<ToolGhost>> m_ghosts;
    bool m_isDragging;
    glm::vec2 m_dragTile;
};
