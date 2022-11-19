#pragma once

#include "common.h"
#include "Scene.h"

class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    // TODO: Add progress callback
    void loadScene(std::unique_ptr<Scene> scene);
    Scene* getCurrentScene();

private:
    std::unique_ptr<Scene> currentScene;
};
