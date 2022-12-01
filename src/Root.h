#pragma once

#include "common.h"
#include "Game.h"
#include "Zoo.h"
#include "ZooScene.h"
#include "UIManager.h"

class Root {
public:
    static Game& game();
    static Zoo* zoo();
    static InputManager& input();
    static Renderer& renderer();
    static UIManager& ui();
    static AssetManager& assetManager();
    static SceneManager& sceneManager();
    static SaveManager& saveManager();
};
