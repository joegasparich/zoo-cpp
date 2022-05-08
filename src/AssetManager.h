#pragma once

#include <SDL2/SDL.h>

#include "pch.h"
#include <renderer/Image.h>
#include <renderer/SpriteSheet.h>

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
    static SpriteSheet* loadSpriteSheet(std::string assetPath, Image* image, int cellWidth, int cellHeight);

private:
    AssetManager();

    std::map<std::string, std::unique_ptr<Image>> m_imageMap;
    std::map<std::string, std::unique_ptr<SpriteSheet>> m_spriteSheetMap;
};
