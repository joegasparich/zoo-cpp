#pragma once

#include "pch.h"

class SaveManager {
public:
    static void newGame();
    static void saveGame(std::string saveFilePath);
    static void loadGame(std::string saveFilePath);
};
