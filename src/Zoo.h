#pragma once

#include "Root.h"
#include "common.h"
#include "Scene.h"
#include "Stage.h"

#define ZOO "Zoo"

class Zoo : public Scene {
public:
    Zoo();
    virtual ~Zoo();

    void start() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void render(double step) override;
    void onGUI() override;
    void stop() override;

    void onInput(InputEvent* event) override;

    std::unique_ptr<Stage> zoo;

    // Temp
    Texture texture;
};

