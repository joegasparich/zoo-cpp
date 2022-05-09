#pragma once

#include <SDL2/SDL.h>

#include "pch.h"
#include <renderer/Image.h>
#include <renderer/SpriteSheet.h>
#include <renderer/Texture.h>

class AssetManager {
public:
    AssetManager(const AssetManager &) = delete;

    static AssetManager &get() {
        static AssetManager instance;
        return instance;
    }

    static Image* getImage(const std::string &key);
    static SpriteSheet* getSpriteSheet(const std::string &key);

    static void loadAssets();
    static void loadWalls();

    static Image* loadImage(const std::string& path);
    static SpriteSheet* loadSpriteSheet(const std::string& assetPath, Image* image, int cellWidth, int cellHeight);
    static Texture* loadTexture(Image* image);

private:
    AssetManager();

    std::map<std::string, std::unique_ptr<Image>> m_imageMap;
    std::map<std::string, std::unique_ptr<SpriteSheet>> m_spriteSheetMap;
    std::map<std::string, std::unique_ptr<Texture>> m_textureMap;
};
