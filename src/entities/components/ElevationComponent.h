#pragma once

#include "common.h"
#include "Component.h"
#include "RenderComponent.h"

class ElevationComponent : public Component {
public:
    typedef ElevationComponent base;
    
    ElevationComponent(Entity *entity);
    COMPONENT getId() override;


    void start() override;
    void update() override;

private:
    RenderComponent* renderer;
};
