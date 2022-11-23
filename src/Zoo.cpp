#include "Zoo.h"
#include "Root.h"
#include "constants/world.h"
#include "UIManager.h"
#include "Profiler.h"
#include "Debug.h"

#define MIN_ZOOM 0.5f
#define MAX_ZOOM 10.0f
#define ZOOM_RATE 0.005f
#define CAMERA_SPEED 2

Zoo::Zoo() :
    entities{},
    entitiesToAdd{},
    entitiesToDelete{}
{
    tools = std::make_unique<ToolManager>();
    // Aim for 200
    world = std::make_unique<World>(10, 10);
}

Zoo::~Zoo() {
    cleanup();
}

void Zoo::setup() {
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

void Zoo::preUpdate() {
    for (auto& pair: entities) {
        pair.second->preUpdate();
    }
}

void Zoo::update() {
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
void Zoo::postUpdate() {
    world->postUpdate();
    tools->postUpdate();

    for (auto& pair: entities) {
        pair.second->postUpdate();
    }

    for (auto& entity : entitiesToAdd) {
        entity->setup();
        entities.insert_or_assign(entity->getId(), std::move(entity));
    }

    entitiesToAdd.clear();
}

void Zoo::render(double step) const {
    Profiler::startTimer("world");
    world->render();
    Profiler::stopTimer("world");

    Profiler::startTimer("entities");
    for (auto& pair : entities) {
        pair.second->render(step);
    }
    Profiler::stopTimer("entities");
}

void Zoo::renderLate(double step) const {
    Profiler::startTimer("tools");
    tools->render();
    Profiler::stopTimer("tools");

    Profiler::startTimer("debug");
    world->renderDebug();
    Profiler::stopTimer("debug");
}

void Zoo::onGUI() {
    tools->onGUI();
}

void Zoo::onInput(InputEvent* event) {
    tools->onInput(event);

    if (event->consumed) return;
}

void Zoo::cleanup() {
    tools->cleanup();
    world->cleanup();

    entities.clear();
    entitiesToAdd.clear();
    entitiesToDelete.clear();

    Root::renderer().resetCamera();
}

unsigned int Zoo::registerEntity(std::unique_ptr<Entity> entity) {
    return registerEntity(std::move(entity), nextEntityId++);
}

unsigned int Zoo::registerEntity(std::unique_ptr<Entity> entity, unsigned int id) {
    entity->setId(id);
    entitiesToAdd.push_back(std::move(entity));

    std::cout << "Registered entity with id " << id << std::endl;

    return id;
}

void Zoo::unregisterEntity(unsigned int entityId) {
    // TODO: Implement
}

Entity *Zoo::getEntityById(unsigned int entityId) {
    if (entities.empty() || entities.size() < entityId)
        return nullptr;

    auto& ptr = entities.at(entityId);
    if (ptr) return ptr.get();
    return nullptr;
}

void Zoo::serialise() {
    if (Root::saveManager().mode == SerialiseMode::Loading) {
        cleanup();
        // TODO: this is basically cleaning up and setting up twice
        // Once to make a new save and another to load the file
        // Fix me
        setup();
    }

    Root::saveManager().SerialiseDeep("world", world.get());
    SerialiseEntities("entities", entities);
    Root::saveManager().SerialiseValue("nextEntityId", nextEntityId);
}

void Zoo::SerialiseEntities (const std::string& label, std::unordered_map<unsigned int, std::unique_ptr<Entity>>& collection) {
    auto parent = Root::saveManager().getCurrentSerialiseNode();

    if (Root::saveManager().mode == SerialiseMode::Saving) {
        json saveData{};
        for (auto& pair : collection) {
            json data{};
            Root::saveManager().setCurrentSerialiseNode(&data);
            pair.second->serialise();
            saveData[pair.first] = data;
        }
        (*parent)[label] = saveData;
    }
    if (Root::saveManager().mode == SerialiseMode::Loading) {
        for (auto& data : parent->at(label).get<json>()) {
            auto entity = std::make_unique<Entity>();
            Root::saveManager().setCurrentSerialiseNode(&data);
            entity->serialise();
            auto id = entity->getId();
            Root::zoo()->registerEntity(std::move(entity), id);
        }
    }

    Root::saveManager().setCurrentSerialiseNode(parent);
}