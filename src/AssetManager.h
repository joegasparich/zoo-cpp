#pragma once

#include <map>
#include <SDL2/SDL.h>

#include "renderer/Texture.h"

class AssetManager {
public:
    AssetManager(const AssetManager &) = delete;

    static AssetManager &get() {
        static AssetManager instance;
        return instance;
    }

    static Texture *getTexture(const std::string &key);

    static void loadAssets();

private:
    AssetManager();

    std::map<std::string, Texture *> textureMap;
};
