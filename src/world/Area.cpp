#include <utility>
#include "Area.h"

Area::Area(std::string id) :
    m_id{std::move(id)},
    m_colour{randomChar(), randomChar(), randomChar(), 255}
{
    m_connectedAreas = {};
}
Area::~Area() = default;

void Area::addAreaConnection(Area* area, Wall* door) {
    if (!door->isDoor) return; // Wall isn't a door
    if (m_connectedAreas.contains(area) && m_connectedAreas.at(area).contains(door)) return; // Duplicate door
    if (area == this) return; // Don't add connection to itself

    if (m_connectedAreas.contains(area)) {
        m_connectedAreas.at(area).insert(door);
    } else {
        m_connectedAreas[area] = {door};
    }
}

void Area::removeAreaConnection(Area* area, Wall* door) {
    if (!door->isDoor) return; // Wall isn't a door
    if (!m_connectedAreas.contains(area)) return; // Area not a connection
    if (door && !m_connectedAreas.at(area).contains(door)) return; // Door not a connection

    if (door) {
        this->m_connectedAreas.at(area).erase(door);

        if (m_connectedAreas.at(area).empty()) {
            m_connectedAreas.erase(area);
        }
    } else {
        m_connectedAreas.erase(area);
    }
}
