#include "PhysicsComponent.h"
#include "entities/Entity.h"

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

json PhysicsComponent::save() {
    auto saveData = Component::save();
    saveData.push_back({"velocity", velocity});
    saveData.push_back({"force", force});
    saveData.push_back({"mass", mass});
    saveData.push_back({"friction", friction});

    return saveData;
}

void PhysicsComponent::load(json data) {
    Component::load(data);
    data.at("velocity").get_to(velocity);
    data.at("force").get_to(force);
    data.at("mass").get_to(mass);
    data.at("friction").get_to(friction);
}
