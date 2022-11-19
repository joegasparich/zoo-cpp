#pragma once

#include "common.h"
#include "UIManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "SceneManager.h"
#include "SaveManager.h"

// TODO: Config options
#define MS_PER_UPDATE 10

class Game {
public:
    Game(const Game &) = delete;

    static Game &get() {
        static Game instance;
        return instance;
    }

    void run();
    void onInput(InputEvent* event);
    static int getTicks();

    UIManager ui;
    Renderer renderer;
    AssetManager assetManager;
    SceneManager sceneManager;
    SaveManager saveManager;
private:
    Game();
    ~Game();

    void init();
    void doLoop();
    void render(double step);

    // TODO: Save this
    int ticksSinceGameStart = 0;
    int framesSinceGameStart = 0;
};
