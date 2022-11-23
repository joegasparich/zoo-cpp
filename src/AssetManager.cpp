#include "AssetManager.h"
#include "Registry.h"
#include "SpriteSheet.h"
#include "constants/assets.h"
#include "util/json.h"

AssetManager::AssetManager() : textureMap{} {};
AssetManager::~AssetManager() = default;

void AssetManager::loadAssets() {
    for (auto path: assets::images) {
        getTexture(path);
    }
}

void AssetManager::loadObjects() {
    for (auto path : assets::objects) {
        try {
            auto j = readJSON(ASSETS_PATH"/" + path);

            // Map to Object
            std::string spritePath = "";
            std::string spriteSheetPath = "";
            if (j["spriteSheet"].is_string()) {
                spriteSheetPath = j["spriteSheet"].get<std::string>();
                loadSpriteSheet(
                    spriteSheetPath,
                    j["cellWidth"].get<int>(),
                    j["cellHeight"].get<int>()
                );
            }
            if (j["sprite"].is_string()) {
                spritePath = j["sprite"].get<std::string>();
            }

            Registry::registerObject(path, {
                path,
                j["name"].get<std::string>(),
                spritePath,
                spriteSheetPath,
                getObjectType(j["type"].get<std::string>()),
                j["pivot"].get<Vector2>(),
                j["size"].get<Vector2>(),
                j["solid"].get<bool>(),
                j["canPlaceOnSlopes"].get<bool>(),
                j["canPlaceInWater"].get<bool>()

            });
        } catch(const std::exception& error) {
            TraceLog(LOG_WARNING, "Error loading object: %s", path.c_str());
            TraceLog(LOG_WARNING, error.what());
        }
    }
}

void AssetManager::loadWalls() {
    for (auto path : assets::walls) {
        try {
            auto json = readJSON(ASSETS_PATH"/" + path);

            // Map to Wall
            auto imagePath = json["spriteSheet"].get<std::string>();
            loadSpriteSheet(imagePath, json["cellWidth"].get<int>(), json["cellHeight"].get<int>());

            Registry::registerWall(path, {
                path,
                json["name"].get<std::string>(),
                json["type"].get<std::string>(),
                json["solid"].get<bool>(),
                imagePath
            });
        } catch(const std::exception& error) {
            TraceLog(LOG_WARNING, "Error loading wall: %s", path.c_str());
            TraceLog(LOG_WARNING, error.what());
        }
    }
}

void AssetManager::loadPaths() {
    for (auto path : assets::paths) {
        try {
            auto json = readJSON(ASSETS_PATH"/" + path);

            // Map to Wall
            auto imagePath = json["spriteSheet"].get<std::string>();
            loadSpriteSheet(imagePath, json["cellWidth"].get<int>(), json["cellHeight"].get<int>());

            Registry::registerPath(path, {
                path,
                json["name"].get<std::string>(),
                imagePath
            });
        } catch(const std::exception& error) {
            TraceLog(LOG_WARNING, "Error loading path: %s", path.c_str());
            TraceLog(LOG_WARNING, error.what());
        }
    }
}

Texture* AssetManager::getTexture(const std::string &path) {
    if (!textureMap.contains(path)) {
        textureMap.insert_or_assign(
            path,
            std::make_unique<Texture>(LoadTexture(
                (ASSETS_PATH"/" + path).c_str()))
        );
    }

    return textureMap[path].get();
}

SpriteSheet* AssetManager::loadSpriteSheet(const std::string& texturePath, int cellWidth, int cellHeight) {
    if (texturePath.empty()) return nullptr;

    if (!spriteSheetMap.contains(texturePath)) {
        spriteSheetMap.insert_or_assign(texturePath, std::make_unique<SpriteSheet>(
            SpriteSheet{
                cellWidth,
                cellHeight,
                texturePath,
                AssetManager::getTexture(texturePath)
            }
        ));
    }

    return spriteSheetMap[texturePath].get();
}

SpriteSheet* AssetManager::getSpriteSheet(const std::string& texturePath) {
    if (texturePath.empty()) return nullptr;
    if (!spriteSheetMap.contains(texturePath)) return nullptr;

    return spriteSheetMap[texturePath].get();
}
