#include "Registry.h"

std::map<std::string, ObjectData> Registry::m_objectRegistry{};
std::map<std::string, WallData> Registry::m_wallRegistry{};

void Registry::registerObject(const std::string &assetPath, const ObjectData &data) {
    m_objectRegistry.insert({assetPath, data});

    std::cout << "Registered Object: " << data.name << std::endl;
}

void Registry::registerWall(const std::string& assetPath, const WallData& data) {
    m_wallRegistry.insert({assetPath, data});

    std::cout << "Registered Wall: " << data.name << std::endl;
}

ObjectData& Registry::getObject(const std::string &assetPath) {
    return m_objectRegistry.at(assetPath);
}

WallData& Registry::getWall(const std::string& assetPath) {
    return m_wallRegistry.at(assetPath);
}

std::vector<ObjectData*> Registry::getAllObjects() {
    std::vector<ObjectData*> objects;
    objects.reserve(m_objectRegistry.size());

    for (const auto& objectPair : m_objectRegistry) {
        objects.push_back(&m_objectRegistry.at(objectPair.first));
    }

    return objects;
}

std::vector<WallData*> Registry::getAllWalls() {
    std::vector<WallData*> walls;
    walls.reserve(m_wallRegistry.size());

    for (const auto& wallPair : m_wallRegistry) {
        walls.push_back(&m_wallRegistry.at(wallPair.first));
    }

    return walls;
}
