#pragma once

#include <Registry.h>
#include "Tool.h"

class WallTool : public Tool {
public:
    WallTool(ToolManager &toolManager);
    ~WallTool();

    void set() override;
    void unset() override;
    void update() override;
    void postUpdate() override;

    void setCurrentWall(WallData& wall);
    WallData& getCurrentWall();

    std::string getName() override;
    ToolType getType() override;

private:
    std::string m_panelId;
    WallData* m_currentWall;
};
