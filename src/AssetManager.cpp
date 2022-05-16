#include <util/json.h>
#include "AssetManager.h"
#include "Registry.h"
#include "constants/assets.h"
#include "Debug.h"

AssetManager::AssetManager() : m_imageMap{} {};

Image* AssetManager::getImage(const std::string &key) {
    if (!get().m_imageMap.contains(key)) {
        return nullptr;
    }

    return get().m_imageMap[key].get();
}

SpriteSheet *AssetManager::getSpriteSheet(const std::string &key) {
    if (!get().m_spriteSheetMap.contains(key)) {
        return nullptr;
    }

    return get().m_spriteSheetMap[key].get();
}

void AssetManager::loadAssets() {
    for (auto path: assets::images) {
        get().loadImage(path);
    }
}

void AssetManager::loadObjects() {
    for (auto path : assets::objects) {
        try {
            auto j = readJSON(path);

            // Map to Object
            Image* image = nullptr;
            SpriteSheet* spriteSheet = nullptr;
            if (j["sprite"].is_string()) {
                image = loadImage(j["sprite"].get<std::string>());
            }
            if (j["spriteSheet"].is_string()) {
                spriteSheet = loadSpriteSheet(
                    j["spriteSheet"].get<std::string>(),
                    loadImage(j["spriteSheet"].get<std::string>()),
                    j["cellWidth"].get<int>(),
                    j["cellHeight"].get<int>()
                );
            }

            Registry::registerObject(path, {
                    path,
                    j["name"].get<std::string>(),
                    image,
                    spriteSheet,
                    getObjectType(j["type"].get<std::string>()),
                    fromJSON(j["pivot"]),
                    fromJSON(j["size"]),
                    j["solid"].get<bool>(),
                    j["canPlaceOnSlopes"].get<bool>(),
                    j["canPlaceInWater"].get<bool>()

            });
        } catch(const std::exception& error) {
            std::cout << "Error loading object: " << path << std::endl;
            std::cout << error.what() << std::endl;
        }
    }
}

void AssetManager::loadWalls() {
    for (auto path : assets::walls) {
        try {
            auto json = readJSON(path);

            // Map to Wall
            auto imagePath = json["spriteSheet"].get<std::string>();
            auto image = loadImage(imagePath);
            loadSpriteSheet(imagePath, image, json["cellWidth"].get<int>(),json["cellHeight"].get<int>());

            Registry::registerWall(path, {
                    path,
                    json["name"].get<std::string>(),
                    json["type"].get<std::string>(),
                    json["solid"].get<bool>(),
                    imagePath
            });
        } catch(const std::exception& error) {
            std::cout << "Error loading wall: " << path << std::endl;
            std::cout << error.what() << std::endl;
        }
    }
}

Image* AssetManager::loadImage(const std::string &path) {
    if (!get().m_imageMap.contains(path)) {
        get().m_imageMap.insert({path, std::make_unique<Image>(path)});
    }

    return get().m_imageMap[path].get();
}

SpriteSheet* AssetManager::loadSpriteSheet(const std::string& assetPath, Image* image, int cellWidth, int cellHeight) {
    if (!image) return nullptr;

    if (!get().m_spriteSheetMap.contains(assetPath)) {
        get().m_spriteSheetMap.insert({assetPath, std::make_unique<SpriteSheet>(
            SpriteSheet{
                cellWidth,
                cellHeight,
                image
            }
        )});
    }

    return get().m_spriteSheetMap[assetPath].get();
}

Texture* AssetManager::loadTexture(Image* image) {
    if (!image) return nullptr;

    auto& texmap = get().m_textureMap;
    if (get().m_textureMap.contains(image->m_filePath)) {
        return get().m_textureMap.at(image->m_filePath).get();
    }

    get().m_textureMap.insert({image->m_filePath, std::make_unique<Texture>(image)});
    return get().m_textureMap.at(image->m_filePath).get();
}
