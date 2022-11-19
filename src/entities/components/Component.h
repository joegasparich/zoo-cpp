#pragma once

#include "common.h"

class Entity;

enum COMPONENT {
    RENDER_COMPONENT,
    TILE_OBJECT_COMPONENT,
    ELEVATION_COMPONENT
};

class Component {
public:
    virtual COMPONENT getId() = 0;
    virtual COMPONENT getType() = 0;
    virtual std::set<COMPONENT> getRequiredComponents() = 0;

    bool disabled = false;

    Component(Entity* entity);

    virtual void start();
    virtual void preUpdate();
    virtual void update();
    virtual void postUpdate();
    virtual void render(double step);
    virtual void end();

    virtual json save();
    virtual void load(json data);

protected:
    Entity* entity;
    bool hasStarted;
};
