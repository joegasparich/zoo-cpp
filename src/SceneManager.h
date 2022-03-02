#pragma once

#include "Scene.h"

class SceneManager {
public:
    // TODO: Add progress callback
    void loadScene(Scene& scene);
    Scene* getCurrentScene();

private:
    Scene* currentScene;
};
