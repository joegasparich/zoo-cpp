#include "Component.h"
#include "entities/Entity.h"

Component::Component(Entity* entity) {
    m_entity = entity;
}

void Component::start() {
    m_hasStarted = true;

    for (auto component: getRequiredComponents()) {
        // TODO: Check component exists
        if (!m_entity->hasComponent(component)) {
            std::cout << "Component " << component << " is required by " << getType() << std::endl;
        }
    }
}

void Component::preUpdate() {}
void Component::update() {}
void Component::postUpdate() {}
void Component::render(double step) {}
void Component::end() {}
