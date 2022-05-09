#include "AssetManager.h"
#include "Registry.h"
#include "constants/assets.h"

AssetManager::AssetManager() : m_imageMap{} {};

Image* AssetManager::getImage(const std::string &key) {
    return get().m_imageMap[key].get();
}

SpriteSheet *AssetManager::getSpriteSheet(const std::string &key) {
    return get().m_spriteSheetMap[key].get();
}

void AssetManager::loadAssets() {
    for (auto path: assets::images) {
        get().loadImage(path);
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

        std::cout << json.dump() << std::endl;

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
    }
}

Image* AssetManager::loadImage(const std::string &path) {
    if (!get().m_imageMap.contains(path)) {
        get().m_imageMap.insert({path, std::make_unique<Image>(path)});
    }

    return get().m_imageMap[path].get();
}

SpriteSheet* AssetManager::loadSpriteSheet(const std::string& assetPath, Image* image, int cellWidth, int cellHeight) {
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
    auto& texmap = get().m_textureMap;
    if (get().m_textureMap.contains(image->m_filePath)) {
        return get().m_textureMap.at(image->m_filePath).get();
    }

    get().m_textureMap.insert({image->m_filePath, std::make_unique<Texture>(image)});
    return get().m_textureMap.at(image->m_filePath).get();
}
