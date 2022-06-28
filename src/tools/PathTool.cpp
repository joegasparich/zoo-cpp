#include "PathTool.h"
#include <ui/UIManager.h>
#include "ToolManager.h"
#include "constants/assets.h"
#include "world/PathGrid.h"
#include "Zoo.h"
#include "ui/PathPanel.h"
#include "Game.h"
#include "util/util.h"

PathTool::PathTool(ToolManager &toolManager) : Tool(toolManager) {}
PathTool::~PathTool() = default;

void PathTool::set() {
    m_panelId = UIManager::createUIComponent(std::make_unique<PathPanel>(m_toolManager, *this));
    m_toolManager.m_ghost->m_type = GhostType::SpriteSheet;
    m_toolManager.m_ghost->m_snap = true;
    m_toolManager.m_ghost->m_scale = {1.0f, 2.0f};
    m_toolManager.m_ghost->m_offset = {0.0f, -1.0f};

    // Temp
    setPath(&Registry::getPath(PATH_DIRT));
}

void PathTool::unset() {
    UIManager::closeUIComponent(m_panelId);
}

void PathTool::update() {
    if (!m_path) return;

    auto& input = Game::get().m_input;
    auto mousePos = Renderer::screenToWorldPos(input->getMousePos());

    updateGhostSprite(*m_toolManager.m_ghost);

    if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
        m_isDragging = true;
        m_dragTile = glm::floor(mousePos);
    }
    if (input->isMouseButtonUp(SDL_BUTTON_LEFT)) {
        m_isDragging = false;

        while (m_ghosts.size() > 0) {
            auto& ghost = m_ghosts.back();
            if (ghost->m_canPlace) {
                Zoo::zoo->m_world->m_pathGrid->placePathAtTile(m_path, glm::floor(ghost->m_pos));
            }
            m_ghosts.pop_back();
        }
    }
    if (input->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
        // Dragging
        m_toolManager.m_ghost->m_visible = false;

        auto xDif = glm::floor(mousePos).x - m_dragTile.x;
        auto yDif = glm::floor(mousePos).y - m_dragTile.y;
        auto horizontal = abs(xDif) > abs(yDif);
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
            updateGhostSprite(*ghost);

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

        updateGhostSprite(*m_toolManager.m_ghost);
    }
}

void PathTool::setPath(PathData *path) {
    m_path = path;
    auto spriteSheet = AssetManager::getSpriteSheet(path->spriteSheetPath);
    m_toolManager.m_ghost->m_sprite = std::make_unique<Sprite>(
        AssetManager::loadTexture(spriteSheet->m_image),
        glm::vec2{0.0f, 0.0f},
        glm::vec2{
            (float)spriteSheet->m_cellWidth / (float)spriteSheet->m_image->m_width,
            (float)spriteSheet->m_cellHeight / (float)spriteSheet->m_image->m_height
        }
    );
}

PathData *PathTool::getPath() {
    return m_path;
}

void PathTool::render() {
    for (auto& ghost : m_ghosts) {
        ghost->render();
    }
}

void PathTool::updateGhostSprite(ToolGhost& ghost) {
    auto& input = Game::get().m_input;

    auto path = Zoo::zoo->m_world->m_pathGrid->getPathAtTile(ghost.m_pos);

    if (!path) {
        ghost.m_visible = false;
        ghost.m_canPlace = false;
        return;
    }

    auto [spriteIndex, elevation] = PathGrid::getSpriteInfo(*path);

    auto spriteSheet = AssetManager::getSpriteSheet(m_path->spriteSheetPath);
    ghost.m_sprite->m_texCoord = spriteSheet->getTexCoords(spriteIndex)[0];

    ghost.m_visible = true;
    ghost.m_canPlace = true;
    if (path->exists) ghost.m_canPlace = false;
    if (Zoo::zoo->m_world->m_elevationGrid->isPositionSlopeCorner(path->pos)) ghost.m_canPlace = false;
    if (Zoo::zoo->m_world->m_elevationGrid->isPositionWater(path->pos)) ghost.m_canPlace = false;

    ghost.m_offset = {0.0f, -1.0f - elevation};
}

std::string PathTool::getName() {
    return "Path Tool";
}

ToolType PathTool::getType() {
    return ToolType::Path;
}