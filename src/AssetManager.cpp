#include "AssetManager.h"
#include "constants/assets.h"

AssetManager::AssetManager() : textureMap{} {};

Texture *AssetManager::getTexture(const std::string &key) {
    return AssetManager::get().textureMap[key];
}

void AssetManager::loadAssets() {
    for (auto image: assets::images) {
        Texture *texture = new Texture{image};
        AssetManager::get().textureMap.insert({image, texture});
    }
}
