#pragma once

#include "common.h"
#include "Component.h"

class InputComponent : public Component {
public:
    typedef InputComponent base;
    
    InputComponent(Entity *entity);
    COMPONENT getId() override;

    Vector2 inputVector = {};
};