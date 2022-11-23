#include "Component.h"
#include "entities/Entity.h"
#include "Root.h"

Component::Component(Entity* entity) {
    this->entity = entity;
}

void Component::start() {
    hasStarted = true;
}

void Component::preUpdate() {}
void Component::update() {}
void Component::postUpdate() {}
void Component::render(double step) {}
void Component::end() {}

void Component::serialise() {
    Root::saveManager().SerialiseValue("id", getId(), (std::function<void(COMPONENT)>)[](auto _){});
    Root::saveManager().SerialiseValue("disabled", disabled);
}
