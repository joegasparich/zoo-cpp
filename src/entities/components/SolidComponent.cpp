#include "SolidComponent.h"

#include "Root.h"

COMPONENT SolidComponent::getId() { return SOLID_COMPONENT; }
COMPONENT SolidComponent::getType() { return SOLID_COMPONENT; }
std::set<COMPONENT> SolidComponent::getRequiredComponents() {
    return {};
}

SolidComponent::SolidComponent(Entity* entity) : Component(entity) {
    tiles = { Cell(entity->pos) };
}

void SolidComponent::start() {
    Component::start();

    for (auto tile : tiles) {
        Root::zoo()->world->pathfinder->setAccessibility(tile, false);
    }
}

void SolidComponent::end() {
    Component::end();

    for (auto tile : tiles) {
        Root::zoo()->world->pathfinder->setAccessibility(tile, true);
    }
}

void SolidComponent::setTiles(std::vector<Cell> t) {
    tiles = std::move(t);
}
