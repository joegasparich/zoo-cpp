#include "Stage.h"
#include "Root.h"
#include "constants/world.h"
#include "UIManager.h"
#include "Profiler.h"

#define MIN_ZOOM 0.5f
#define MAX_ZOOM 10.0f
#define ZOOM_RATE 0.005f
#define CAMERA_SPEED 2

// TODO: This should probably be renamed Zoo and Zoo to ZooScene
Stage::Stage() :
    entities{},
    entitiesToAdd{},
    entitiesToDelete{}
{
    tools = std::make_unique<ToolManager>();
    // Aim for 200
    world = std::make_unique<World>(10, 10);
}

Stage::~Stage() {
    cleanup();
}

void Stage::setup() {
    world->setup();

    for (auto& pair: entities) {
        pair.second->setup();
    }

    tools->setup();

    auto &camera = Root::renderer().camera;
    dragStart = GetMousePosition();
    dragCameraOrigin = camera.target;
    camera.offset = {SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
}

void Stage::update() {
    tools->update();
    world->update();

    for (auto& pair: entities) {
        pair.second->update();
    }

    // TODO: Refactor this out somewhere and use input manager
    // Camera movement
    auto &camera = Root::renderer().camera;

    // TODO: Why is there a lag spike when you first start moving the camera
    float inputHorizontal = float(IsKeyDown(KEY_RIGHT)) - float(IsKeyDown(KEY_LEFT));
    float inputVertical = float(IsKeyDown(KEY_DOWN)) - float(IsKeyDown(KEY_UP));

    camera.target.x += inputHorizontal * CAMERA_SPEED / camera.zoom;
    camera.target.y += inputVertical * CAMERA_SPEED / camera.zoom;

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
        dragStart = GetMousePosition();
        dragCameraOrigin = camera.target;
    }
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        camera.target = dragCameraOrigin + (dragStart - GetMousePosition()) / (camera.zoom * WORLD_SCALE);
    }

    // Camera zoom
    float zoomDelta = GetMouseWheelMove() / 10.0f;

    if (IsKeyDown(KEY_COMMA)) zoomDelta += ZOOM_RATE;
    if (IsKeyDown(KEY_PERIOD)) zoomDelta -= ZOOM_RATE;

    // TODO: Zoom towards mouse

    // Do zoom
    auto normalisedZoomLog = normalise(log(camera.zoom), log(MIN_ZOOM), log(MAX_ZOOM));
    camera.zoom = exp(lerp(log(MIN_ZOOM), log(MAX_ZOOM), normalisedZoomLog + zoomDelta));
    camera.zoom = Clamp(camera.zoom, MIN_ZOOM, MAX_ZOOM);
}

void Stage::preUpdate() {

}

void Stage::postUpdate() {
    world->postUpdate();
    tools->postUpdate();

    for (auto& entity : entitiesToAdd) {
        entity->setup();
        entities.insert_or_assign(entity->getId(), std::move(entity));
    }

    entitiesToAdd.clear();
}

void Stage::render(double step) const {
    Profiler::startTimer("world");
    world->render();
    Profiler::stopTimer("world");

    Profiler::startTimer("entities");
    for (auto& pair : entities) {
        pair.second->render(step);
    }
    Profiler::stopTimer("entities");

    Profiler::startTimer("tools");
    tools->render();
    Profiler::stopTimer("tools");
}

void Stage::onGUI() {
    tools->onGUI();
}

void Stage::onInput(InputEvent* event) {
    tools->onInput(event);
}

void Stage::cleanup() {
    tools->cleanup();
    world->cleanup();

    entities.clear();
    entitiesToAdd.clear();
    entitiesToDelete.clear();

    Root::renderer().resetCamera();
}

json Stage::save() {
    std::vector<json> entitiesSaveData;

    for (auto& pair : entities) {
        entitiesSaveData.push_back(pair.second->save());
    }

    return json{
        { "world", world->save() },
        { "entities", json{
            { "nextEntityId", nextEntityId },
            { "entityList", entitiesSaveData }
        }}
    };
}

void Stage::load(json data) {
    cleanup();
    setup();

    world->load(data.at("world"));

    json entitiesData = data.at("entities");
    entitiesData.at("nextEntityId").get_to(nextEntityId);
    for (auto& entityData : entitiesData.at("entityList").get<std::vector<json>>()) {
        auto entity = std::make_unique<Entity>();
        entity->load(entityData);
        registerEntity(std::move(entity), entityData.at("id").get<unsigned int>());
    }
}

unsigned int Stage::registerEntity(std::unique_ptr<Entity> entity) {
    return registerEntity(std::move(entity), nextEntityId++);
}

unsigned int Stage::registerEntity(std::unique_ptr<Entity> entity, unsigned int id) {
    entity->setId(id);
    entitiesToAdd.push_back(std::move(entity));

    std::cout << "Registered entity with id " << id << std::endl;

    return id;
}

void Stage::unregisterEntity(unsigned int entityId) {
    // TODO: Implement
}

Entity *Stage::getEntityById(unsigned int entityId) {
    auto& ptr = entities.at(entityId);
    if (ptr) return ptr.get();
    return nullptr;
}
