#include "Entity.h"
#include "Debug.h"
#include "util/util.h"
#include "entities/components/createComponentById.h"
#include "Root.h"

Entity::Entity() : pos{0}, components{}, hasStarted{false} {}
Entity::Entity(Vector2 pos) : pos{pos}, components{}, hasStarted{false} {}
Entity::~Entity() {
    cleanup();
};

void Entity::setup() {
    hasStarted = true;

    for (auto &component: components) {
        component.second->start();
    }
}

void Entity::preUpdate() {
    for (auto &component: components) {
        component.second->preUpdate();
    }
}

void Entity::update() {
    for (auto &component: components) {
        component.second->update();
    }
}

void Entity::postUpdate() {
    for (auto &component: components) {
        component.second->postUpdate();
    }
}

void Entity::render(const double step) const {
    for (auto &component: components) {
        component.second->render(step);
    }

    // Debug rendering
    // TODO: Toggleable
    Debug::drawLine(pos - Vector2{0.25f, 0.25f}, pos + Vector2{0.25f, 0.25f}, Color{255, 0, 0, 255}, true);
    Debug::drawLine(pos - Vector2{-0.25f, 0.25f}, pos + Vector2{-0.25f, 0.25f}, Color{255, 0, 0, 255}, true);
}

void Entity::cleanup () {
    for (auto &component: components) {
        component.second->end();
    }
    components.clear();
}

void Entity::destroy () {
    Root::zoo()->unregisterEntity(id);
}

bool Entity::hasComponent(COMPONENT type) {
    bool contains = false;
    for (auto &component: components) {
        if (component.second->getId() == type) contains = true;
    }
    return contains;
}

void Entity::serialise() {
    Root::saveManager().SerialiseValue("id", id);
    Root::saveManager().SerialiseValue("pos", pos);
    SerialiseComponentMap("components", components);
}

void Entity::SerialiseComponentMap(const std::string& label, TypeMap<std::unique_ptr<Component>>& collection) {
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
        for (auto data : parent->at(label).get<std::vector<json>>()) {
            auto component = createComponentById(data.at("id").get<COMPONENT>(), this);
            Root::saveManager().setCurrentSerialiseNode(&data);
            component->serialise();
        }
    }

    Root::saveManager().setCurrentSerialiseNode(parent);
}
