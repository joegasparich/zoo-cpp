#pragma once

#include "AssetManager.h"

struct WallData {
    std::string assetPath;
    std::string name;
    std::string type;
    bool solid;
    std::string spriteSheetPath;
};

class Registry {
public:
    static void registerWall(const std::string& assetPath, const WallData& data);

    static WallData& getWall(const std::string& assetPath);
    static std::vector<WallData*> getAllWalls();
private:
    static std::map<std::string, WallData> m_wallRegistry;
};
