#pragma once

#include "common.h"

#include "components/Component.h"
#include "util/typeMap.h"

class Entity {
public:
    Entity();
    Entity(Vector2 pos);
    ~Entity();

    Vector2 pos;

    void setup();
    void preUpdate();
    void update();
    void postUpdate();
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
    unsigned int id;
    TypeMap<std::unique_ptr<Component>> components;
    TypeMap<Component*> baseComponents;
    bool hasStarted;
};

#include "Entity.tpp"
