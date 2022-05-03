#pragma once

#include <SDL2/SDL.h>
#include "glm/vec2.hpp"

#include "components/Component.h"
#include "util/typeMap.h"

class Entity {
public:
    Entity(float x, float y);

    glm::vec2 pos;

    void setup();

    void update();

    void render(double step) const;

    template<typename T>
    void addComponent(std::unique_ptr<T> component);

    template<typename T>
    T *getComponent(T) const;

private:
    TypeMap<std::unique_ptr<Component>> components;
    bool hasStarted;
};

#include "Entity.tpp"
