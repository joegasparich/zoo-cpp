#pragma once

#include <set>

class Entity;

enum COMPONENT {
    RENDER_COMPONENT
};

class Component {
public:
    COMPONENT id;
    COMPONENT type;
    std::set<COMPONENT> requiredComponents;

    bool disabled;

    Component();

    virtual void start(Entity &_entity);
    virtual void preUpdate();
    virtual void update();
    virtual void postUpdate();
    virtual void render(double step);
    virtual void end();

protected:
    Entity *entity;
    bool hasStarted;
};
