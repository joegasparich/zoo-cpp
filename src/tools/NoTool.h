#pragma once

#include "Tool.h"
#include "ToolManager.h"

class NoTool : public Tool {
public:
    NoTool(ToolManager &toolManager);
    virtual ~NoTool();

    void set() override;
    void unset() override;

    std::string getName() override;
    ToolType getType() override;
private:
    void update() override;
    void postUpdate() override;
};
