#pragma once

#include "pch.h"

#include "Scene.h"

class SceneManager {
public:
    static SceneManager &get() {
        static SceneManager instance;
        return instance;
    }

    // TODO: Add progress callback
    static void loadScene(std::unique_ptr<Scene> scene);
    static Scene* getCurrentScene();

private:
    std::unique_ptr<Scene> currentScene;
};
