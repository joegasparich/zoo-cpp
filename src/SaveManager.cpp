#include "SaveManager.h"
#include "SceneManager.h"
#include "ZooScene.h"

SaveManager::SaveManager() = default;
SaveManager::~SaveManager() = default;

void SaveManager::newGame() {
    Root::sceneManager().loadScene(std::make_unique<ZooScene>());
}

void SaveManager::saveGame(std::string saveFilePath) {
    json saveData{};
    try {
        mode = SerialiseMode::Saving;
        curr = &saveData;
        Root::zoo()->serialise();

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
            mode = SerialiseMode::Loading;
            curr = &saveData;
            Root::zoo()->serialise();
        } catch(const std::exception& error) {
            std::cout << "Error loading save file: " << saveFilePath << std::endl;
            std::cout << error.what() << std::endl;
        }
    }
}

json* SaveManager::getCurrentSerialiseNode () {
    return curr;
}

void SaveManager::setCurrentSerialiseNode (json* node) {
    curr = node;
}
