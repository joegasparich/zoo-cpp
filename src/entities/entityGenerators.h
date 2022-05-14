#pragma once

#include <entities/components/RenderComponent.h>
#include <entities/components/TileObjectComponent.h>
#include "Zoo.h"
#include "Entity.h"

inline std::unique_ptr<Entity> createTileObject(std::string assetPath, glm::ivec2 position) {
    if (!Zoo::zoo->m_world->isPositionInMap(position)) return nullptr;
//    if (!Zoo::zoo->m_world->isTileFree(position)) return nullptr;

    auto data = Registry::getObject(assetPath);

    auto entity = std::make_unique<Entity>(glm::vec2(position) + (data.size / 2.0f));
    auto renderer = entity->addComponent(std::make_unique<RenderComponent>(
        AssetManager::loadTexture(AssetManager::getImage(data.spritePath))
    ));

    renderer->m_pivot = data.pivot;
    // scale
    // solid

    entity->addComponent(std::make_unique<TileObjectComponent>(data));

    return entity;
}
