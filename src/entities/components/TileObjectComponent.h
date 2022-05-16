#pragma once

#include "Component.h"
#include "Registry.h"

class TileObjectComponent : public Component {
public:
    COMPONENT getId() override;
    COMPONENT getType() override;
    std::set<COMPONENT> getRequiredComponents() override;

    TileObjectComponent(Entity *entity, const ObjectData &data);

    void start() override;

    std::vector<glm::ivec2> getTiles();

    ObjectData m_data;
private:
};
