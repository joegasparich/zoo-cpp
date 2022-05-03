#include "NoTool.h"
#include "ToolManager.h"

NoTool::NoTool(ToolManager &toolManager) : Tool(toolManager) {}
NoTool::~NoTool() {}

void NoTool::update() {}
void NoTool::postUpdate() {}

std::string NoTool::getName() {
    return "No Tool";
}

ToolType NoTool::getType() {
    return ToolType::None;
}
