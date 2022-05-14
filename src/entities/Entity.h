#pragma once

#include <SDL2/SDL.h>
#include "pch.h"

#include "components/Component.h"
#include "util/typeMap.h"

class Entity {
public:
    Entity(glm::vec2 pos);

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

    const std::string &getId() const;

private:
    std::string m_id;
    TypeMap<std::unique_ptr<Component>> m_components;
    bool m_hasStarted;
};

#include "Entity.tpp"
