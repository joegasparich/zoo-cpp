#include "SceneManager.h"

SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

void SceneManager::loadScene(std::unique_ptr<Scene> scene) {
    if (currentScene) {
        std::cout << "Stopping scene: " << currentScene->name << std::endl;
        currentScene->stop();
    }

    currentScene = std::move(scene);

    currentScene->start();
}

Scene* SceneManager::getCurrentScene() {
    return currentScene.get();
}