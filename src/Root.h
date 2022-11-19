#pragma once

#include "common.h"
#include "Game.h"
#include "Stage.h"
#include "Zoo.h"
#include "UIManager.h"

class Root {
public:
    static Game& game();
    static Stage* zoo();
    static Renderer& renderer();
    static UIManager& ui();
    static AssetManager& assetManager();
    static SceneManager& sceneManager();
    static SaveManager& saveManager();
};
