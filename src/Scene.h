#pragma once

#include "pch.h"

class Scene {
public:
    Scene(std::string name) : m_name(std::move(name)) {}
    virtual ~Scene() {}

    std::string m_name;

    virtual void start() = 0;
    virtual void preUpdate() = 0;
    virtual void update() = 0;
    virtual void postUpdate() = 0;
    virtual void render(double step) = 0;
    virtual void stop() = 0;
};
