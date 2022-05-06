#pragma once

#include <SDL2/SDL.h>

#include "Stage.h"
#include "InputManager.h"

// TODO: Config options
#define MS_PER_UPDATE 10

enum class GameState {
    PLAY, EXIT
};

class Game {
public:
    static Game &get() {
        static Game instance;
        return instance;
    }

    void run();

    std::unique_ptr<InputManager> m_input;

private:
    Game();
    ~Game();

    void init();
    void doLoop();
    void pollEvents();
    void render(double step) const;

    GameState m_state;
};
