#include "Root.h"
#include "SceneManager.h"
#include "Game.h"

Game& Root::game() {
    return Game::get();
}

Stage* Root::zoo() {
    auto scene = sceneManager().getCurrentScene();
    if (scene->name == ZOO) {
        return ((Zoo*)scene)->zoo.get();
    } else {
        return nullptr;
    }
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
