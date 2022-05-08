#include "ElevationTool.h"
#include "ToolManager.h"
#include "Game.h"
#include "ui/ElevationPanel.h"
#include "ui/UIManager.h"
#include "Zoo.h"

ElevationTool::ElevationTool(ToolManager &toolManager) : Tool(toolManager) {}
ElevationTool::~ElevationTool() = default;

void ElevationTool::set() {
    m_currentElevation = Elevation::Hill;
    m_panelId = UIManager::createUIComponent(std::make_unique<ElevationPanel>(m_toolManager, *this));
    m_toolManager.m_ghost->m_type =  GhostType::Circle;
}

void ElevationTool::unset() {
    UIManager::closeUIComponent(m_panelId);
}

void ElevationTool::update() {
    auto& input = Game::get().m_input;
    auto& elevationGrid = Zoo::zoo->m_world->m_elevationGrid;

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

Elevation ElevationTool::getCurrentElevation() const {
    return m_currentElevation;
}

void ElevationTool::setCurrentElevation(Elevation currentElevation) {
    m_currentElevation = currentElevation;
}
