#pragma once

#include "common.h"

#include "components/Component.h"
#include "util/typeMap.h"
#include "ISerialisable.h"

class Entity : public ISerialisable {
public:
    Entity();
    Entity(Vector2 pos);
    ~Entity();

    unsigned int id = 0;
    Vector2 pos = {0, 0};

    void setup();
    void preUpdate();
    void update();
    void postUpdate();
    void render(double step) const;
    void cleanup();

    void destroy();

    template<typename T>
    T* addComponent(std::unique_ptr<T> component);
    template<typename T>
    T* getComponent() const;
    template<typename T>
    bool hasComponent() const;
    bool hasComponent(COMPONENT type);

    void serialise() override;
    void SerialiseComponentMap(const std::string& label, TypeMap<std::unique_ptr<Component>>& collection);

private:
    TypeMap<std::unique_ptr<Component>> components;
    TypeMap<Component*> baseComponents;
    bool hasStarted;
};

#include "Entity.tpp"
