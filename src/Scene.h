#pragma once

#include "common.h"
#include "InputManager.h"

class Scene {
public:
    Scene(std::string name) : name(std::move(name)) {}
    virtual ~Scene() {}

    std::string name;

    virtual void start() = 0;
    virtual void preUpdate() = 0;
    virtual void update() = 0;
    virtual void postUpdate() = 0;
    virtual void render(double step) = 0;
    virtual void renderLate(double step) = 0;
    virtual void onGUI() = 0;
    virtual void onInput(InputEvent* event) = 0;
    virtual void stop() = 0;
};