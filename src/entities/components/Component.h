#pragma once

#include "pch.h"

class Entity;

enum COMPONENT {
    RENDER_COMPONENT,
    TILE_OBJECT_COMPONENT
};

class Component {
public:
    virtual COMPONENT getId() = 0;
    virtual COMPONENT getType() = 0;
    virtual std::set<COMPONENT> getRequiredComponents() = 0;

    bool m_disabled;

    Component();

    virtual void start(Entity &_entity);
    virtual void preUpdate();
    virtual void update();
    virtual void postUpdate();
    virtual void render(double step);
    virtual void end();

protected:
    Entity* m_entity;
    bool m_hasStarted;
};
