#pragma once

#include "pch.h"
#include "WallGrid.h"

class Area {
public:
    Area(std::string id);
    virtual ~Area();

    void addAreaConnection(Area* area, Wall* door);
    void removeAreaConnection(Area* area, Wall* door);

    std::string m_id;
    glm::vec3 m_colour;
    std::vector<glm::ivec2> m_tiles;
    std::unordered_map<Area*, std::unordered_set<Wall*>> m_connectedAreas;
};
