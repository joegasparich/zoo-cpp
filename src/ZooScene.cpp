#include "ZooScene.h"
#include "entities/components/RenderComponent.h"
#include "constants/assets.h"
#include "entities/entityGenerators.h"

ZooScene::ZooScene() : Scene(ZOO) {}
ZooScene::~ZooScene() {}

void ZooScene::start() {
    zoo = std::make_unique<Zoo>();

    zoo->setup();

//    // temp
//    auto test = std::make_unique<Entity>();
//    test->addComponent(std::make_unique<RenderComponent>(
//        test.get(),
//        IMG_KEEPER
//    ));
//    zoo->registerEntity(std::move(test));
//
//    auto building = createTileObject(OBJ_BUILDING, {5, 5});
//    zoo->registerEntity(std::move(building));
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

void ZooScene::onGUI() {
    zoo->onGUI();
}

void ZooScene::onInput(InputEvent* event) {
    zoo->onInput(event);
}

void ZooScene::stop() {
    zoo->cleanup();
}
