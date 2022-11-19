#include "Zoo.h"
#include "entities/components/RenderComponent.h"
#include "constants/assets.h"
#include "entities/entityGenerators.h"

Zoo::Zoo() : Scene(ZOO) {}
Zoo::~Zoo() {}

void Zoo::start() {
    zoo = std::make_unique<Stage>();

    zoo->setup();

    // temp
    auto test = std::make_unique<Entity>();
    test->addComponent(std::make_unique<RenderComponent>(
        test.get(),
        IMG_KEEPER
    ));
    zoo->registerEntity(std::move(test));

    auto building = createTileObject(OBJ_BUILDING, {5, 5});
    zoo->registerEntity(std::move(building));
}

void Zoo::preUpdate() {
    zoo->preUpdate();
}

void Zoo::update() {
    zoo->update();
}

void Zoo::postUpdate() {
    zoo->postUpdate();
}

void Zoo::render(double step) {
    zoo->render(step);
}

void Zoo::onGUI() {
    zoo->onGUI();
}

void Zoo::onInput(InputEvent* event) {
    zoo->onInput(event);
}

void Zoo::stop() {
    zoo->cleanup();
}
