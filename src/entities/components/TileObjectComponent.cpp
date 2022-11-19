#include <Zoo.h>

#include <utility>
#include "TileObjectComponent.h"
#include "Root.h"

COMPONENT TileObjectComponent::getId() { return TILE_OBJECT_COMPONENT; }
COMPONENT TileObjectComponent::getType() { return TILE_OBJECT_COMPONENT; }
std::set<COMPONENT> TileObjectComponent::getRequiredComponents() {
    return { RENDER_COMPONENT };
}

TileObjectComponent::TileObjectComponent(Entity *entity) : Component(entity) {}
TileObjectComponent::TileObjectComponent(Entity *entity, ObjectData data) : Component(entity), m_data(std::move(data)) {}

void TileObjectComponent::start() {
    Component::start();

    Root::zoo()->world->registerTileObject(entity);
}

std::vector<Cell> TileObjectComponent::getTiles() {
    std::vector<Cell> tiles{};
    auto base = Cell{entity->pos - m_data.size/2.0f};
    for (auto i = 0; i < m_data.size.x; i++) {
        for (auto j = 0; j < m_data.size.y; j++) {
            tiles.push_back(base + Cell{i, j});
        }
    }
    return tiles;
}

json TileObjectComponent::save() {
    auto saveData = Component::save();
    saveData.push_back({"path", m_data.assetPath});

    return saveData;
}

void TileObjectComponent::load(json data) {
    Component::load(data);
    m_data = Registry::getObject(data.at("path").get<std::string>());
}
