#include <constants/world.h>
#include <util/uuid.h>
#include "Stage.h"
#include "constants/assets.h"
#include "Game.h"
#include "renderer/Renderer.h"
#include "util/math.h"
#include "AssetManager.h"
#include "entities/entityGenerators.h"

#define MIN_ZOOM 0.5f
#define MAX_ZOOM 10.0f
#define CAMERA_SPEED 0.1f

Stage::Stage() :
    m_entities{},
    m_entitiesToAdd{},
    m_entitiesToDelete{}
{
    m_tools = std::make_unique<ToolManager>();
    // Aim for 200
    m_world = std::make_unique<World>(10, 10);
}

Stage::~Stage() {
    reset();
}

void Stage::setup() {
    m_world->setup();

    auto player{std::make_unique<Entity>(glm::vec2{1, 1})};
    player->addComponent(std::make_unique<RenderComponent>(player.get(), AssetManager::loadTexture(AssetManager::getImage(IMG_KEEPER))));

    registerEntity(std::move(player));

    for (auto& pair: m_entities) {
        pair.second->setup();
    }

    m_tools->setup();

    auto &input = Game::get().m_input;
    auto &camera = Renderer::get().m_camera;
    m_dragStart = input->getMousePos();
    m_dragCameraOrigin = camera.pos;
}

void Stage::update() {
    m_tools->update();
    m_world->update();

    for (auto& pair: m_entities) {
        pair.second->update();
    }

    // TODO: Refactor this out somewhere
    // Camera movement
    auto &input = Game::get().m_input;
    auto &camera = Renderer::get().m_camera;

    float inputHorizontal = (float) input->isInputHeld("Right") - (float) input->isInputHeld("Left");
    float inputVertical = (float) input->isInputHeld("Down") - (float) input->isInputHeld("Up");

    camera.pos.x += inputHorizontal * CAMERA_SPEED / camera.scale;
    camera.pos.y += inputVertical * CAMERA_SPEED / camera.scale;

    if (input->isMouseButtonDown(SDL_BUTTON_MIDDLE)) {
        m_dragStart = input->getMousePos();
        m_dragCameraOrigin = camera.pos;
    }
    if (input->isMouseButtonHeld(SDL_BUTTON_MIDDLE)) {
        camera.pos = m_dragCameraOrigin + (m_dragStart - input->getMousePos()) / (camera.scale * WORLD_SCALE);
    }

    // Camera zoom
    if (input->isInputHeld("ZOOM_IN")) camera.scale = exp(lerp(log(camera.scale), log(MAX_ZOOM), 0.01));
    if (input->isInputHeld("ZOOM_OUT")) camera.scale = exp(lerp(log(camera.scale), log(MIN_ZOOM), 0.01));

    auto scroll = input->getMouseScroll();
    while (scroll != 0) {
        auto dir = glm::sign(scroll);
        auto mouseWorldPos = Renderer::screenToWorldPos(input->getMousePos());
        auto oldScale = camera.scale;
        camera.scale = glm::clamp(camera.scale + ((float)dir * log(camera.scale + 1) * 0.05f), MIN_ZOOM, MAX_ZOOM);
        // TODO: figure out how to reposition in order to maintain previous mouse world pos
        if (camera.scale != oldScale) {
            camera.pos += (mouseWorldPos - camera.pos) * 0.03f * (float)dir;
        }
        scroll -= dir;
    }
}

void Stage::preUpdate() {

}


void Stage::postUpdate() {
    m_world->postUpdate();
    m_tools->postUpdate();

    for (auto& entity : m_entitiesToAdd) {
        entity->setup();
        m_entities.insert({entity->getId(), std::move(entity)});
    }

    m_entitiesToAdd.clear();
}

void Stage::render(double step) const {
    m_world->render();
    m_tools->render();

    for (auto& pair : m_entities) {
        pair.second->render(step);
    }
}

void Stage::reset() {
    m_tools->reset();
    m_world->reset();

    Renderer::resetCamera();
}

std::string Stage::registerEntity(std::unique_ptr<Entity> entity) {
    auto id = uuid::generate();
    entity->setId(id);
    m_entitiesToAdd.push_back(std::move(entity));

    return id;
}

void Stage::unregisterEntity(std::string entityId) {
    // TODO: Implement
}

Entity *Stage::getEntityById(std::string entityId) {
    auto& ptr = m_entities.at(entityId);
    if (ptr) return ptr.get();
    return nullptr;
}
