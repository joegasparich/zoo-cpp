#include "Root.h"

#include "SceneManager.h"
#include "Game.h"

Game& Root::game() {
    return Game::get();
}

Zoo* Root::zoo() {
    auto scene = sceneManager().getCurrentScene();
    if (scene->name == ZOO) {
        return ((ZooScene*)scene)->zoo.get();
    } else {
        return nullptr;
    }
}

InputManager& Root::input() {
    return game().input;
}
Renderer& Root::renderer() {
    return game().renderer;
}

UIManager& Root::ui() {
    return game().ui;
}

AssetManager& Root::assetManager() {
    return game().assetManager;
}

SceneManager& Root::sceneManager() {
    return game().sceneManager;
}

SaveManager& Root::saveManager() {
    return game().saveManager;
}
