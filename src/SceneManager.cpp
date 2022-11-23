#include "SceneManager.h"

SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

void SceneManager::loadScene(std::unique_ptr<Scene> scene) {
    if (currentScene) {
        TraceLog(LOG_TRACE, "Stopping scene: %s", currentScene->name.c_str());
        currentScene->stop();
    }

    currentScene = std::move(scene);

    currentScene->start();
}

Scene* SceneManager::getCurrentScene() {
    return currentScene.get();
}