#include "PathFollowComponent.h"
#include "entities/Entity.h"
#include "Root.h"

#define NODE_REACHED_DIST 0.5f

PathFollowComponent::PathFollowComponent(Entity *entity) : InputComponent(entity) {}

COMPONENT PathFollowComponent::getId() { return PATH_FOLLOW_COMPONENT; }

void PathFollowComponent::pathTo(Vector2 targetPos) {
    path = Root::zoo()->world->pathfinder->getPath(flr(entity->pos), flr(targetPos));
}

void PathFollowComponent::update() {
    Component::update();

    if (path.empty()) return;

    if (Vector2Distance(entity->pos, getCurrentNode()) < NODE_REACHED_DIST) {
        // TODO (optimisation): should we reverse the path so we can pop?
        path.erase(path.begin());

        if (path.empty()) {
            // Path complete
            path = {};
            inputVector = {};
            return;
        }
    }

    inputVector = Vector2Normalize(getCurrentNode() - entity->pos);
}

Vector2 PathFollowComponent::getCurrentNode() {
    return path.front() + Vector2{0.5f, 0.5f};
}

std::vector<Cell>& PathFollowComponent::getPath() {
    return path;
}
