#include <Game.h>
#include <constants/assets.h>
#include <entities/entityGenerators.h>
#include <ui/ObjectPanel.h>
#include <ui/UIManager.h>
#include "TileObjectTool.h"
#include "ToolManager.h"

TileObjectTool::TileObjectTool(ToolManager &toolManager) : Tool(toolManager) {}
TileObjectTool::~TileObjectTool() = default;

void TileObjectTool::set() {
    m_panelId = UIManager::createUIComponent(std::make_unique<ObjectPanel>(m_toolManager, *this));
    m_toolManager.m_ghost->m_type = GhostType::Sprite;
    m_toolManager.m_ghost->m_snap = true;
    m_toolManager.m_ghost->m_scale = {1.0f, 1.0f};
    m_toolManager.m_ghost->m_offset = {0.5f, 0.5f};

    // Temp
    setObject(&Registry::getObject(OBJ_TREE));
}

void TileObjectTool::unset() {
    UIManager::closeUIComponent(m_panelId);
}

void TileObjectTool::update() {
    if (!m_currentObject) return;
    auto& input = Game::get().m_input;
    auto& elevationGrid = Zoo::zoo->m_world->m_elevationGrid;
    auto mousePos = glm::floor(Renderer::screenToWorldPos(input->getMousePos()));

    m_toolManager.m_ghost->m_visible = true;
    m_toolManager.m_ghost->m_canPlace = true;

    for (int i = 0; i < m_currentObject->size.x; i++) {
        for (int j = 0; j < m_currentObject->size.y; j++) {
            auto pos = mousePos + glm::vec2{i, j};
            if (Zoo::zoo->m_world->getTileObjectAtPosition(pos)) m_toolManager.m_ghost->m_canPlace = false;
            if (elevationGrid->isTileWater(pos)) m_toolManager.m_ghost->m_canPlace = false;
            if (!Zoo::zoo->m_world->isPositionInMap(pos)) {
                m_toolManager.m_ghost->m_visible = false;
                return;
            }
        }
    }

    if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
        if (!m_toolManager.m_ghost->m_canPlace) return;

        auto object = createTileObject(m_currentObject->assetPath, m_toolManager.m_ghost->m_pos);
        Zoo::zoo->m_world->registerTileObject(object.get());
        Zoo::zoo->registerEntity(std::move(object));
    }
}

void TileObjectTool::setObject(ObjectData* object) {
    m_currentObject = object;

    if (object->image) {
        m_toolManager.m_ghost->m_type = GhostType::Sprite;
        m_toolManager.m_ghost->m_sprite = std::make_unique<Sprite>(AssetManager::loadTexture(object->image));
    } else if (object->spriteSheet) {
        m_toolManager.m_ghost->m_type = GhostType::SpriteSheet;
        m_toolManager.m_ghost->m_sprite = std::make_unique<Sprite>(
            AssetManager::loadTexture(object->spriteSheet->m_image),
            glm::vec2{0.0f, 0.0f},
            glm::vec2{
                (float)object->spriteSheet->m_cellWidth / (float)object->spriteSheet->m_image->m_width,
                (float)object->spriteSheet->m_cellHeight / (float)object->spriteSheet->m_image->m_height
            }
        );
    } else {
        std::cout << "[TileObjectTool] No object sprite" << std::endl;
    }
    m_toolManager.m_ghost->m_pivot = object->pivot;
    m_toolManager.m_ghost->m_offset = object->size / 2.0f;
}

ObjectData* TileObjectTool::getObject() {
    return m_currentObject ? m_currentObject : nullptr;
}

std::string TileObjectTool::getName() {
    return "Object Tool";
}

ToolType TileObjectTool::getType() {
    return ToolType::TileObject;
}
