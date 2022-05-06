#include "Registry.h"

std::map<std::string, WallData> Registry::m_wallRegistry{};

void Registry::registerWall(std::string assetPath, WallData data) {
    m_wallRegistry.insert({assetPath, data});

    std::cout << "Registered Wall: " << data.name << std::endl;
}

WallData Registry::getWall(std::string assetPath) {
    return m_wallRegistry.at(assetPath);
}
