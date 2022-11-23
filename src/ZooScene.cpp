#include "ZooScene.h"
#include "entities/components/RenderComponent.h"
#include "constants/assets.h"
#include "entities/components/PhysicsComponent.h"
#include "entities/components/MoveComponent.h"
#include "entities/components/AreaPathFollowComponent.h"
#include "Debug.h"

ZooScene::ZooScene() : Scene(ZOO) {}
ZooScene::~ZooScene() {}

void ZooScene::start() {
    TraceLog(LOG_TRACE, "Creating New Zoo");
    zoo = std::make_unique<Zoo>();

    zoo->setup();

    // temp
    auto test = std::make_unique<Entity>();
    test->addComponent(std::make_unique<RenderComponent>(
        test.get(),
        IMG_KEEPER
    ));
    test->addComponent(std::make_unique<AreaPathFollowComponent>(test.get()));
    test->addComponent(std::make_unique<PhysicsComponent>(test.get()));
    test->addComponent(std::make_unique<MoveComponent>(test.get()));
    manId = zoo->registerEntity(std::move(test));
}

void ZooScene::preUpdate() {
    zoo->preUpdate();
}

void ZooScene::update() {
    zoo->update();
}

void ZooScene::postUpdate() {
    zoo->postUpdate();
}

void ZooScene::render(double step) {
    zoo->render(step);
}

void ZooScene::renderLate(double step) {
    zoo->renderLate(step);

    // Temp
    auto man = zoo->getEntityById(manId);
    if (!man) return;
    auto path = man->getComponent<AreaPathFollowComponent>()->getPath();
    if (!path.empty()) {
        for (int i = 1; i < path.size(); i++) {
            Debug::drawLine(
                path[i -1] + Vector2{0.5f, 0.5f},
                path[i] + Vector2{0.5f, 0.5f},
                RED,
                true
            );
        }
    }
}

void ZooScene::onGUI() {
    zoo->onGUI();
}

void ZooScene::onInput(InputEvent* event) {
    zoo->onInput(event);

    if (event->consumed) return;

    if (event->mouseButtonDown == MOUSE_BUTTON_LEFT) {
        zoo->getEntityById(manId)->getComponent<AreaPathFollowComponent>()->pathTo(event->mouseWorldPos);
    }
}

void ZooScene::stop() {
    zoo->cleanup();
}
