#pragma once

#include "common.h"
#include "SpriteSheet.h"

class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    void loadAssets();
    void loadObjects();
    void loadWalls();
    void loadPaths();

    SpriteSheet* loadSpriteSheet(const std::string& texturePath, int cellWidth, int cellHeight);
    SpriteSheet* getSpriteSheet(const std::string& texturePath);
    Texture* getTexture(const std::string& path);

private:

    std::map<std::string, std::unique_ptr<Texture>> textureMap;
    std::map<std::string, std::unique_ptr<SpriteSheet>> spriteSheetMap;
};
