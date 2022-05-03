#include "ElevationTool.h"
#include "../Game.h"

ElevationTool::ElevationTool(ToolManager &toolManager) : Tool(toolManager) {
    m_currentElevation = Elevation::Hill;
}
ElevationTool::~ElevationTool() {}

void ElevationTool::update() {
    auto& input = Game::get().m_input;
    auto& elevationGrid = Game::get().m_stage->m_world->m_elevationGrid;

    if (input->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
        elevationGrid->setElevationInCircle(Renderer::screenToWorldPos(input->getMousePos()), 1.0f, m_currentElevation);
    }
}

void ElevationTool::postUpdate() {}

std::string ElevationTool::getName() {
    return "Elevation Tool";
}

ToolType ElevationTool::getType() {
    return ToolType::Elevation;
}
