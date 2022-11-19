#pragma once

#include "common.h"

class SaveManager {
public:
    SaveManager();
    ~SaveManager();

    void newGame();
    void saveGame(std::string saveFilePath);
    void loadGame(std::string saveFilePath);
};
