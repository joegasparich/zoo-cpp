#include "MoveComponent.h"
#include "entities/Entity.h"

MoveComponent::MoveComponent(Entity *entity) : Component(entity) {}
MoveComponent::MoveComponent(Entity* entity, float acceleration)
    : Component(entity), acceleration(acceleration) {}

COMPONENT MoveComponent::getId() { return MOVE_COMPONENT; }

void MoveComponent::start() {
    Component::start();

    input = entity->getComponent<InputComponent>();
    physics = entity->getComponent<PhysicsComponent>();
}

void MoveComponent::update() {
    Component::update();

    physics->addForce(input->inputVector * acceleration);
}

json MoveComponent::save() {
    auto saveData = Component::save();
    saveData.push_back({"acceleration", acceleration});

    return saveData;
}

void MoveComponent::load(json data) {
    Component::load(data);
    data.at("acceleration").get_to(acceleration);
}
