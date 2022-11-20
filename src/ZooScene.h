#pragma once

#include "Root.h"
#include "common.h"
#include "Scene.h"
#include "Zoo.h"

#define ZOO "Zoo"

class ZooScene : public Scene {
public:
    ZooScene();
    virtual ~ZooScene();

    void start() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void render(double step) override;
    void onGUI() override;
    void stop() override;

    void onInput(InputEvent* event) override;

    std::unique_ptr<Zoo> zoo;

    // Temp
    Texture texture;
};

