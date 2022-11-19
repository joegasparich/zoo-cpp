#include "Tool.h"
#include "ToolManager.h"
#include "InputManager.h"

Tool::Tool(ToolManager &toolManager) : toolManager{toolManager} {}

std::string Tool::getName() {
    return "Unknown Tool";
}

ToolType Tool::getType() {
    return ToolType::None;
}

Tool::~Tool() {}
