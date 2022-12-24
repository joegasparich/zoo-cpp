#pragma once

#include "common.h"
#include "PathFollowComponent.h"
#include "world/Area.h"

class AreaPathFollowComponent : public PathFollowComponent {
public:
    COMPONENT getId() override;

    AreaPathFollowComponent(Entity *entity);

    void pathTo (Vector2 targetPos) override;

    void update () override;

    void serialise () override;

private:
    void resetAreaPath();

    std::list<Area*> areaPath;
    Vector2 targetPos;
    Area* currentArea;
    Wall* currentDoor;

    bool hasDeferredTarget = true;
    Vector2 deferredTargetPos;

    bool enteringDoor = false;
    Vector2 enterDoorPos;
};
