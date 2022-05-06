#include "SaveManager.h"
#include "SceneManager.h"
#include "Zoo.h"

void SaveManager::newGame() {
    SceneManager::loadScene(std::make_unique<Zoo>());
}

void SaveManager::saveGame(std::string saveFilePath) {
    auto saveData = json{
        {"world", Zoo::zoo->m_world->save()}
    };

    std::ofstream file;
    file.open(saveFilePath);
    file << saveData.dump();
    file.close();
}

void SaveManager::loadGame(std::string saveFilePath) {
    std::ifstream file;
    file.open(saveFilePath);

    if (file.is_open()) {
        std::string contents;
        std::string line;
        while (std::getline(file,line)) {
            contents += line + '\n';
        }
        file.close();

        auto saveData = json::parse(contents);
        if (saveData.empty()) return;

        Zoo::zoo->m_world->load(saveData["world"]);
    }

}
