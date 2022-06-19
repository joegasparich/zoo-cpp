#include "DoorTool.h"
#include "ToolManager.h"
#include <Game.h>
#include <Zoo.h>

DoorTool::DoorTool(ToolManager &toolManager) : Tool(toolManager) {}
DoorTool::~DoorTool() = default;

void DoorTool::set() {
    m_toolManager.m_ghost->m_type = GhostType::SpriteSheet;
    m_toolManager.m_ghost->m_snap = true;
    m_toolManager.m_ghost->m_scale = {1.0f, 2.0f};
    m_toolManager.m_ghost->m_offset = {0.0f, -1.0f};
}

void DoorTool::update() {
    auto& input = Game::get().m_input;
    auto& wallGrid = Zoo::zoo->m_world->m_wallGrid;

    auto mousePos = Renderer::screenToWorldPos(input->getMousePos());
    auto mouseQuadrant = World::getQuadrantAtPos(mousePos);

    updateGhostSprite(*m_toolManager.m_ghost, glm::floor(mousePos), mouseQuadrant);

    if (input->isMouseButtonUp(SDL_BUTTON_LEFT)) {
        if (m_toolManager.m_ghost->m_canPlace) {
            Zoo::zoo->m_world->m_wallGrid->placeDoor(Zoo::zoo->m_world->m_wallGrid->getWallAtTile(glm::floor(mousePos), mouseQuadrant));
        }
    }
}

void DoorTool::updateGhostSprite(ToolGhost &ghost, glm::ivec2 tilePos, Side quadrant) {
    auto wall = Zoo::zoo->m_world->m_wallGrid->getWallAtTile(tilePos, quadrant);
    auto& world = Zoo::zoo->m_world;

    if (!wall || !wall->exists) {
        ghost.m_visible = false;
        return;
    }
    ghost.m_visible = true;

    ghost.m_canPlace = !wall->isDoor && !world->m_wallGrid->isWallSloped(*wall);

    auto [spriteIndex, elevation] = WallGrid::getSpriteInfo(*wall, true);
    auto spriteSheet = AssetManager::getSpriteSheet(wall->data.spriteSheetPath);

    ghost.m_sprite = std::make_unique<Sprite>(
        AssetManager::loadTexture(spriteSheet->m_image),
        glm::vec2{0.0f, 0.0f},
        glm::vec2{
            (float)spriteSheet->m_cellWidth / (float)spriteSheet->m_image->m_width,
            (float)spriteSheet->m_cellHeight / (float)spriteSheet->m_image->m_height
        }
    );
    ghost.m_sprite->m_texCoord = spriteSheet->getTexCoords(spriteIndex)[0];

    switch (quadrant) {
        case Side::North:
            ghost.m_offset = {0.0f, -2.0f - elevation};
            break;
        case Side::South:
            ghost.m_offset = {0.0f, -1.0f - elevation};
            break;
        case Side::West:
            ghost.m_offset = {-0.5f, -1.0f - elevation};
            break;
        case Side::East:
            ghost.m_offset = {0.5f, -1.0f - elevation};
            break;
        default:
            break;
    }
}

void DoorTool::postUpdate() {}
void DoorTool::render() {}
void DoorTool::unset() {}

std::string DoorTool::getName() {
    return "Door Tool";
}

ToolType DoorTool::getType() {
    return ToolType::Door;
}
