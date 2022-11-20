#pragma once

#include "common.h"
#include "Component.h"

class PhysicsComponent : public Component {
public:
    typedef PhysicsComponent base;

    PhysicsComponent(Entity* entity);

    PhysicsComponent(Entity* entity, float mass, float friction);

    COMPONENT getId() override;

    void update() override;
    void postUpdate() override;

    void addForce(Vector2 force);

    json save() override;
    void load(json data) override;

    float mass = 50;
    float friction = 0.5f;
private:
    Vector2 velocity = {};
    Vector2 force = {};
};