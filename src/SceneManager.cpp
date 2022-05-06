#include "SceneManager.h"

void SceneManager::loadScene(std::unique_ptr<Scene> scene) {
    auto& sceneManager = SceneManager::get();

    if (sceneManager.currentScene) {
        std::cout << "Stopping scene: " << sceneManager.currentScene->m_name << std::endl;
        sceneManager.currentScene->stop();
    }

    sceneManager.currentScene = std::move(scene);

    sceneManager.currentScene->start();
}

Scene* SceneManager::getCurrentScene() {
    auto& sceneManager = SceneManager::get();

    return sceneManager.currentScene.get();
}
