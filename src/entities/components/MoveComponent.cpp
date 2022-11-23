#include "MoveComponent.h"
#include "entities/Entity.h"
#include "Root.h"

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

void MoveComponent::serialise() {
    Component::serialise();

    Root::saveManager().SerialiseValue("acceleration", acceleration);
}
