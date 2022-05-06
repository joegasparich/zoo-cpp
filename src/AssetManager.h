#pragma once

#include <map>
#include <SDL2/SDL.h>

#include "renderer/Texture.h"

struct SpriteSheet {
    int cellWidth;
    int cellHeight;
    Texture* texture;
};

class AssetManager {
public:
    AssetManager(const AssetManager &) = delete;

    static AssetManager &get() {
        static AssetManager instance;
        return instance;
    }

    static Texture* getTexture(const std::string &key);
    static SpriteSheet* getSpriteSheet(const std::string &key);

    static void loadAssets();
    static void loadWalls();

    static SpriteSheet* loadSpriteSheet(std::string assetPath, Texture* texture, int cellWidth, int cellHeight);

private:
    AssetManager();

    std::map<std::string, Texture*> m_textureMap;
    std::map<std::string, SpriteSheet*> m_spriteSheetMap;
};
