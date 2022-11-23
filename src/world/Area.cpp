#include <utility>
#include "Area.h"

Area::Area(std::string id) :
    id{std::move(id)},
    colour{randomChar(), randomChar(), randomChar(), 255}
{
    connectedAreas = {};
}
Area::~Area() = default;

void Area::addAreaConnection(Area* area, Wall* door) {
    assert(door->isDoor);
    if (connectedAreas.contains(area) && connectedAreas.at(area).contains(door)) return; // Duplicate door
    if (area == this) return; // Don't add connection to itself

    if (connectedAreas.contains(area)) {
        connectedAreas.at(area).insert(door);
    } else {
        connectedAreas[area] = {door};
    }
}

void Area::removeAreaConnection(Area* area, Wall* door) {
    assert(door->isDoor);
    if (!connectedAreas.contains(area)) return; // Area not a connection
    if (door && !connectedAreas.at(area).contains(door)) return; // Door not a connection

    if (door) {
        this->connectedAreas.at(area).erase(door);

        if (connectedAreas.at(area).empty()) {
            connectedAreas.erase(area);
        }
    } else {
        connectedAreas.erase(area);
    }
}
