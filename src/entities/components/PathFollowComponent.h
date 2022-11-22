#pragma once

#include "common.h"
#include "Component.h"
#include "InputComponent.h"

class PathFollowComponent : public InputComponent {
public:
    PathFollowComponent(Entity *entity);
    COMPONENT getId() override;

    void start() override;
    void end() override;
    void update() override;

    void pathTo(Vector2 targetPos);

    std::vector<Cell>& getPath();
private:
    Vector2 getCurrentNode();
    bool isCellInPath(const Cell& cell);

    std::vector<Cell> path = {};
    std::string pathRequestHandle;
    std::string placeSolidListener;
};