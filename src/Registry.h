#pragma once

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
    Image* image;
    SpriteSheet* spriteSheet;
    ObjectType type;
    glm::vec2 pivot;
    glm::vec2 size;
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

class Registry {
public:
    static void registerObject(const std::string& assetPath, const ObjectData& data);
    static void registerWall(const std::string& assetPath, const WallData& data);

    static ObjectData& getObject(const std::string& assetPath);
    static WallData& getWall(const std::string& assetPath);
    static std::vector<ObjectData*> getAllObjects();
    static std::vector<WallData*> getAllWalls();
private:
    static std::map<std::string, ObjectData> m_objectRegistry;
    static std::map<std::string, WallData> m_wallRegistry;
};
