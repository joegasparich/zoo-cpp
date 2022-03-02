#pragma once

#include <string>

class Scene {
public:
    std::string name;

    virtual void start();
    virtual void preUpdate();
    virtual void update();
    virtual void postUpdate();
    virtual void stop();
};
