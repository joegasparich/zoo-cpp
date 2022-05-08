#include "ToolManager.h"
#include "BiomeTool.h"
#include "NoTool.h"
#include "Game.h"
#include "ui/Toolbar.h"
#include "ui/UIManager.h"
#include "ElevationTool.h"
#include "WallTool.h"

ToolManager::ToolManager() {}
ToolManager::~ToolManager() {
    reset();
}

void ToolManager::setup() {
    m_ghost = std::make_unique<ToolGhost>();
    setTool(ToolType::None);
    m_toolbarHandle = UIManager::createUIComponent(std::make_unique<Toolbar>(this));
}

void ToolManager::reset() {
    setTool(ToolType::None);
    UIManager::closeUIComponent(m_toolbarHandle);
}

void ToolManager::update() {
    m_activeTool->update();
}

void ToolManager::postUpdate() {
    m_activeTool->postUpdate();

    auto& input = Game::get().m_input;

    if (input->isMouseButtonDown(SDL_BUTTON_RIGHT)) {
        setTool(ToolType::None);
    }
}

void ToolManager::render() {
    m_ghost->render();
}

void ToolManager::setTool(ToolType type) {
    if (m_activeTool && m_activeTool->getType() == type) return;

    if (m_activeTool) {
        m_activeTool->unset();
        m_ghost->reset();
    }

    // TODO: can we automate this
    switch(type) {
        case ToolType::Biome:
            m_activeTool = std::make_unique<BiomeTool>(*this);
            break;
        case ToolType::Elevation:
            m_activeTool = std::make_unique<ElevationTool>(*this);
            break;
        case ToolType::Wall:
            m_activeTool = std::make_unique<WallTool>(*this);
            break;
        case ToolType::None:
        default:
            m_activeTool = std::make_unique<NoTool>(*this);
            break;
    }

    m_activeTool->set();
}

Tool& ToolManager::getActiveTool() {
    return *m_activeTool;
}
