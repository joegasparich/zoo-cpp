#pragma once

#include "pch.h"

#include "components/Component.h"
#include "util/typeMap.h"

class Entity {
public:
    Entity();
    Entity(glm::vec2 pos);
    ~Entity();

    glm::vec2 m_pos;

    void setup();
    void update();
    void render(double step) const;

    template<typename T>
    T* addComponent(std::unique_ptr<T> component);
    template<typename T>
    T* getComponent() const;
    template<typename T>
    bool hasComponent() const;
    bool hasComponent(COMPONENT type);

    void setId(unsigned int id);
    unsigned int getId();

    json save();
    void load(json data);

private:
    unsigned int m_id;
    TypeMap<std::unique_ptr<Component>> m_components;
    bool m_hasStarted;
};

#include "Entity.tpp"
