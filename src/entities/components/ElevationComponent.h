#pragma once

#include "common.h"
#include "Component.h"
#include "RenderComponent.h"

class ElevationComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    ElevationComponent(Entity *entity);

    void start() override;

    void update() override;

private:
    RenderComponent* renderer;
};
