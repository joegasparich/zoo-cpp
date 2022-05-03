#include "Tool.h"
#include "ToolManager.h"

Tool::Tool(ToolManager &toolManager) : m_toolManager{toolManager} {}

std::string Tool::getName() {
    return "Unknown Tool";
}

ToolType Tool::getType() {
    return ToolType::None;
}

Tool::~Tool() {}
