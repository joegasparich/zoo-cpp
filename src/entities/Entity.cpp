#include "Game.h"
#include "Entity.h"
#include "Debug.h"
#include "util/util.h"
#include "entities/components/createComponentById.h"

Entity::Entity() : pos{0}, components{}, hasStarted{false} {}
Entity::Entity(Vector2 pos) : pos{pos}, components{}, hasStarted{false} {}
Entity::~Entity() = default;

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

void Entity::setId(unsigned int _id) {
    id = _id;
}

unsigned int Entity::getId() {
    return id;
}

json Entity::save() {
    std::vector<json> componentSaveData{};
    for(auto& component : components) {
       componentSaveData.push_back(component.second->save());
    }

    return json({
        {"id", getId()},
        {"pos", pos},
        {"components", componentSaveData}
    });
}

void Entity::load(json data) {
    data.at("pos").get_to(pos);

    for (auto componentData : data.at("components").get<std::vector<json>>()) {
        auto id = componentData.at("id").get<COMPONENT>();
        auto component = createComponentById(id, this);
        component->load(componentData);
    }
}

bool Entity::hasComponent(COMPONENT type) {
    bool contains = false;
    for (auto &component: components) {
        if (component.second->getId() == type) contains = true;
    }
    return contains;
}
