#include "Zoo.h"
#include "Root.h"
#include "UIManager.h"
#include "Profiler.h"

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
}

void Zoo::preUpdate() {
    tools->preUpdate();
    world->preUpdate();

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
}
void Zoo::postUpdate() {
    world->postUpdate();
    tools->postUpdate();

    for (auto& pair: entities) {
        pair.second->postUpdate();
    }

    for (auto& entity : entitiesToAdd) {
        entity->setup();
        entities.insert_or_assign(entity->id, std::move(entity));
    }
    entitiesToAdd.clear();

    for (auto id : entitiesToDelete) {
        entities.erase(id);
    }
    entitiesToDelete.clear();
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
    entities.clear();
    entitiesToAdd.clear();
    entitiesToDelete.clear();

    tools->cleanup();
    world->cleanup();

    Root::renderer().resetCamera();
}

unsigned int Zoo::registerEntity(std::unique_ptr<Entity> entity) {
    return registerEntity(std::move(entity), nextEntityId++);
}

unsigned int Zoo::registerEntity(std::unique_ptr<Entity> entity, unsigned int id) {
    entity->id = id;
    entitiesToAdd.push_back(std::move(entity));

    TraceLog(LOG_TRACE, "Registered entity with id: %u", id);

    return id;
}

void Zoo::unregisterEntity(unsigned int entityId) {
    entitiesToDelete.push_back(entityId);
}

Entity *Zoo::getEntityById(unsigned int entityId) {
    if (entities.empty())
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
    // TODO: Do we need to clear entitiesToAdd and entitiesToDelete first?
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
            saveData.push_back(data);
        }
        (*parent)[label] = saveData;
    }
    if (Root::saveManager().mode == SerialiseMode::Loading) {
        loadEntitiesFromJson(parent->at(label).get<json>());
    }

    Root::saveManager().setCurrentSerialiseNode(parent);
}

void Zoo::loadEntitiesFromJson (json data) {
    auto prevNode = Root::saveManager().getCurrentSerialiseNode();
    for (auto& entitiyData : data) {
        auto entity = std::make_unique<Entity>();
        Root::saveManager().setCurrentSerialiseNode(&entitiyData);
        entity->serialise();
        auto id = entity->id;
        Root::zoo()->registerEntity(std::move(entity), id);
    }
    Root::saveManager().setCurrentSerialiseNode(prevNode);
}
