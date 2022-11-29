#include <ZooScene.h>

#include <utility>
#include "TileObjectComponent.h"
#include "Root.h"

TileObjectComponent::TileObjectComponent(Entity *entity) : Component(entity) {}
TileObjectComponent::TileObjectComponent(Entity *entity, ObjectData data)
    : Component(entity), data(std::move(data)) {}

COMPONENT TileObjectComponent::getId() { return TILE_OBJECT_COMPONENT; }

void TileObjectComponent::start() {
    Component::start();

    Root::zoo()->world->registerTileObject(entity);
}

void TileObjectComponent::end () {
    Component::end();

    Root::zoo()->world->unregisterTileObject(entity);
}

std::vector<Cell> TileObjectComponent::getTiles() {
    std::vector<Cell> tiles{};
    auto base = Cell{entity->pos - data.size / 2.0f};
    for (auto i = 0; i < data.size.x; i++) {
        for (auto j = 0; j < data.size.y; j++) {
            tiles.push_back(base + Cell{i, j});
        }
    }
    return tiles;
}

void TileObjectComponent::serialise() {
    Component::serialise();

    Root::saveManager().SerialiseValue("path",
        (std::function<json()>)     [&]() { return data.assetPath; },
        (std::function<void(json)>) [&](auto path) { data = Registry::getObject(path); });
}
