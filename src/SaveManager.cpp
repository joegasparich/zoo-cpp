#include "SaveManager.h"
#include "SceneManager.h"
#include "Zoo.h"

json saveData;

void SaveManager::newGame() {
    SceneManager::loadScene(std::make_unique<Zoo>());
}

void SaveManager::saveGame(std::string saveFilePath) {
    saveData = {
        {"world", Zoo::zoo->m_world->save()}
    };
}

void SaveManager::loadGame(std::string saveFilePath) {
    Zoo::zoo->m_world->load(saveData["world"]);
}
