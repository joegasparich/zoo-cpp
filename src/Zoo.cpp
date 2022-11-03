#include "entities/components/RenderComponent.h"
#include "constants/assets.h"
#include "Zoo.h"

std::unique_ptr<Stage> Zoo::zoo = nullptr;

Zoo::Zoo() : Scene("Zoo") {}
Zoo::~Zoo() {}

void Zoo::start() {
    Zoo::zoo = std::make_unique<Stage>();

    Zoo::zoo->setup();
}

void Zoo::preUpdate() {
    Zoo::zoo->preUpdate();
}

void Zoo::update() {
    Zoo::zoo->update();
}

void Zoo::postUpdate() {
    Zoo::zoo->postUpdate();
}

void Zoo::render(double step) {
    Zoo::zoo->render(step);
}

void Zoo::stop() {
    Zoo::zoo->cleanup();
}
