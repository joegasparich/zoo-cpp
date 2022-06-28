#include <ui/UIManager.h>
#include <Game.h>
#include <Zoo.h>
#include <constants/assets.h>
#include <ui/WallPanel.h>
#include "WallTool.h"
#include "ToolManager.h"

WallTool::WallTool(ToolManager &toolManager) : Tool(toolManager) {}
WallTool::~WallTool() = default;

void WallTool::set() {
    m_currentWall = nullptr;
    m_panelId = UIManager::createUIComponent(std::make_unique<WallPanel>(m_toolManager, *this));
    m_toolManager.m_ghost->m_type = GhostType::SpriteSheet;
    m_toolManager.m_ghost->m_snap = true;
    m_toolManager.m_ghost->m_scale = {1.0f, 2.0f};
    m_toolManager.m_ghost->m_offset = {0.0f, -1.0f};

    // Temp
    setWall(&Registry::getWall(WALL_IRON_FENCE));
}

void WallTool::unset() {
    UIManager::closeUIComponent(m_panelId);
}

void WallTool::update() {
    if (!m_currentWall) return;

    auto& input = Game::get().m_input;
    auto& wallGrid = Zoo::zoo->m_world->m_wallGrid;

    auto mousePos = Renderer::screenToWorldPos(input->getMousePos());
    auto mouseQuadrant = World::getQuadrantAtPos(mousePos);

    if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
        m_isDragging = true;
        m_dragTile = glm::floor(mousePos);
        m_dragQuadrant = mouseQuadrant;
    }
    if (input->isMouseButtonUp(SDL_BUTTON_LEFT)) {
        m_isDragging = false;

        while (m_ghosts.size() > 0) {
            auto& ghost = m_ghosts.back();
            if (ghost->m_canPlace) {
                Zoo::zoo->m_world->m_wallGrid->placeWallAtTile(m_currentWall, glm::floor(ghost->m_pos), m_dragQuadrant);
            }
            m_ghosts.pop_back();
        }
    }
    if (input->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
        // Dragging
        m_toolManager.m_ghost->m_visible = false;

        auto xDif = glm::floor(mousePos).x - m_dragTile.x;
        auto yDif = glm::floor(mousePos).y - m_dragTile.y;
        auto horizontal = m_dragQuadrant == Side::North || m_dragQuadrant == Side::South;
        auto length = (horizontal ? abs(xDif) : abs(yDif)) + 1;

        // Push new ghosts to reach length
        while (m_ghosts.size() < length) {
            auto ghost = std::make_unique<ToolGhost>();
            ghost->m_type = GhostType::SpriteSheet;
            ghost->m_follow = false;
            ghost->m_sprite = std::make_unique<Sprite>(
                m_toolManager.m_ghost->m_sprite->m_texture,
                m_toolManager.m_ghost->m_sprite->m_texCoord,
                m_toolManager.m_ghost->m_sprite->m_texBounds
            );
            ghost->m_offset = m_toolManager.m_ghost->m_offset;
            ghost->m_scale = m_toolManager.m_ghost->m_scale;
            m_ghosts.push_back(std::move(ghost));
        }

        auto i = glm::floor(m_dragTile.x);
        auto j = glm::floor(m_dragTile.y);
        for (auto& ghost : m_ghosts) {
            ghost->m_pos = {i, j};
            updateGhostSprite(*ghost, {i, j}, m_dragQuadrant);

            if (horizontal) {
                i += glm::sign(glm::floor(mousePos).x - i);
            } else {
                j += glm::sign(glm::floor(mousePos).y - j);
            }
        }

        // Pop additional ghosts
        while (m_ghosts.size() > length) {
            m_ghosts.pop_back();
        }
    } else {
        m_toolManager.m_ghost->m_visible = true;

        updateGhostSprite(*m_toolManager.m_ghost, glm::floor(mousePos), mouseQuadrant);
    }
}

void WallTool::postUpdate() {}

void WallTool::render() {
    for (auto& ghost : m_ghosts) {
        ghost->render();
    }
}

void WallTool::updateGhostSprite(ToolGhost& ghost, glm::ivec2 tilePos, Side quadrant) {
    auto wall = Zoo::zoo->m_world->m_wallGrid->getWallAtTile(tilePos, quadrant);
    auto& world = Zoo::zoo->m_world;
    auto& elevationGrid = world->m_elevationGrid;

    if (!wall) {
        ghost.m_visible = false;
        return;
    }
    ghost.m_visible = true;

    ghost.m_canPlace = true;
    if (wall->exists) ghost.m_canPlace = false;
    auto [v1, v2] = world->m_wallGrid->getWallVertices(*wall);
    if (elevationGrid->getElevationAtPos(v1) < 0) ghost.m_canPlace = false;
    if (elevationGrid->getElevationAtPos(v2) < 0) ghost.m_canPlace = false;
    auto tiles = world->m_wallGrid->getAdjacentTiles(*wall);

    Entity* blockingObj;
    for (auto tile : tiles) {
        auto obj = world->getTileObjectAtPosition(tile);
        if (!blockingObj && obj) blockingObj = obj;

        if (obj && obj == blockingObj) ghost.m_canPlace = false;
    }

    auto [spriteIndex, elevation] = WallGrid::getSpriteInfo(*wall);

    auto spriteSheet = AssetManager::getSpriteSheet(m_currentWall->spriteSheetPath);
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

void WallTool::setWall(WallData* wall) {
    m_currentWall = wall;
    auto spriteSheet = AssetManager::getSpriteSheet(wall->spriteSheetPath);
    m_toolManager.m_ghost->m_sprite = std::make_unique<Sprite>(
        AssetManager::loadTexture(spriteSheet->m_image),
        glm::vec2{0.0f, 0.0f},
        glm::vec2{
            (float)spriteSheet->m_cellWidth / (float)spriteSheet->m_image->m_width,
            (float)spriteSheet->m_cellHeight / (float)spriteSheet->m_image->m_height
        }
    );
}

WallData &WallTool::getWall() {
    return *m_currentWall;
}

std::string WallTool::getName() {
    return "Wall Tool";
}

ToolType WallTool::getType() {
    return ToolType::Wall;
}
