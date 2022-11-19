#pragma once

#include "common.h"
#include "AssetManager.h"

enum class ObjectType {
    Foliage,
    Building,
    Consumable,
    Misc
};

inline ObjectType getObjectType(const std::string& typeString) {
    if (typeString == "foliage") return ObjectType::Foliage;
    if (typeString == "building") return ObjectType::Building;
    if (typeString == "consumable") return ObjectType::Consumable;
    return ObjectType::Misc;
}

struct ObjectData {
    std::string assetPath;
    std::string name;
    std::string spritePath;
    std::string spriteSheetPath;
    ObjectType type;
    Vector2 pivot;
    Vector2 size;
    bool solid;
    bool canPlaceOnSlopes;
    bool canPlaceInWater;
};

struct WallData {
    std::string assetPath;
    std::string name;
    std::string type;
    bool solid;
    std::string spriteSheetPath;
};

struct PathData {
    std::string assetPath;
    std::string name;
    std::string spriteSheetPath;
};

class Registry {
public:
    static void registerObject(const std::string& assetPath, const ObjectData& data);
    static void registerWall(const std::string& assetPath, const WallData& data);
    static void registerPath(const std::string& assetPath, const PathData& data);

    static ObjectData& getObject(const std::string& assetPath);
    static WallData& getWall(const std::string& assetPath);
    static PathData& getPath(const std::string& assetPath);
    static std::vector<ObjectData*> getAllObjects();
    static std::vector<WallData*> getAllWalls();
    static std::vector<PathData*> getAllPaths();
private:
    static std::map<std::string, ObjectData> objectRegistry;
    static std::map<std::string, WallData> wallRegistry;
    static std::map<std::string, PathData> pathRegistry;
};
