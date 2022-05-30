#pragma once

#include <entities/components/RenderComponent.h>
#include <entities/components/TileObjectComponent.h>
#include "Zoo.h"
#include "Entity.h"

inline std::unique_ptr<Entity> createTileObject(const std::string& assetPath, glm::ivec2 position) {
    if (!Zoo::zoo->m_world->isPositionInMap(position)) return nullptr;

    auto data = Registry::getObject(assetPath);

    auto entity = std::make_unique<Entity>(glm::vec2(position) + (data.size / 2.0f));
    if (data.image) {
        auto renderer = entity->addComponent(std::make_unique<RenderComponent>(
            entity.get(),
            std::make_unique<Sprite>(AssetManager::loadTexture(data.image))
        ));
        renderer->m_pivot = data.pivot;
    } else if (data.spriteSheet) {
        auto renderer = entity->addComponent(std::make_unique<RenderComponent>(
            entity.get(),
            std::make_unique<Sprite>(
                AssetManager::loadTexture(data.spriteSheet->m_image),
                glm::vec2{0.0f, 0.0f},
                glm::vec2{
                    (float)data.spriteSheet->m_cellWidth / (float)data.spriteSheet->m_image->m_width,
                    (float)data.spriteSheet->m_cellHeight / (float)data.spriteSheet->m_image->m_height
                }

            )
        ));
        renderer->m_pivot = data.pivot;
    }

    // scale
    // solid

    entity->addComponent(std::make_unique<TileObjectComponent>(entity.get(), data));

    return entity;
}
