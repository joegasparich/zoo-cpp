#pragma once

#include "Tool.h"
#include "ToolManager.h"

class NoTool : public Tool {
public:
    NoTool(ToolManager &toolManager);

    std::string getName() override;
    ToolType getType() override;

    virtual ~NoTool();

    ToolType m_toolType = ToolType::None;
private:
    void update() override;
    void postUpdate() override;
};
