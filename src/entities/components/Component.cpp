#include "Component.h"
#include "entities/Entity.h"

Component::Component(Entity* entity) {
    this->entity = entity;
}

void Component::start() {
    hasStarted = true;

    for (auto component: getRequiredComponents()) {
        // TODO: Check component exists
        if (!entity->hasComponent(component)) {
            std::cout << "Component " << component << " is required by " << getType() << std::endl;
        }
    }
}

void Component::preUpdate() {}
void Component::update() {}
void Component::postUpdate() {}
void Component::render(double step) {}
void Component::end() {}

json Component::save() {
    return json({
        {"id", getId()},
        {"isDisabled", disabled}
    });
}

void Component::load(json data) {
    data.at("isDisabled").get_to(disabled);
}
