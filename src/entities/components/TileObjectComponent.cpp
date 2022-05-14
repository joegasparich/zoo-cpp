#include <Zoo.h>
#include "TileObjectComponent.h"

COMPONENT TileObjectComponent::getId() { return TILE_OBJECT_COMPONENT; }
COMPONENT TileObjectComponent::getType() { return TILE_OBJECT_COMPONENT; }
std::set<COMPONENT> TileObjectComponent::getRequiredComponents() {
    return { RENDER_COMPONENT };
}

TileObjectComponent::TileObjectComponent(const ObjectData &data) : m_data(data) {}

void TileObjectComponent::start(Entity &entity) {
    Component::start(entity);

    Zoo::zoo->m_world->registerTileObject(&entity);
}
