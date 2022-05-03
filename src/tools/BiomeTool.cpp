#include "BiomeTool.h"
#include "ToolManager.h"
#include "../Game.h"
#include "../ui/BiomePanel.h"
#include "../ui/UIManager.h"

BiomeTool::BiomeTool(ToolManager &toolManager) : Tool(toolManager) {
    m_currentBiome = Biome::Sand;
    m_panelId = UIManager::createUIComponent(std::make_unique<BiomePanel>(m_toolManager, *this));
}
BiomeTool::~BiomeTool() {
    UIManager::closeUIComponent(m_panelId);
}

void BiomeTool::update() {
    auto& input = Game::get().m_input;
    auto& biomeGrid = Game::get().m_stage->m_world->m_biomeGrid;

    if (input->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
        biomeGrid->setBiomeInRadius(Renderer::screenToWorldPos(input->getMousePos()), 0.69f, m_currentBiome);
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
