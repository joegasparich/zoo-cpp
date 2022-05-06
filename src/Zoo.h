#pragma once

#include "Scene.h"
#include "Stage.h"

class Zoo : public Scene {
public:
    Zoo();
    virtual ~Zoo();

    void start() override;
    void preUpdate() override;
    void update() override;
    void postUpdate() override;
    void render(double step) override;
    void stop() override;

    static std::unique_ptr<Stage> zoo;
};
