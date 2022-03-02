#include "Stage.h"
#include "Entity.h"
#include "components/RenderComponent.h"
#include "AssetManager.h"
#include "constants/assets.h"
#include "Game.h"
#include "renderer/Renderer.h"
#include "util/math.h"

#define MIN_ZOOM 0.5
#define MAX_ZOOM 10
#define CAMERA_SPEED 5

Stage::Stage() : entities{} {}

void Stage::setup() {
    auto player{std::make_unique<Entity>(0, 0)};
    player->addComponent(std::make_unique<RenderComponent>(AssetManager::getTexture(IMG_SHIP)));
    auto player2{std::make_unique<Entity>(200, 200)};
    player2->addComponent(std::make_unique<RenderComponent>(AssetManager::getTexture(IMG_SHIP)));

    entities.push_back(std::move(player));
    entities.push_back(std::move(player2));

    for (auto &entity: entities) {
        entity->setup();
    }
}

void Stage::update() {
    for (auto &entity: entities) {
        entity->update();
    }

    // TODO: Refactor this out somewhere
    auto &input = Game::get().input;
    auto &camera = Renderer::get().m_camera;

    float inputHorizontal = (float) input->isInputHeld("Right") - (float) input->isInputHeld("Left");
    float inputVertical = (float) input->isInputHeld("Down") - (float) input->isInputHeld("Up");

    camera.pos.x += inputHorizontal * CAMERA_SPEED / camera.scale;
    camera.pos.y += inputVertical * CAMERA_SPEED / camera.scale;

    if (input->isInputHeld("ZOOM_IN")) camera.scale = exp(lerp(log(camera.scale), log(MAX_ZOOM), 0.01));
    if (input->isInputHeld("ZOOM_OUT")) camera.scale = exp(lerp(log(camera.scale), log(MIN_ZOOM), 0.01));
}

void Stage::render(double step) const {
    for (auto &entity: entities) {
        entity->render(step);
    }
}
