#include "Registry.h"

std::map<std::string, ObjectData> Registry::objectRegistry{};
std::map<std::string, WallData> Registry::wallRegistry{};
std::map<std::string, PathData> Registry::pathRegistry{};

void Registry::registerObject(const std::string &assetPath, const ObjectData &data) {
    objectRegistry.insert_or_assign(assetPath, data);

    TraceLog(LOG_TRACE, "Registered Object: %s", data.name.c_str());
}

void Registry::registerWall(const std::string& assetPath, const WallData& data) {
    wallRegistry.insert_or_assign(assetPath, data);

    TraceLog(LOG_TRACE, "Registered Wall: %s", data.name.c_str());
}

void Registry::registerPath(const std::string &assetPath, const PathData &data) {
    pathRegistry.insert_or_assign(assetPath, data);

    TraceLog(LOG_TRACE, "Registered Path: %s", data.name.c_str());
}

ObjectData& Registry::getObject(const std::string &assetPath) {
    return objectRegistry.at(assetPath);
}

WallData& Registry::getWall(const std::string& assetPath) {
    return wallRegistry.at(assetPath);
}

PathData &Registry::getPath(const std::string &assetPath) {
    return pathRegistry.at(assetPath);
}

std::vector<ObjectData*> Registry::getAllObjects() {
    std::vector<ObjectData*> objects;
    objects.reserve(objectRegistry.size());

    for (const auto& objectPair : objectRegistry) {
        objects.push_back(&objectRegistry.at(objectPair.first));
    }

    return objects;
}

std::vector<WallData*> Registry::getAllWalls() {
    std::vector<WallData*> walls;
    walls.reserve(wallRegistry.size());

    for (const auto& wallPair : wallRegistry) {
        walls.push_back(&wallRegistry.at(wallPair.first));
    }

    return walls;
}

std::vector<PathData *> Registry::getAllPaths() {
    std::vector<PathData*> paths;
    paths.reserve(pathRegistry.size());

    for (const auto& pathPair : pathRegistry) {
        paths.push_back(&pathRegistry.at(pathPair.first));
    }

    return paths;
}
