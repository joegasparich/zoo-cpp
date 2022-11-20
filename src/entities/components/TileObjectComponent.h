#pragma once

#include "Component.h"
#include "Registry.h"

class TileObjectComponent : public Component {
public:
    typedef TileObjectComponent base;

    TileObjectComponent(Entity *entity);
    TileObjectComponent(Entity *entity, ObjectData data);

    COMPONENT getId() override;

    void start() override;

    std::vector<Cell> getTiles();

    json save() override;
    void load(json data) override;

    ObjectData data;
private:
};
