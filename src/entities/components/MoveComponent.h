#pragma once

#include "common.h"
#include "Component.h"
#include "PhysicsComponent.h"
#include "InputComponent.h"

class MoveComponent : public Component {
public:
    typedef MoveComponent base;

    MoveComponent(Entity *entity);
    MoveComponent(Entity* entity, float acceleration);

    COMPONENT getId() override;

    void start() override;
    void update() override;

    void serialise() override;

    float acceleration = 0.5f;
private:
    InputComponent* input;
    PhysicsComponent* physics;
};