#include "Stage.h"
#include "constants/assets.h"
#include "Game.h"
#include "renderer/Renderer.h"
#include "util/math.h"

#define MIN_ZOOM 0.5
#define MAX_ZOOM 10
#define CAMERA_SPEED 0.1f

Stage::Stage() : m_entities{} {
    m_tools = std::make_unique<ToolManager>();
    // Aim for 200
    m_world = std::make_unique<World>(10, 10);
}

void Stage::setup() {
    m_world->setup();

//    auto player{std::make_unique<Entity>(0, 0)};
//    player->addComponent(std::make_unique<RenderComponent>(AssetManager::getTexture(IMG_SHIP)));
//    auto player2{std::make_unique<Entity>(2, 2)};
//    player2->addComponent(std::make_unique<RenderComponent>(AssetManager::getTexture(IMG_SHIP)));
//
//    m_entities.push_back(std::move(player));
//    m_entities.push_back(std::move(player2));

    for (auto &entity: m_entities) {
        entity->setup();
    }
}

void Stage::update() {
    m_tools->update();
    m_world->update();

    for (auto &entity: m_entities) {
        entity->update();
    }

    // Camera movement
    // TODO: Refactor this out somewhere
    auto &input = Game::get().m_input;
    auto &camera = Renderer::get().m_camera;

    float inputHorizontal = (float) input->isInputHeld("Right") - (float) input->isInputHeld("Left");
    float inputVertical = (float) input->isInputHeld("Down") - (float) input->isInputHeld("Up");

    camera.pos.x += inputHorizontal * CAMERA_SPEED / camera.scale;
    camera.pos.y += inputVertical * CAMERA_SPEED / camera.scale;

    if (input->isInputHeld("ZOOM_IN")) camera.scale = exp(lerp(log(camera.scale), log(MAX_ZOOM), 0.01));
    if (input->isInputHeld("ZOOM_OUT")) camera.scale = exp(lerp(log(camera.scale), log(MIN_ZOOM), 0.01));
}

void Stage::preUpdate() {

}


void Stage::postUpdate() {
    m_world->postUpdate();
    m_tools->postUpdate();
}

void Stage::render(double step) const {
    m_world->render();
    m_tools->render();

    for (auto &entity: m_entities) {
        entity->render(step);
    }
}

void Stage::reset() {}
