#include <Debug.h>
#include <util/util.h>
#include <entities/components/createComponentById.h>
#include "Entity.h"
#include "Game.h"

Entity::Entity() : m_pos{}, m_components{}, m_hasStarted{false} {}
Entity::Entity(glm::vec2 pos) : m_pos{pos}, m_components{}, m_hasStarted{false} {}
Entity::~Entity() = default;

void Entity::setup() {
    m_hasStarted = true;

    for (auto &component: m_components) {
        component.second->start();
    }
}

void Entity::update() {
    auto &input = Game::get().m_input;

    float inputHorizontal = (float) input->isInputHeld("D") - (float) input->isInputHeld("A");
    float inputVertical = (float) input->isInputHeld("S") - (float) input->isInputHeld("W");

    m_pos.x += inputHorizontal * 0.05f;
    m_pos.y += inputVertical * 0.05f;
    for (auto &component: m_components) {
        component.second->update();
    }
}

void Entity::render(const double step) const {
    for (auto &component: m_components) {
        component.second->render(step);
    }

    // Debug rendering
    // TODO: Toggleable
    Debug::drawLine(m_pos - glm::vec2{0.25f, 0.25f}, m_pos + glm::vec2{0.25f, 0.25f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    Debug::drawLine(m_pos - glm::vec2{-0.25f, 0.25f}, m_pos + glm::vec2{-0.25f, 0.25f}, glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
}

void Entity::setId(unsigned int id) {
    m_id = id;
}

unsigned int Entity::getId() {
    return m_id;
}

json Entity::save() {
    std::vector<json> componentSaveData{};
    for(auto& component : m_components) {
       componentSaveData.push_back(component.second->save());
    }

    return json({
        {"id",  getId()},
        {"pos", m_pos},
        {"components", componentSaveData}
    });
}

void Entity::load(json data) {
    data.at("pos").get_to(m_pos);

    for (auto componentData : data.at("components").get<std::vector<json>>()) {
        auto id = componentData.at("id").get<COMPONENT>();
        auto component = createComponentById(id, this);
        component->load(componentData);
    }
}

bool Entity::hasComponent(COMPONENT type) {
    bool contains = false;
    for (auto &component: m_components) {
        if (component.second->getId() == type) contains = true;
    }
    return contains;
}
