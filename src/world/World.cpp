#include "World.h"

World::World(unsigned int width, unsigned int height) : m_width(width), m_height(height) {}

void World::setup() {
    m_elevationGrid = std::make_unique<ElevationGrid>(m_width + 1, m_height + 1);
    m_elevationGrid->setup();

    m_biomeGrid = std::make_unique<BiomeGrid>(m_width * (int)BIOME_SCALE, m_height * (int)BIOME_SCALE);
}

void World::update() {

}

void World::postUpdate() {
    m_biomeGrid->postUpdate();
}

void World::render() {
    m_biomeGrid->render();
    m_elevationGrid->render();
}

json World::save() {
    return json{
        {"elevation", m_elevationGrid->save()},
        {"biome", m_biomeGrid->save()}
    };
}

void World::load(json saveData) {
    m_elevationGrid->load(saveData["elevation"]);
    m_biomeGrid->load(saveData["biome"]);
}
