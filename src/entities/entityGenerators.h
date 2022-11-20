#pragma once

#include "ZooScene.h"
#include "Entity.h"
#include "components/RenderComponent.h"
#include "components/TileObjectComponent.h"
#include "components/ElevationComponent.h"
#include "components/SolidComponent.h"

inline std::unique_ptr<Entity> createTileObject(const std::string& assetPath, Vector2 position) {
    if (!Root::zoo()->world->isPositionInMap(position)) return nullptr;

    auto data = Registry::getObject(assetPath);

    auto entity = std::make_unique<Entity>(Vector2(position) + (data.size / 2.0f));
    if (!data.spritePath.empty()) {
        auto renderer = entity->addComponent(std::make_unique<RenderComponent>(
            entity.get(),
            data.spritePath
        ));
        renderer->pivot = data.pivot;
    } else if (!data.spriteSheetPath.empty()) {
        auto spritesheet = Root::assetManager().getSpriteSheet(data.spriteSheetPath);
        auto renderer = entity->addComponent(std::make_unique<RenderComponent>(
            entity.get(),
            spritesheet->texturePath
        ));
        renderer->pivot = data.pivot;
        renderer->setSource({
            0.0f,
            0.0f,
            float(spritesheet->cellWidth) / float(spritesheet->texture->width),
            float(spritesheet->cellHeight) / float(spritesheet->texture->height)
        });
    }

    auto tileObject = entity->addComponent(std::make_unique<TileObjectComponent>(entity.get(), data));

    // scale
    if (data.solid) {
        auto solid = entity->addComponent(std::make_unique<SolidComponent>(entity.get()));
        solid->setTiles(tileObject->getTiles());
    }
    entity->addComponent(std::make_unique<ElevationComponent>(entity.get()));

    return entity;
}
