#include "Registry.h"

std::map<std::string, ObjectData> Registry::m_objectRegistry{};
std::map<std::string, WallData> Registry::m_wallRegistry{};
std::map<std::string, PathData> Registry::m_pathRegistry{};

void Registry::registerObject(const std::string &assetPath, const ObjectData &data) {
    m_objectRegistry.insert_or_assign(assetPath, data);

    std::cout << "Registered Object: " << data.name << std::endl;
}

void Registry::registerWall(const std::string& assetPath, const WallData& data) {
    m_wallRegistry.insert_or_assign(assetPath, data);

    std::cout << "Registered Wall: " << data.name << std::endl;
}

void Registry::registerPath(const std::string &assetPath, const PathData &data) {
    m_pathRegistry.insert_or_assign(assetPath, data);

    std::cout << "Registered Path: " << data.name << std::endl;
}

ObjectData& Registry::getObject(const std::string &assetPath) {
    return m_objectRegistry.at(assetPath);
}

WallData& Registry::getWall(const std::string& assetPath) {
    return m_wallRegistry.at(assetPath);
}

PathData &Registry::getPath(const std::string &assetPath) {
    return m_pathRegistry.at(assetPath);
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

std::vector<PathData *> Registry::getAllPaths() {
    std::vector<PathData*> paths;
    paths.reserve(m_pathRegistry.size());

    for (const auto& pathPair : m_pathRegistry) {
        paths.push_back(&m_pathRegistry.at(pathPair.first));
    }

    return paths;
}
