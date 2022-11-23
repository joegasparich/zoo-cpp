#include "AreaPathFollowComponent.h"
#include "entities/Entity.h"
#include "Root.h"

COMPONENT AreaPathFollowComponent::getId() { return AREA_PATH_FOLLOW_COMPONENT; }

AreaPathFollowComponent::AreaPathFollowComponent(Entity *entity) : PathFollowComponent(entity), currentDoor{}, currentArea{} {}

void AreaPathFollowComponent::update () {

    if (areaPath.empty()) {
        PathFollowComponent::update();
        currentDoor = nullptr;
        return;
    };

    if (!currentDoor) {
        // We don't have a door to go to
        // Find and path to the next door
        auto nextArea = areaPath.front();
        float minDist = INFINITY;
        for (auto door : currentArea->connectedAreas.at(nextArea)) {
            auto dist = Vector2Distance(entity->pos, door->worldPos);
            if (dist < minDist) {
                currentDoor = door;
                minDist = dist;
            }
        }
        assert(currentDoor);
        auto doorTiles = Root::zoo()->world->wallGrid->getAdjacentTiles(*currentDoor);
        auto targetTile = std::find_if(doorTiles.begin(), doorTiles.end(), [&](auto tile) { return Root::zoo()->world->areaManager->getAreaAtPosition(tile) == currentArea; });
        PathFollowComponent::pathTo(targetTile->toVector());
    }

    // Head to the door
    PathFollowComponent::update();

    if (PathFollowComponent::reachedDest()) {
        // We've made it to the door
        enteringDoor = true;
        auto doorTiles = Root::zoo()->world->wallGrid->getAdjacentTiles(*currentDoor);
        enterDoorPos = std::find_if(doorTiles.begin(), doorTiles.end(), [&](auto tile) { return Root::zoo()->world->areaManager->getAreaAtPosition(tile) != currentArea; })->toVector() + Vector2{0.5f, 0.5f};
    }

    if (enteringDoor) {
        // We are going through the door
        inputVector = Vector2Normalize(enterDoorPos - entity->pos);

        if (Vector2Distance(entity->pos, enterDoorPos) < NODE_REACHED_DIST) {
            // We've made it through the door
            enterDoorPos = {};
            enteringDoor = false;
            currentDoor = nullptr;

            if (hasDeferredTarget) {
                pathTo(deferredTargetPos);
                hasDeferredTarget = false;
                return;
            }

            currentArea = areaPath.front();
            areaPath.pop_front();

            if (areaPath.empty()) {
                // We're in the final area, path to final destination
                PathFollowComponent::pathTo(targetPos);
            }
        }
    }

}

void AreaPathFollowComponent::pathTo(Vector2 target) {
    if (enteringDoor) {
        deferredTargetPos = target;
        hasDeferredTarget = true;
        return;
    }

    resetAreaPath();

    auto curArea = Root::zoo()->world->areaManager->getAreaAtPosition(entity->pos);
    auto targetArea = Root::zoo()->world->areaManager->getAreaAtPosition(target);

    if (curArea != targetArea) {
        areaPath = Root::zoo()->world->areaManager->BFS(curArea, targetArea);
        if (!areaPath.empty()) {
            currentArea = areaPath.front();
            areaPath.pop_front();
            targetPos = target;
        }
    } else {
        PathFollowComponent::pathTo(target);
    }
}

void AreaPathFollowComponent::resetAreaPath () {
    areaPath = {};
    currentArea = nullptr;
    currentDoor = nullptr;
    targetPos = {};
    enteringDoor = false;
    enterDoorPos = {};
    hasDeferredTarget = false;
    deferredTargetPos = {};
}

void AreaPathFollowComponent::serialise () {
    PathFollowComponent::serialise();

    Root::saveManager().SerialiseValue("areaPath",
        (std::function<json()>)     [&]() {
            std::vector<std::string> pathData{};
            for (auto area : areaPath) pathData.push_back(area->id);
            return pathData;
        },
        (std::function<void(json)>) [&](auto data) {
            for (json& id : data) areaPath.push_back(Root::zoo()->world->areaManager->getAreaById(id.get<std::string>()));
        }
    );
    Root::saveManager().SerialiseValue("currentArea", currentArea ? currentArea->id : "",
        (std::function<void(std::string)>)[&](std::string id){ currentArea = Root::zoo()->world->areaManager->getAreaById(id); });
    Root::saveManager().SerialiseValue("currentDoor", currentDoor ? currentDoor->gridPos : Cell{-1, -1},
        (std::function<void(Cell)>)[&](Cell gridPos){ currentDoor = Root::zoo()->world->wallGrid->getWallByGridPos(gridPos); });
    Root::saveManager().SerialiseValue("targetPos", targetPos);
    Root::saveManager().SerialiseValue("enteringDoor", enteringDoor);
    Root::saveManager().SerialiseValue("enterDoorPos", enterDoorPos);
    Root::saveManager().SerialiseValue("hasDeferredTarget", hasDeferredTarget);
    Root::saveManager().SerialiseValue("deferredTargetPos", deferredTargetPos);
}
