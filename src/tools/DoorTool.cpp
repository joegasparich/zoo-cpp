#include "DoorTool.h"
#include "ToolManager.h"
#include <Game.h>
#include <ZooScene.h>

DoorTool::DoorTool(ToolManager &toolManager) : Tool(toolManager) {}
DoorTool::~DoorTool() = default;

void DoorTool::set() {
    toolManager.ghost->type = GhostType::SpriteSheet;
    toolManager.ghost->snap = true;
    toolManager.ghost->scale = {1.0f, 2.0f};
    toolManager.ghost->offset = {0.0f, -1.0f};
}

void DoorTool::onInput(InputEvent* event) {
    Tool::onInput(event);

    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        auto& wallGrid = Root::zoo()->world->wallGrid;

        auto mousePos = Root::renderer().screenToWorldPos(GetMousePosition());
        auto mouseQuadrant = World::getQuadrantAtPos(mousePos);

        if (toolManager.ghost->canPlace) {
            Root::zoo()->world->wallGrid->placeDoor(Root::zoo()->world->wallGrid->getWallAtTile(flr(mousePos), mouseQuadrant));
        }
    }
}

void DoorTool::update() {
    auto& wallGrid = Root::zoo()->world->wallGrid;

    auto mousePos = Root::renderer().screenToWorldPos(GetMousePosition());
    auto mouseQuadrant = World::getQuadrantAtPos(mousePos);

    updateGhostSprite(*toolManager.ghost, flr(mousePos), mouseQuadrant);
}

void DoorTool::updateGhostSprite(ToolGhost &ghost, Cell tilePos, Side quadrant) {
    auto wall = Root::zoo()->world->wallGrid->getWallAtTile(tilePos, quadrant);
    auto& world = Root::zoo()->world;

    if (!wall || !wall->exists) {
        ghost.visible = false;
        ghost.canPlace = false;
        return;
    }
    ghost.visible = true;

    ghost.canPlace = !wall->isDoor && !world->wallGrid->isWallSloped(*wall);

    auto [spriteIndex, elevation] = WallGrid::getSpriteInfo(*wall, true);
    ghost.spriteSheet = Root::assetManager().getSpriteSheet(wall->data->spriteSheetPath);
    ghost.spriteSheetIndex = spriteIndex;

    switch (quadrant) {
        case Side::North:
            ghost.offset = {0.0f, -2.0f - elevation};
            break;
        case Side::South:
            ghost.offset = {0.0f, -1.0f - elevation};
            break;
        case Side::West:
            ghost.offset = {-0.5f, -1.0f - elevation};
            break;
        case Side::East:
            ghost.offset = {0.5f, -1.0f - elevation};
            break;
        default:
            break;
    }
}

std::string DoorTool::getName() {
    return "Door Tool";
}

ToolType DoorTool::getType() {
    return ToolType::Door;
}
