#include <Zoo.h>
#include "TileObjectComponent.h"

COMPONENT TileObjectComponent::getId() { return TILE_OBJECT_COMPONENT; }
COMPONENT TileObjectComponent::getType() { return TILE_OBJECT_COMPONENT; }
std::set<COMPONENT> TileObjectComponent::getRequiredComponents() {
    return { RENDER_COMPONENT };
}

TileObjectComponent::TileObjectComponent(Entity *entity) : Component(entity) {}
TileObjectComponent::TileObjectComponent(Entity *entity, const ObjectData &data) : Component(entity), m_data(data) {}

void TileObjectComponent::start() {
    Component::start();

    Zoo::zoo->m_world->registerTileObject(m_entity);
}

std::vector<glm::ivec2> TileObjectComponent::getTiles() {
    std::vector<glm::ivec2> tiles{};
    glm::ivec2 base = glm::floor(m_entity->m_pos - m_data.size/2.0f);
    for (int i = 0; i < m_data.size.x; i++) {
        for (int j = 0; j < m_data.size.y; j++) {
            tiles.push_back(base + glm::ivec2{i, j});
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
    m_data = Registry::getObject(data["path"].get<std::string>());
}
