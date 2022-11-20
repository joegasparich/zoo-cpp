#pragma once

#include "common.h"
#include "Component.h"

class SolidComponent : public Component {
public:
    SolidComponent(Entity* entity);

    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    void start() override;
    void end() override;

    void setTiles(std::vector<Cell> tiles);

private:
    std::vector<Cell> tiles;
};
