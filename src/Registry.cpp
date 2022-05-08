#include "Registry.h"

std::map<std::string, WallData> Registry::m_wallRegistry{};

void Registry::registerWall(const std::string& assetPath, const WallData& data) {
    m_wallRegistry.insert({assetPath, data});

    std::cout << "Registered Wall: " << data.name << std::endl;
}

WallData& Registry::getWall(const std::string& assetPath) {
    return m_wallRegistry.at(assetPath);
}

std::vector<WallData*> Registry::getAllWalls() {
    std::vector<WallData*> walls;
    walls.reserve(m_wallRegistry.size());

    for (auto wallPair : m_wallRegistry) {
        walls.push_back(&m_wallRegistry.at(wallPair.first));
    }

    return walls;
}
