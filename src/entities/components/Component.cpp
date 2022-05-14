#include "Component.h"
#include "entities/Entity.h"

Component::Component() {}

void Component::start(Entity& _entity) {
    m_entity = &_entity;
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
