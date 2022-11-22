#pragma once

#include "Tool.h"
#include "world/ElevationGrid.h"

class ElevationTool : public Tool {
public:
    ElevationTool(ToolManager &toolManager);
    ~ElevationTool();

    void set() override;
    void update() override;
    void onInput(InputEvent* event) override;
    void onGUI() override;

    Elevation getCurrentElevation() const;
    void setCurrentElevation(Elevation currentElevation);

    std::string getName() override;
    ToolType getType() override;
private:
    Elevation currentElevation;
    bool dragging;
};
