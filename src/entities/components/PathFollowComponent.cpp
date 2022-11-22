#include "PathFollowComponent.h"
#include "entities/Entity.h"
#include "Root.h"
#include "Messenger.h"

#define NODE_REACHED_DIST 0.5f

PathFollowComponent::PathFollowComponent(Entity *entity) : InputComponent(entity) {}

COMPONENT PathFollowComponent::getId() { return PATH_FOLLOW_COMPONENT; }

void PathFollowComponent::start() {
    Component::start();

    // TODO: check performance on this
    placeSolidListener = Messenger::on(EventType::PlaceSolid, [this](Event* _e) {
        auto e = static_cast<PlaceSolidEvent*>(_e);

        for (auto& cell : path) {
            if (std::find(e->affectedCells->begin(), e->affectedCells->end(), cell) != e->affectedCells->end()) {
                pathTo(path.back());
                std::cout << "Path blocked, finding new path" << std::endl;
                return;
            }
        }
    });
}

void PathFollowComponent::end() {
    Messenger::unsubscribe(EventType::PlaceSolid, placeSolidListener);
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

void PathFollowComponent::pathTo(Vector2 targetPos) {
    path = Root::zoo()->world->pathfinder->getPath(flr(entity->pos), flr(targetPos));
}

Vector2 PathFollowComponent::getCurrentNode() {
    return path.front() + Vector2{0.5f, 0.5f};
}

std::vector<Cell>& PathFollowComponent::getPath() {
    return path;
}