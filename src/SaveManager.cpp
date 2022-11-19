#include "SaveManager.h"
#include "SceneManager.h"
#include "Zoo.h"

SaveManager::SaveManager() = default;
SaveManager::~SaveManager() = default;

void SaveManager::newGame() {
    Root::sceneManager().loadScene(std::make_unique<Zoo>());
}

void SaveManager::saveGame(std::string saveFilePath) {
    json saveData;
    try {
        saveData = json{
                {"zoo", Root::zoo()->save()}
        };
    } catch(const std::exception& error) {
        std::cout << "Error saving game " << std::endl;
        std::cout << error.what() << std::endl;
    }

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

        try {
            Root::zoo()->load(saveData["zoo"]);
        } catch(const std::exception& error) {
            std::cout << "Error loading save file: " << saveFilePath << std::endl;
            std::cout << error.what() << std::endl;
        }
    }

}
