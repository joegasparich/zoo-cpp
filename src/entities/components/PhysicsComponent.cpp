#include "PhysicsComponent.h"
#include "entities/Entity.h"
#include "Root.h"

PhysicsComponent::PhysicsComponent(Entity *entity) : Component(entity) {}
PhysicsComponent::PhysicsComponent(Entity* entity, float mass, float friction)
    : Component(entity), mass(mass), friction(friction) {}

COMPONENT PhysicsComponent::getId() { return PHYSICS_COMPONENT; }

void PhysicsComponent::update() {
    Component::update();

    entity->pos = entity->pos + velocity;
}

void PhysicsComponent::postUpdate() {
    Component::postUpdate();

    // Add force
    velocity += force / mass;
    // Apply dampening
    velocity *= 1 / (1 + friction);

    force = {};
}

void PhysicsComponent::addForce(Vector2 f) {
    force += f;
}

void PhysicsComponent::serialise() {
    Component::serialise();

    Root::saveManager().SerialiseValue("velocity", velocity);
    Root::saveManager().SerialiseValue("force", force);
    Root::saveManager().SerialiseValue("mass", mass);
    Root::saveManager().SerialiseValue("friction", friction);
}
