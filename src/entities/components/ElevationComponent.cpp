#include "Root.h"
#include "ElevationComponent.h"

ElevationComponent::ElevationComponent(Entity *entity) : Component(entity) {}

COMPONENT ElevationComponent::getId() { return ELEVATION_COMPONENT; }

void ElevationComponent::start() {
    Component::start();
    renderer = entity->getComponent<RenderComponent>();
}

void ElevationComponent::update() {
    renderer->offset.y = -Root::zoo()->world->elevationGrid->getElevationAtPos(entity->pos);
}
