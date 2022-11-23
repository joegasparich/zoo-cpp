#pragma once

#include "common.h"
#include "ISerialisable.h"

class Entity;

enum COMPONENT {
    INPUT_COMPONENT,
    RENDER_COMPONENT,
    TILE_OBJECT_COMPONENT,
    ELEVATION_COMPONENT,
    PHYSICS_COMPONENT,
    MOVE_COMPONENT,
    PATH_FOLLOW_COMPONENT
};

class Component: public ISerialisable {
public:
    Component(Entity* entity);
    virtual COMPONENT getId() = 0;

    virtual void start();
    virtual void preUpdate();
    virtual void update();
    virtual void postUpdate();
    virtual void render(double step);
    virtual void end();

    void serialise() override;

    bool disabled = false;
protected:
    Entity* entity;
    bool hasStarted;
};
