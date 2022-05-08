#pragma once

#include "Tool.h"
#include "world/ElevationGrid.h"

class ElevationTool : public Tool {
public:
    ElevationTool(ToolManager &toolManager);
    ~ElevationTool();

    void set() override;
    void unset() override;
    void update() override;
    void postUpdate() override;

    Elevation getCurrentElevation() const;
    void setCurrentElevation(Elevation currentElevation);

    std::string getName() override;
    ToolType getType() override;
private:
    std::string m_panelId;
    Elevation m_currentElevation;
};
