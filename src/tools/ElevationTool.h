#pragma once

#include "Tool.h"
#include "../ElevationGrid.h"

class ElevationTool : public Tool {
public:
    ElevationTool(ToolManager &toolManager);
    ~ElevationTool();

    void postUpdate() override;
    std::string getName() override;
    ToolType getType() override;

private:
    void update() override;

    Elevation m_currentElevation;
};
