#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

#include "AssetManager.h"
#include "Registry.h"
#include "constants/assets.h"

using json = nlohmann::json;

AssetManager::AssetManager() : m_textureMap{} {};

Texture *AssetManager::getTexture(const std::string &key) {
    return AssetManager::get().m_textureMap[key];
}

SpriteSheet *AssetManager::getSpriteSheet(const std::string &key) {
    return AssetManager::get().m_spriteSheetMap[key];
}

void AssetManager::loadAssets() {
    for (auto path: assets::images) {
        // TODO: smart pointer?
        Texture *texture = new Texture{path};
        AssetManager::get().m_textureMap.insert({path, texture});
    }
}

void AssetManager::loadWalls() {
    for (auto path : assets::walls) {
        // Load file
        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();

        // Parse JSON
        auto json = json::parse(buffer.str());

        // Map to Wall
        auto imagePath = json["spriteSheet"].get<std::string>();
        auto texture = new Texture{imagePath};
        loadSpriteSheet(imagePath, texture, json["cellWidth"].get<int>(),json["cellHeight"].get<int>());

        Registry::registerWall(path, {
                path,
                json["name"].get<std::string>(),
                json["type"].get<std::string>(),
                json["solid"].get<bool>(),
                imagePath
        });
    }
}

SpriteSheet* AssetManager::loadSpriteSheet(std::string assetPath, Texture* texture, int cellWidth, int cellHeight) {
    // TODO: smart pointer?
    auto spriteSheet = new SpriteSheet{
         cellWidth,
         cellHeight,
         texture
     };

    AssetManager::get().m_spriteSheetMap.insert({assetPath, spriteSheet});
}
