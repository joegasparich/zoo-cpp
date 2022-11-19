#include "Root.h"
#include "ElevationComponent.h"

COMPONENT ElevationComponent::getId() { return ELEVATION_COMPONENT; }
COMPONENT ElevationComponent::getType() { return ELEVATION_COMPONENT; }
std::set<COMPONENT> ElevationComponent::getRequiredComponents() {
    return { RENDER_COMPONENT };
}

ElevationComponent::ElevationComponent(Entity *entity) : Component(entity) {}

void ElevationComponent::start() {
    Component::start();
    renderer = entity->getComponent<RenderComponent>();
}

void ElevationComponent::update() {
    renderer->offset.y = -Root::zoo()->world->elevationGrid->getElevationAtPos(entity->pos);
}
