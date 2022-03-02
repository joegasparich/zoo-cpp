#include <iostream>
#include "SceneManager.h"

void SceneManager::loadScene(Scene& scene) {
    if (currentScene) {
        std::cout << "Stopping scene: " << currentScene->name << std::endl;
        currentScene->stop();
    }

    currentScene = &scene;
}

Scene* SceneManager::getCurrentScene() {
    return currentScene;
}
