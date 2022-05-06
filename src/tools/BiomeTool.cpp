#include "BiomeTool.h"
#include "ToolManager.h"
#include "Game.h"
#include "ui/BiomePanel.h"
#include "ui/UIManager.h"
#include "Zoo.h"

BiomeTool::BiomeTool(ToolManager &toolManager) : Tool(toolManager) {}
BiomeTool::~BiomeTool() = default;

void BiomeTool::set() {
    m_currentBiome = Biome::Sand;
    m_panelId = UIManager::createUIComponent(std::make_unique<BiomePanel>(m_toolManager, *this));
    m_toolManager.m_ghost->m_type = GhostType::Circle;
}

void BiomeTool::unset() {
    UIManager::closeUIComponent(m_panelId);
}

void BiomeTool::update() {
    auto& input = Game::get().m_input;

    if (input->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
        Zoo::zoo->m_world->m_biomeGrid->setBiomeInRadius(Renderer::screenToWorldPos(input->getMousePos()), 0.69f, m_currentBiome);
    }
}

void BiomeTool::postUpdate() {}

std::string BiomeTool::getName() {
    return "Biome Tool";
}

ToolType BiomeTool::getType() {
    return ToolType::Biome;
}

Biome BiomeTool::getBiome() {
    return m_currentBiome;
}

void BiomeTool::setBiome(Biome biome) {
    m_currentBiome = biome;
}
