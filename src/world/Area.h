#pragma once

#include "pch.h"

class Area {
public:
    Area(std::string id);
    virtual ~Area();

    std::string m_id;
    glm::vec3 m_colour;
    std::vector<glm::ivec2> m_tiles;
    // connectedAreas TODO: with gates
private:
};
