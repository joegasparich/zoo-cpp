#include <ui/UIManager.h>
#include <Game.h>
#include <Zoo.h>
#include <constants/assets.h>
#include "WallTool.h"
#include "ToolManager.h"

WallTool::WallTool(ToolManager &toolManager) : Tool(toolManager) {}
WallTool::~WallTool() = default;

void WallTool::set() {
    m_currentWall = nullptr;
    // m_panelId = UIManager::createUIComponent(std::make_unique<ElevationPanel>(m_toolManager, *this));
    m_toolManager.m_ghost->m_type = GhostType::Sprite;
    m_toolManager.m_ghost->m_snap = true;
    m_toolManager.m_ghost->m_scale = {1.0f, 2.0f};
    m_toolManager.m_ghost->m_offset = {0.0f, -1.0f};

    // Temp
    setCurrentWall(Registry::getWall(WALL_IRON_FENCE));
}

void WallTool::unset() {
    UIManager::closeUIComponent(m_panelId);
}

void WallTool::update() {
    if (!m_currentWall) return;

    auto& input = Game::get().m_input;
    auto& wallGrid = Zoo::zoo->m_world->m_wallGrid;

    auto mousePos = Renderer::screenToWorldPos(input->getMousePos());
    auto side = World::getQuadrantAtPos(mousePos);
    auto wall = Zoo::zoo->m_world->m_wallGrid->getWallAtTile(mousePos, side);

    if (!wall) return;

    auto [spriteIndex, elevation] = WallGrid::getSpriteInfo(*wall);

    auto spriteSheet = AssetManager::getSpriteSheet(m_currentWall->spriteSheetPath);
    m_toolManager.m_ghost->m_texture->m_texCoord = spriteSheet->getTexCoords(spriteIndex)[0];

    switch (side) {
        case Side::North:
            m_toolManager.m_ghost->m_offset = {0.0f, -2.0f - elevation};
            break;
        case Side::South:
            m_toolManager.m_ghost->m_offset = {0.0f, -1.0f - elevation};
            break;
        case Side::West:
            m_toolManager.m_ghost->m_offset = {-0.5f, -1.0f - elevation};
            break;
        case Side::East:
            m_toolManager.m_ghost->m_offset = {0.5f, -1.0f - elevation};
            break;
        default:
            break;
    }

    if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
        // Place wall
        Zoo::zoo->m_world->m_wallGrid->placeWallAtTile(*m_currentWall, glm::floor(mousePos), side);
    }
}

void WallTool::postUpdate() {}

void WallTool::setCurrentWall(WallData& wall) {
    m_currentWall = &wall;
    auto spriteSheet = AssetManager::getSpriteSheet(wall.spriteSheetPath);
    m_toolManager.m_ghost->m_texture = std::make_unique<Texture>(
        spriteSheet->m_image,
        glm::vec2{0.0f, 0.0f},
        glm::vec2{
            (float)spriteSheet->m_cellWidth / (float)spriteSheet->m_image->m_width,
            (float)spriteSheet->m_cellHeight / (float)spriteSheet->m_image->m_height
        }
    );
}

WallData& WallTool::getCurrentWall() {
    return *m_currentWall;
}

std::string WallTool::getName() {
    return "Wall Tool";
}

ToolType WallTool::getType() {
    return ToolType::Wall;
}
