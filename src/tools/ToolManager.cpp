#include "ToolManager.h"
#include "BiomeTool.h"
#include "NoTool.h"
#include "Game.h"
#include "UIManager.h"
#include "ui/dialogs/Toolbar.h"
#include "ElevationTool.h"
#include "WallTool.h"
#include "TileObjectTool.h"
#include "DoorTool.h"
#include "PathTool.h"
#include "Root.h"

ToolManager::ToolManager() {}
ToolManager::~ToolManager() {
    cleanup();
}

void ToolManager::setup() {
    ghost = std::make_unique<ToolGhost>();
    setTool(ToolType::None);

    toolbarId = Root::ui().pushWindow(std::make_unique<Toolbar>(this));
}

void ToolManager::cleanup() {
    setTool(ToolType::None);
    Root::ui().closeWindow(toolbarId);
}

void ToolManager::preUpdate () {
    activeTool->preUpdate();
}

void ToolManager::update() {
    activeTool->update();
}

void ToolManager::postUpdate() {
    activeTool->postUpdate();
}

void ToolManager::render() {
    ghost->render();
    activeTool->render();
}

void ToolManager::onGUI() {
    activeTool->onGUI();
}

void ToolManager::onInput(InputEvent* event) {
    activeTool->onInput(event);
    if (event->consumed) return;

    if (event->mouseButtonDown == MOUSE_BUTTON_RIGHT) {
        setTool(ToolType::None);
        event->consume();
    }
}

void ToolManager::setTool(ToolType type) {
    if (activeTool && activeTool->getType() == type) return;

    if (activeTool) {
        activeTool->unset();
        ghost->cleanup();
    }

    // TODO: can we automate this
    switch(type) {
        case ToolType::Biome:
            activeTool = std::make_unique<BiomeTool>(*this);
            break;
        case ToolType::Elevation:
            activeTool = std::make_unique<ElevationTool>(*this);
            break;
        case ToolType::Wall:
            activeTool = std::make_unique<WallTool>(*this);
            break;
        case ToolType::Door:
            activeTool = std::make_unique<DoorTool>(*this);
            break;
        case ToolType::Path:
            activeTool = std::make_unique<PathTool>(*this);
            break;
        case ToolType::TileObject:
            activeTool = std::make_unique<TileObjectTool>(*this);
            break;
        case ToolType::None:
        default:
            activeTool = std::make_unique<NoTool>(*this);
            break;
    }

    activeTool->set();
}

Tool& ToolManager::getActiveTool() {
    return *activeTool;
}
