#include "Component.h"

Component::Component() {}

void Component::start(Entity &_entity) {
    entity = &_entity;
    hasStarted = true;

    for (auto component: requiredComponents) {
        // TODO: Check component exists
    }
}

void Component::preUpdate() {}

void Component::update() {}

void Component::postUpdate() {}

void Component::render(double step) {}

void Component::end() {}
