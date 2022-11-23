#include "PathFollowComponent.h"
#include "entities/Entity.h"
#include "Root.h"
#include "Messenger.h"

PathFollowComponent::PathFollowComponent(Entity *entity) : InputComponent(entity) {}

COMPONENT PathFollowComponent::getId() { return PATH_FOLLOW_COMPONENT; }

void PathFollowComponent::start() {
    Component::start();

    // TODO: check performance on this
    placeSolidListener = Messenger::on(EventType::PlaceSolid, [this](Event* _e) {
        if (path.empty()) return;

        auto e = static_cast<PlaceSolidEvent*>(_e);

        for (auto& cell : path) {
            if (std::find(e->affectedCells->begin(), e->affectedCells->end(), cell) != e->affectedCells->end()) {
                pathTo(path.back());
                TraceLog(LOG_TRACE, "Path blocked, finding new path");
                return;
            }
        }
    });
}

void PathFollowComponent::end() {
    Messenger::unsubscribe(EventType::PlaceSolid, placeSolidListener);
}

void PathFollowComponent::update() {
    auto& pathfinder = Root::zoo()->world->pathfinder;
    Component::update();

    if (path.empty()) {
        if (pathRequestHandle.empty()) return;
        if (!pathfinder->asyncPathExists(pathRequestHandle)) return;
        if (!pathfinder->asyncPathReady(pathRequestHandle)) {
            return;
        };

        path = pathfinder->getAsyncPath(pathRequestHandle);
        pathRequestHandle = "";

        if (path.empty()) {
            // No valid path
            inputVector = {};
            pathCompleted = true;
            return;
        };
    }

    if (Vector2Distance(entity->pos, getCurrentNode()) < NODE_REACHED_DIST) {
        // TODO (optimisation): should we reverse the path so we can pop?
        path.erase(path.begin());

        if (path.empty()) {
            // Path complete
            path = {};
            inputVector = {};
            pathCompleted = true;
            return;
        }
    }

    inputVector = Vector2Normalize(getCurrentNode() - entity->pos);
}

void PathFollowComponent::pathTo(Vector2 targetPos) {
    path = {};
    pathRequestHandle = Root::zoo()->world->pathfinder->requestAsyncPath(flr(entity->pos), flr(targetPos));
    pathCompleted = false;
}

bool PathFollowComponent::reachedDest () {
    return pathCompleted;
}

Vector2 PathFollowComponent::getCurrentNode() {
    assert(!path.empty());
    return path.front() + Vector2{0.5f, 0.5f};
}

std::vector<Cell>& PathFollowComponent::getPath() {
    return path;
}

void PathFollowComponent::serialise() {
    Component::serialise();

    Root::saveManager().SerialiseValue("path", path);
}
