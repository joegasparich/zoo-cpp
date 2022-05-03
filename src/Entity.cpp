#include "Entity.h"
#include "Game.h"

Entity::Entity(float x, float y) : pos{x, y}, components{}, hasStarted{false} {}

void Entity::setup() {
    hasStarted = true;

    for (auto &component: components) {
        component.second->start(*this);
    }
}

void Entity::update() {
    auto &input = Game::get().m_input;

    float inputHorizontal = (float) input->isInputHeld("D") - (float) input->isInputHeld("A");
    float inputVertical = (float) input->isInputHeld("S") - (float) input->isInputHeld("W");

    pos.x += inputHorizontal * 0.05f;
    pos.y += inputVertical * 0.05f;
    for (auto &component: components) {
        component.second->update();
    }
}

void Entity::render(const double step) const {
    for (auto &component: components) {
        component.second->render(step);
    }
}
