#pragma once

#include "common.h"
#include "Component.h"
#include "InputComponent.h"

#define NODE_REACHED_DIST 0.2f

class PathFollowComponent : public InputComponent {
public:
    PathFollowComponent(Entity *entity);
    COMPONENT getId() override;

    void start() override;
    void end() override;
    void update() override;

    virtual void pathTo(Vector2 targetPos);
    bool reachedDest();

    std::vector<Cell>& getPath();

    void serialise() override;

private:
    Vector2 getCurrentNode();

    std::vector<Cell> path = {};
    std::string pathRequestHandle;
    std::string placeSolidListener;
    bool pathCompleted;
};