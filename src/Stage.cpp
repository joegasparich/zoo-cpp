#include <constants/world.h>
#include <util/uuid.h>
#include "Stage.h"
#include "constants/assets.h"
#include "Game.h"
#include "Debug.h"
#include "gfx/Renderer.h"
#include "util/jmath.h"
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
    cleanup();
}

void Stage::setup() {
    m_world->setup();

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
    if (input->isInputHeld("ZOOM_IN")) camera.scale = exp(jmath::lerp(log(camera.scale), log(MAX_ZOOM), 0.01));
    if (input->isInputHeld("ZOOM_OUT")) camera.scale = exp(jmath::lerp(log(camera.scale), log(MIN_ZOOM), 0.01));

    auto scroll = input->getMouseScroll();
    while (scroll != 0) {
        auto dir = glm::sign(scroll);
        auto mouseWorldPos = Renderer::screenToWorldPos(input->getMousePos());
        auto oldScale = camera.scale;
        camera.scale = glm::clamp((float)(camera.scale + ((float)dir * log(camera.scale + 1) * 0.05f)), MIN_ZOOM, MAX_ZOOM);
        // TODO: figure out how to reposition in order to maintain previous mouse world pos
        if (camera.scale != oldScale) {
            camera.pos += (mouseWorldPos - camera.pos) * 0.03f * (float)dir;
        }
        scroll -= dir;
    }

    // if (input->isMouseButtonDown(SDL_BUTTON_LEFT)) {
    //     std::cout << Renderer::pick(glm::floor(input->getMousePos())) << std::endl;
    // }
}

void Stage::preUpdate() {

}


void Stage::postUpdate() {
    m_world->postUpdate();
    m_tools->postUpdate();

    for (auto& entity : m_entitiesToAdd) {
        entity->setup();
        m_entities.insert_or_assign(entity->getId(), std::move(entity));
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

void Stage::cleanup() {
    m_tools->cleanup();
    m_world->cleanup();

    m_entities.clear();
    m_entitiesToAdd.clear();
    m_entitiesToDelete.clear();

    Renderer::resetCamera();
}

json Stage::save() {
    std::vector<json> entitiesSaveData;

    for (auto& pair : m_entities) {
        entitiesSaveData.push_back(pair.second->save());
    }

    return json{
        { "world", m_world->save() },
        { "entities", json{
            { "nextEntityId", m_nextEntityId },
            { "entityList", entitiesSaveData }
        }}
    };
}

void Stage::load(json data) {
    cleanup();
    setup();

    m_world->load(data.at("world"));

    json entitiesData = data.at("entities");
    entitiesData.at("nextEntityId").get_to(m_nextEntityId);
    for (auto& entityData : entitiesData.at("entityList").get<std::vector<json>>()) {
        auto entity = std::make_unique<Entity>();
        entity->load(entityData);
        registerEntity(std::move(entity), entityData.at("id").get<unsigned int>());
    }
}

unsigned int Stage::registerEntity(std::unique_ptr<Entity> entity) {
    return registerEntity(std::move(entity), m_nextEntityId++);
}

unsigned int Stage::registerEntity(std::unique_ptr<Entity> entity, unsigned int id) {
    entity->setId(id);
    m_entitiesToAdd.push_back(std::move(entity));

    std::cout << "Registered entity with id " << id << std::endl;

    return id;
}

void Stage::unregisterEntity(unsigned int entityId) {
    // TODO: Implement
}

Entity *Stage::getEntityById(unsigned int entityId) {
    auto& ptr = m_entities.at(entityId);
    if (ptr) return ptr.get();
    return nullptr;
}
