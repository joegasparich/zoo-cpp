#pragma once

#include "common.h"
#include "Component.h"
#include "InputComponent.h"

class PathFollowComponent : public InputComponent {
public:
    PathFollowComponent(Entity *entity);
    COMPONENT getId() override;

    void update() override;

    void pathTo(Vector2 targetPos);

    std::vector<Cell>& getPath();

private:
    Vector2 getCurrentNode();

    std::vector<Cell> path = {};
};