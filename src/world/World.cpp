#include <constants/world.h>
#include <util/util.h>
#include "World.h"
#include "entities/components/TileObjectComponent.h"

World::World(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

void World::setup() {
    m_elevationGrid = std::make_unique<ElevationGrid>(m_width + 1, m_height + 1);
    m_elevationGrid->setup();

    m_biomeGrid = std::make_unique<BiomeGrid>(m_width * (int)BIOME_SCALE, m_height * (int)BIOME_SCALE);

    m_wallGrid = std::make_unique<WallGrid>(m_width, m_height);
    m_wallGrid->setup();
}

void World::reset() {
    m_biomeGrid->reset();
    m_elevationGrid->reset();
    m_wallGrid->reset();
}

void World::update() {

}

void World::postUpdate() {
    m_biomeGrid->postUpdate();
}

void World::render() {
    m_biomeGrid->render();
    m_elevationGrid->render();
    m_elevationGrid->renderDebug();
    m_wallGrid->render();
}

void World::registerTileObject(Entity *tileObject) {
    auto component = tileObject->getComponent<TileObjectComponent>();

    m_tileObjects.insert({tileObject->getId(), tileObject});
    for (int i = 0; i < component->m_data.size.x; i++) {
        for (int j = 0; j < component->m_data.size.y; j++) {
            m_tileObjectMap.insert({vecToString(glm::floor(tileObject->m_pos) + glm::vec2{i, j}), tileObject});
        }
    }
}

void World::unregisterTileObject(Entity *tileObject) {

}

bool World::isPositionInMap(glm::vec2 pos) const {
    return pos.x >= 0 && pos.x < (float)m_width && pos.y >= 0 && pos.y < (float)m_height;
}

Side World::getQuadrantAtPos(glm::vec2 pos) {
    auto xrel = fmod(pos.x + LARGER_THAN_WORLD, 1) - 0.5f;
    auto yrel = fmod(pos.y + LARGER_THAN_WORLD, 1) - 0.5f;

    if (yrel <= 0 && abs(yrel) >= abs(xrel)) return Side::North;
    if (xrel >= 0 && abs(xrel) >= abs(yrel)) return Side::East;
    if (yrel >= 0 && abs(yrel) >= abs(xrel)) return Side::South;
    if (xrel <= 0 && abs(xrel) >= abs(yrel)) return Side::West;
}

json World::save() {
    return json{
        {"elevation", m_elevationGrid->save()},
        {"biome", m_biomeGrid->save()},
        {"wall", m_wallGrid->save()}
    };
}

void World::load(json saveData) {
    // TODO: Check these exist first?
    m_elevationGrid->load(saveData["elevation"]);
    m_biomeGrid->load(saveData["biome"]);
    m_wallGrid->load(saveData["wall"]);
}
